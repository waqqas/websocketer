#ifndef WEBSOCKETER_H
#define WEBSOCKETER_H

#include "error_code.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <list>
#include <memory>
#include <string_view>
#include <uriparserpp/uriparserpp.hpp>
#include <utility>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace http      = beast::http;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
using tcp           = boost::asio::ip::tcp;

namespace details {

struct async_intiate_resolve
{
  tcp::resolver &              resolver;
  const std::string &          host;
  const std::string &          port;
  tcp::resolver::results_type &_resolver_results;

  template <typename Self>
  void operator()(Self &self)
  {
    resolver.async_resolve(host, port, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error,
                  const tcp::resolver::results_type &results)
  {
    _resolver_results = results;
    self.complete(error);
  }
};

template <typename CompletionToken>
auto async_resolve(tcp::resolver &resolver, std::string &host, std::string &port,
                   tcp::resolver::results_type &results, CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &)>(
      details::async_intiate_resolve{resolver, host, port, results}, token, resolver);
}

struct async_intiate_connect
{
  websocket::stream<beast::tcp_stream> &stream_;
  tcp::resolver::results_type &         results_;

  template <typename Self>
  void operator()(Self &self)
  {
    stream_.async_connect(results_, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error,
                  const tcp::resolver::results_type::endpoint_type &)
  {
    self.complete(error);
  }
};

template <typename CompletionToken>
auto async_connect(websocket::stream<beast::tcp_stream> &stream,
                   tcp::resolver::results_type &results, CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &)>(
      details::async_intiate_connect{stream, results}, token, stream);
}

class session : public std::enable_shared_from_this<session>, private boost::asio::noncopyable
{
  friend struct async_initiate_request;

private:
  std::string &                        host_;
  std::string &                        port_;
  websocket::stream<beast::tcp_stream> stream_;
  tcp::resolver                        resolver_;
  tcp::resolver::results_type          results_;
  bool                                 is_free_;

public:
  session(net::io_context &io, std::string &host, std::string &port)
    : host_(host)
    , port_(port)
    , stream_(io)
    , resolver_(io)
    , is_free_(true)
  {}

  bool is_free()
  {
    return is_free_;
  }

  template <typename CompletionToken>
  auto compose(std::string_view req, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              const std::size_t bytes_transferred)>::return_type;
};

struct async_initiate_request
{

  std::shared_ptr<session> session_;
  std::string_view         message_;

  enum
  {
    resolving,
    connecting,
    handshaking,
  } state_;
  std::unique_ptr<beast::flat_buffer> buffer_;
  std::string_view                    res_;

  async_initiate_request(std::shared_ptr<session> session, const std::string_view &message)
    : session_(session)
    , message_(message)
    , state_(resolving)
  {
    session_->is_free_ = false;
    buffer_            = std::make_unique<boost::beast::flat_buffer>();
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error,
                  const tcp::resolver::results_type &results)
  {
    BOOST_ASSERT(state_ == resolving);
    if (!error)
    {

      session_->results_ = results;
      state_             = connecting;
      async_connect(session_->stream_, session_->results_, std::move(self));
      return;
    }
    req_.reset();
    buffer_.reset();

    // set error in case of non-2xx response
    if (res_->result_int() < 200 || res_->result_int() >= 300)
    {
      self.complete(make_error_code(ws_errc::status_code), res_);
    }
    else
    {
      self.complete(error, res_);
    }

    session_->is_free_ = true;
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error = {}, const std::size_t = 0)
  {
    if (!error)
    {
      switch (state_)
      {
      case resolving: {
        // go directly to connecting, if we have already resolved it
        if (session_->results_.empty())
        {
          async_resolve(session_->resolver_, session_->host_, session_->port_, session_->results_,
                        std::move(self));
        }
        else
        {
          state_ = connecting;
          async_connect(session_->stream_, session_->results_, std::move(self));
        }
        return;
      }
      case connecting: {
        state_ = writing;
        (*this)(self);
        return;
      }
      case writing: {
        state_ = reading;
        http::async_write(session_->stream_, *req_, std::move(self));
        return;
      }
      case reading: {
        state_ = waiting_for_response;
        http::async_read(session_->stream_, *buffer_, *res_, std::move(self));
        return;
      }
      case waiting_for_response: {
        break;
      }
      }
    }
    else if (error == http::error::end_of_stream || error == net::error::bad_descriptor)
    {
      // connect again
      state_ = connecting;
      async_connect(session_->stream_, session_->results_, std::move(self));
      return;
    }

    req_.reset();
    buffer_.reset();
    // set error in case of non-2xx response
    if (res_->result_int() < 200 || res_->result_int() >= 300)
    {
      self.complete(make_error_code(ws_errc::status_code), res_);
    }
    else
    {
      self.complete(error, res_);
    }
    session_->is_free_ = true;
  }
};

template <typename CompletionToken>
auto session::compose(const std::string_view &message, CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            const std::size_t bytes_transferred)>::return_type
{

  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &,
                                                          const std::size_t bytes_transferred)>(
      async_initiate_request(shared_from_this(), message), token, stream_, resolver_);
}

}  // namespace details

class ws : private boost::asio::noncopyable
{
  net::io_context &                            io_;
  uriparserpp::uri                             _server_url;
  std::string                                  _host;
  std::string                                  _port;
  std::list<std::shared_ptr<details::session>> sessions;

public:
  ws(net::io_context &io, const std::string &server_url)
    : io_(io)
    , _server_url(server_url)
  {}

  template <typename CompletionToken>
  auto async_write(const std::string_view message, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              const std::size_t bytes_transferred)>::return_type
  {
    auto                              session = std::find_if(sessions.begin(), sessions.end(),
                                [](auto &session) { return session->is_free(); });
    std::shared_ptr<details::session> sess;
    if (session == sessions.end())
    {
      sess = std::make_shared<details::session>(io_, std::ref(_host), std::ref(_port));

      sessions.push_back(sess);
    }
    else
    {
      sess = *session;
    }

    return sess->compose(message, token);
  }
};
}  // namespace asio
}  // namespace websocketer

#endif
