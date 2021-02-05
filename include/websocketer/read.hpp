#ifndef WEBSOCKETER_READ_HPP
#define WEBSOCKETER_READ_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>

namespace websocketer {
namespace asio {
namespace details {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
using tcp           = boost::asio::ip::tcp;

struct async_intiate_read
{
  std::shared_ptr<socket> _socket;
  beast::flat_buffer &    _buffer;

  template <typename Self>
  void operator()(Self &self)
  {
    _socket->_stream.async_read(_buffer, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error, std::size_t bytes_transferred)
  {
    self.complete(error, _socket, bytes_transferred);
  }
};

template <typename CompletionToken>
auto async_read(std::shared_ptr<socket> s, beast::flat_buffer &buffer, CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::shared_ptr<socket>, std::size_t)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &,
                                                          std::shared_ptr<socket>, std::size_t)>(
      async_intiate_read{s, buffer}, token, s->_stream);
}

}  // namespace details
}  // namespace asio
}  // namespace websocketer
#endif
