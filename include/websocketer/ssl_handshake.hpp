#ifndef WEBSOCKETER_SSL_HANDSHAKE_HPP
#define WEBSOCKETER_SSL_HANDSHAKE_HPP

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
struct async_intiate_ssl_handshake
{
  websocket::stream<Stream> &_stream;
  const std::string &        _host;

  template <typename Self>
  void operator()(Self &self)
  {
    // Set a timeout on the operation
    beast::get_lowest_layer(_stream).expires_after(std::chrono::seconds(30));

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(_stream.next_layer().native_handle(), _host.c_str()))
    {
      boost::system::error_code ec =
          beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
      self.complete(ec);
      return;
    }

    // Perform the SSL handshake
    _stream.next_layer().async_handshake(ssl::stream_base::client, self.move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error)
  {
    self.complete(error);
  }
};
}  // namespace details

template <typename Stream, typename CompletionToken>
auto async_ssl_handshake(websocket::stream<Stream> &stream, const std::string &host,
                         CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &)>(
      details::async_intiate_ssl_handshake<Stream>{stream, host}, token, stream);
}

}  // namespace asio
}  // namespace websocketer
#endif
