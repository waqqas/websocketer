#ifndef WEBOSCKETER_SESSION_H
#define WEBOSCKETER_SESSION_H

#include "websocketer/close.h"
#include "websocketer/open.h"
#include "websocketer/read.h"
#include "websocketer/write.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
using tcp           = boost::asio::ip::tcp;

class session : private boost::asio::noncopyable
{
private:
  tcp::resolver                        _resolver;
  websocket::stream<beast::tcp_stream> _stream;

public:
  session(net::io_context &io)
    : _resolver(io)
    , _stream(io)
  {}

  template <typename CompletionToken>
  auto async_open(const std::string &host, const std::string &service, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &)>::return_type
  {
    return websocketer::asio::async_open(_resolver, _stream, host, service, token);
  }

  template <typename CompletionToken>
  auto async_close(CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &)>::return_type
  {
    return websocketer::asio::async_close(_stream, token);
  }

  template <typename CompletionToken>
  auto async_read(beast::flat_buffer &buffer, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::size_t)>::return_type
  {
    return websocketer::asio::async_read(_stream, buffer, token);
  }

  template <typename CompletionToken>
  auto async_write(const std::string &to_send, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::size_t)>::return_type
  {
    return websocketer::asio::async_write(_stream, to_send, token);
  }
};

}  // namespace asio
}  // namespace websocketer

#endif
