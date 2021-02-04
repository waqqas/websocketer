#ifndef WEBSOCKETER_CLOSE_H
#define WEBSOCKETER_CLOSE_H

#include "websocketer/session.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
using tcp           = boost::asio::ip::tcp;

namespace details {
struct async_intiate_close
{
  std::shared_ptr<session> _session;

  template <typename Self>
  void operator()(Self &self)
  {
    _stream.async_close(websocket::close_code::normal, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error)
  {
    self.complete(error);
  }
};

}  // namespace details

template <typename CompletionToken>
auto async_close(std::shared_ptr<session> session, CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &)>(
      details::async_intiate_close{session}, token, session->_stream);
}

}  // namespace asio
}  // namespace websocketer
#endif
