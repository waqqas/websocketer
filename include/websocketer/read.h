#ifndef WEBSOCKETER_READ_H
#define WEBSOCKETER_READ_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
using tcp           = boost::asio::ip::tcp;

namespace details {
struct async_intiate_read
{
  websocket::stream<beast::tcp_stream> &_stream;
  beast::flat_buffer &                  _buffer;

  template <typename Self>
  void operator()(Self &self)
  {
    _stream.async_read(_buffer, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error, std::size_t bytes_transferred)
  {
    self.complete(error, bytes_transferred);
  }
};

}  // namespace details

template <typename CompletionToken>
auto async_read(websocket::stream<beast::tcp_stream> &stream, beast::flat_buffer &buffer,
                CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::size_t)>::return_type
{
  return boost::asio::async_compose<CompletionToken,
                                    void(const boost::system::error_code &, std::size_t)>(
      details::async_intiate_read{stream, buffer}, token, stream);
}

}  // namespace asio
}  // namespace websocketer
#endif
