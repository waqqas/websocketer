#ifndef WEBSOCKETER_OPEN_HPP
#define WEBSOCKETER_OPEN_HPP

#include "websocketer/connect.hpp"
#include "websocketer/handshake.hpp"
#include "websocketer/resolve.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace websocketer {
namespace asio {
namespace details {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
using tcp           = boost::asio::ip::tcp;

struct async_initiate_open
{
  std::shared_ptr<socket> _socket;
  const std::string &     _host;
  const std::string &     _service;

  enum
  {
    resolving,
    connecting,
    handshaking,
  } _state = handshaking;

  template <typename Self>
  void operator()(Self &self)
  {
    _state = resolving;
    websocketer::asio::async_resolve(_socket->_resolver, _host, _service, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error,
                  tcp::resolver::results_type results)
  {
    BOOST_ASSERT(_state == resolving);
    if (!error)
    {

      _state = connecting;
      websocketer::asio::async_connect(_socket->_stream, results, std::move(self));
      return;
    }
    self.complete(error, _socket);
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error,
                  const tcp::resolver::results_type::endpoint_type &ep)
  {
    BOOST_ASSERT(_state == connecting);
    if (!error)
    {

      _state = handshaking;
      websocketer::asio::async_handshake(_socket->_stream, _host, ep, std::move(self));
      return;
    }
    self.complete(error, _socket);
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error)
  {
    BOOST_ASSERT(_state == handshaking);
    self.complete(error, _socket);
  };
};

template <typename CompletionToken>
auto async_open(std::shared_ptr<socket> s, const std::string &host, const std::string &service,
                CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::shared_ptr<socket>)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &,
                                                          std::shared_ptr<socket>)>(
      async_initiate_open{s, host, service}, token, s->_resolver, s->_stream);
}

}  // namespace details
}  // namespace asio
}  // namespace websocketer
#endif