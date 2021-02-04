#ifndef WEBOSCKETER_SESSION_H
#define WEBOSCKETER_SESSION_H

#include "websocketer/close.h"
#include "websocketer/isession.h"
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

class session : private boost::asio::noncopyable,
                public std::enable_shared_from_this<session>,
                public details::isession
{
public:
  session(net::io_context &io)
    : details::isession(io)
  {}

  template <typename CompletionToken>
  auto async_open(const std::string &host, const std::string &service, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &)>::return_type
  {
    return details::async_open(shared_from_this(), host, service, token);
  }

  template <typename CompletionToken>
  auto async_close(CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &)>::return_type
  {
    return details::async_close(shared_from_this(), token);
  }

  template <typename CompletionToken>
  auto async_read(beast::flat_buffer &buffer, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::size_t)>::return_type
  {
    return details::async_read(shared_from_this(), buffer, token);
  }

  template <typename CompletionToken>
  auto async_write(const std::string &to_send, CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::size_t)>::return_type
  {
    return details::async_write(shared_from_this(), to_send, token);
  }
};

}  // namespace asio
}  // namespace websocketer

#endif
