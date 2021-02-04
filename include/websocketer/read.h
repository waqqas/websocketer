#ifndef WEBSOCKETER_READ_H
#define WEBSOCKETER_READ_H

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
struct async_intiate_read
{
  std::shared_ptr<session> _session;
  beast::flat_buffer &     _buffer;

  template <typename Self>
  void operator()(Self &self)
  {
    _session->_stream.async_read(_buffer, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error, std::size_t bytes_transferred)
  {
    self.complete(error, bytes_transferred);
  }
};

}  // namespace details

template <typename CompletionToken>
auto async_read(std::shared_ptr<session> session, beast::flat_buffer &buffer,
                CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            std::size_t)>::return_type
{
  return boost::asio::async_compose<CompletionToken,
                                    void(const boost::system::error_code &, std::size_t)>(
      details::async_intiate_read{session, buffer}, token, session->_stream);
}

}  // namespace asio
}  // namespace websocketer
#endif
