#ifndef WEBOSCKETER_SSOCKET_HPP
#define WEBOSCKETER_SSOCKET_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/certify/extensions.hpp>
#include <boost/certify/https_verification.hpp>
#include <memory>
#include <string>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
namespace ssl       = boost::asio::ssl;
using tcp           = boost::asio::ip::tcp;

class ssocket : private boost::asio::noncopyable, public std::enable_shared_from_this<ssocket>
{
public:
  using stream_type = beast::ssl_stream<beast::tcp_stream>;

  ssl::context                   _ctx;
  tcp::resolver                  _resolver;
  websocket::stream<stream_type> _stream;
  // constexpr bool is_secure = true;

  ssocket(net::io_context &io)
    : _ctx{ssl::context::tlsv12_client}
    , _resolver(io)
    , _stream(io, _ctx)
  {
    // Verify the remote server's certificate
    _ctx.set_verify_mode(ssl::verify_peer);
    _ctx.set_default_verify_paths();
  }

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
