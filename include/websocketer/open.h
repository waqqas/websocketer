#ifndef WEBSOCKETER_OPEN_H
#define WEBSOCKETER_OPEN_H

#include "websocketer/connect.h"
#include "websocketer/handshake.h"
#include "websocketer/resolve.h"

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

  tcp::resolver &                       _resolver;
  websocket::stream<beast::tcp_stream> &_stream;
  const std::string &                   _host;
  const std::string &                   _service;

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
    websocketer::asio::async_resolve(_resolver, _host, _service, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error,
                  tcp::resolver::results_type results)
  {
    BOOST_ASSERT(_state == resolving);
    if (!error)
    {

      _state = connecting;
      websocketer::asio::async_connect(_stream, results, std::move(self));
      return;
    }
    self.complete(error);
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error,
                  const tcp::resolver::results_type::endpoint_type &)
  {
    BOOST_ASSERT(_state == connecting);
    if (!error)
    {

      _state = handshaking;
      websocketer::asio::async_handshake(_stream, _host, std::move(self));
      return;
    }
    self.complete(error);
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error)
  {
    BOOST_ASSERT(_state == handshaking);
    self.complete(error);
  };
};
}  // namespace details

template <typename CompletionToken>
auto async_open(tcp::resolver &resolver, websocket::stream<beast::tcp_stream> &stream,
                const std::string &host, const std::string &service, CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &)>(
      details::async_initiate_open{resolver, stream, host, service}, token, resolver, stream);
}

}  // namespace asio
}  // namespace websocketer
#endif