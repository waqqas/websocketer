#ifndef WEBOSCKETER_SSOCKET_HPP
#define WEBOSCKETER_SSOCKET_HPP

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

class ssocket : private boost::asio::noncopyable, public std::enable_shared_from_this<ssocket>
{
public:
  tcp::resolver                        _resolver;
  websocket::stream<beast::tcp_stream> _stream;

  ssocket(net::io_context &io)
    : _resolver(io)
    , _stream(io)
  {}

  template <typename CompletionToken>
  auto async_open(const std::string &host, const std::string &service, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::shared_ptr<ssocket>)>::return_type;

  template <typename CompletionToken>
  auto async_close(CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::shared_ptr<ssocket>)>::return_type;

  template <typename CompletionToken>
  auto async_read(beast::flat_buffer &buffer, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::shared_ptr<ssocket>, std::size_t)>::return_type;

  template <typename CompletionToken>
  auto async_write(const std::string &to_send, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::shared_ptr<ssocket>, std::size_t)>::return_type;
};

}  // namespace asio
}  // namespace websocketer

#endif
