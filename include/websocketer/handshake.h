#ifndef WEBSOCKETER_HANDSHAKE_H
#define WEBSOCKETER_HANDSHAKE_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace http      = beast::http;
using tcp           = boost::asio::ip::tcp;

namespace details {
struct async_intiate_handshake
{
  websocket::stream<beast::tcp_stream> &_stream;
  const std::string &                   _host;

  template <typename Self>
  void operator()(Self &self)
  {
    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(_stream).expires_never();

    // Set suggested timeout settings for the websocket
    _stream.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    _stream.set_option(websocket::stream_base::decorator([](websocket::request_type &req) {
      req.set(http::field::user_agent,
              std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-async");
    }));

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    // std::string host = _host + ':' + std::to_string(ep.port());

    // Perform the websocket handshake
    _stream.async_handshake(_host, "/", std::move(self));
  }
  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error)
  {
    self.complete(error);
  }
};
}  // namespace details

template <typename CompletionToken>
auto async_handshake(websocket::stream<beast::tcp_stream> &stream, const std::string &host,
                     CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &)>(
      details::async_intiate_handshake{stream, host}, token, stream);
}

}  // namespace asio
}  // namespace websocketer
#endif
