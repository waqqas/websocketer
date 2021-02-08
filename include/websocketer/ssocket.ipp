#ifndef WEBSOCKETER_SSOCKET_IPP
#define WEBSOCKETER_SSOCKET_IPP

#include "websocketer/ssocket.hpp"

namespace websocketer {
namespace asio {

template <typename CompletionToken>
auto ssocket::async_open(const std::string &host, const std::string &service,
                        CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::shared_ptr<ssocket>)>::return_type
{
  return details::async_open(shared_from_this(), host, service, token);
}

template <typename CompletionToken>
auto ssocket::async_close(CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::shared_ptr<ssocket>)>::return_type
{
  return details::async_close(shared_from_this(), token);
}

template <typename CompletionToken>
auto ssocket::async_read(beast::flat_buffer &buffer, CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::shared_ptr<ssocket>, std::size_t)>::return_type
{
  return details::async_read(shared_from_this(), buffer, token);
}

template <typename CompletionToken>
auto ssocket::async_write(const std::string &to_send, CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::shared_ptr<ssocket>, std::size_t)>::return_type
{
  return details::async_write(shared_from_this(), to_send, token);
}

}  // namespace asio
}  // namespace websocketer

#endif
