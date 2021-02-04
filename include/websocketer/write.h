#ifndef WEBSOCKETER_WRITE_H
#define WEBSOCKETER_WRITE_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
using tcp           = boost::asio::ip::tcp;

namespace details {
struct async_intiate_write
{
  websocket::stream<beast::tcp_stream> &_stream;
  const std::string &                   _to_send;

  template <typename Self>
  void operator()(Self &self)
  {
    _stream.async_write(net::buffer(_to_send), std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error, std::size_t bytes_transferred)
  {
    self.complete(error, bytes_transferred);
  }
};

}  // namespace details

template <typename CompletionToken>
auto async_write(websocket::stream<beast::tcp_stream> &stream, const std::string &to_send,
                 CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::size_t)>::return_type
{
  return boost::asio::async_compose<CompletionToken,
                                    void(const boost::system::error_code &, std::size_t)>(
      details::async_intiate_write{stream, to_send}, token, stream);
}

}  // namespace asio
}  // namespace websocketer
#endif
