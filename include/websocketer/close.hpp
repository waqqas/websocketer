#ifndef WEBSOCKETER_CLOSE_HPP
#define WEBSOCKETER_CLOSE_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>

namespace websocketer {
namespace asio {
namespace details {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
using tcp           = boost::asio::ip::tcp;

struct async_intiate_close
{
  std::shared_ptr<socket> _socket;

  template <typename Self>
  void operator()(Self &self)
  {
    _socket->_stream.async_close(websocket::close_code::normal, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error)
  {
    self.complete(error, _socket);
  }
};

template <typename CompletionToken>
auto async_close(std::shared_ptr<socket> s, CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::shared_ptr<socket>)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &,
                                                          std::shared_ptr<socket>)>(
      async_intiate_close{s}, token, s->_stream);
}

}  // namespace details
}  // namespace asio
}  // namespace websocketer
#endif
