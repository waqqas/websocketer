#ifndef WEBSOCKETER_WRITE_H
#define WEBSOCKETER_WRITE_H

#include "websocketer/isession.h"

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

struct async_intiate_write
{
  std::shared_ptr<isession> _session;
  const std::string &       _to_send;

  template <typename Self>
  void operator()(Self &self)
  {
    _session->_stream.async_write(net::buffer(_to_send), std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error, std::size_t bytes_transferred)
  {
    self.complete(error, bytes_transferred);
  }
};

template <typename CompletionToken>
auto async_write(std::shared_ptr<isession> session, const std::string &to_send,
                 CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::size_t)>::return_type
{
  return boost::asio::async_compose<CompletionToken,
                                    void(const boost::system::error_code &, std::size_t)>(
      async_intiate_write{session, to_send}, token, session->_stream);
}

}  // namespace details
}  // namespace asio
}  // namespace websocketer
#endif
