#ifndef WEBOSCKETER_SOCKET_HPP
#define WEBOSCKETER_SOCKET_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <memory>
#include <string>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
using tcp           = boost::asio::ip::tcp;

class socket : private boost::asio::noncopyable, public std::enable_shared_from_this<socket>
{
public:
  tcp::resolver                        _resolver;
  websocket::stream<beast::tcp_stream> _stream;
  const bool                           is_secure = false;

  socket(net::io_context &io)
    : _resolver(io)
    , _stream(io)
  {}

  template <typename CompletionToken>
  auto async_open(const std::string &host, const std::string &service, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::shared_ptr<socket>)>::return_type;

  template <typename CompletionToken>
  auto async_close(CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::shared_ptr<socket>)>::return_type;

  template <typename CompletionToken>
  auto async_read(beast::flat_buffer &buffer, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::shared_ptr<socket>, std::size_t)>::return_type;

  template <typename CompletionToken>
  auto async_write(const std::string &to_send, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::shared_ptr<socket>, std::size_t)>::return_type;
};

}  // namespace asio
}  // namespace websocketer

#endif
