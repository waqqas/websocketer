#ifndef WEBSOCKETER_HANDSHAKE_HPP
#define WEBSOCKETER_HANDSHAKE_HPP

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
template <typename Stream>
struct async_intiate_handshake
{
  websocket::stream<Stream> &_stream;
  const std::string &        _host;

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

template <typename Stream, typename CompletionToken>
auto async_handshake(websocket::stream<Stream> &stream, const std::string &host,
                     CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &)>(
      details::async_intiate_handshake<Stream>{stream, host}, token, stream);
}

}  // namespace asio
}  // namespace websocketer
#endif
