#ifndef WEBSOCKETER_WRITE_HPP
#define WEBSOCKETER_WRITE_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>

namespace websocketer {
namespace asio {
namespace details {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
using tcp           = boost::asio::ip::tcp;

template <typename SOCKET>
struct async_intiate_write
{
  std::shared_ptr<SOCKET> _socket;
  const std::string &     _to_send;

  template <typename Self>
  void operator()(Self &self)
  {
    _socket->_stream.async_write(net::buffer(_to_send), std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error, std::size_t bytes_transferred)
  {
    self.complete(error, _socket, bytes_transferred);
  }
};

template <typename Socket,typename CompletionToken>
auto async_write(std::shared_ptr<Socket> s, const std::string &to_send, CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::shared_ptr<Socket>, std::size_t)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &,
                                                          std::shared_ptr<Socket>, std::size_t)>(
      async_intiate_write<Socket>{s, to_send}, token, s->_stream);
}

}  // namespace details
}  // namespace asio
}  // namespace websocketer
#endif
