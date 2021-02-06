#ifndef WEBSOCKETER_OPEN_HPP
#define WEBSOCKETER_OPEN_HPP

#include "websocketer/connect.hpp"
#include "websocketer/handshake.hpp"
#include "websocketer/resolve.hpp"
#include "websocketer/ssl_handshake.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace websocketer {
namespace asio {
namespace details {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace ws        = websocketer::asio;
using tcp           = boost::asio::ip::tcp;

template <typename Socket>
struct async_initiate_open
{
  std::shared_ptr<Socket> _socket;
  std::string             _host;
  const std::string &     _service;

  enum
  {
    resolving,
    connecting,
    ssl_handshaking,
    handshaking,
  } _state = handshaking;

  template <typename Self>
  void operator()(Self &self)
  {
    _state = resolving;
    ws::async_resolve(_socket->_resolver, _host, _service, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error,
                  tcp::resolver::results_type results)
  {
    BOOST_ASSERT(_state == resolving);
    if (!error)
    {

      _state = connecting;
      ws::async_connect(_socket->_stream, results, std::move(self));
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
      _host += ':' + std::to_string(ep.port());
      if (_socket->is_secure)
      {
        _state = ssl_handshaking;
        ws::async_ssl_handshake(_socket->_stream, _host, std::move(self));
      }
      else
      {
        _state = handshaking;
        ws::async_handshake(_socket->_stream, _host, std::move(self));
      }
      return;
    }
    self.complete(error, _socket);
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error)
  {
    if (_state == ssl_handshaking)
    {
      _state = handshaking;
      // _host already contains port
      ws::async_handshake(_socket->_stream, _host, std::move(self));
      return;
    }
    BOOST_ASSERT(_state == handshaking);
    self.complete(error, _socket);
  };
};

template <typename Socket, typename CompletionToken>
auto async_open(std::shared_ptr<Socket> s, const std::string &host, const std::string &service,
                CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::shared_ptr<Socket>)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &,
                                                          std::shared_ptr<Socket>)>(
      async_initiate_open<Socket>{s, host, service}, token, s->_resolver, s->_stream);
}

}  // namespace details
}  // namespace asio
}  // namespace websocketer
#endif