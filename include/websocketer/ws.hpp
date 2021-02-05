#ifndef WEBSOCKETER_WS_HPP
#define WEBSOCKETER_WS_HPP

#include "socket.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <list>
#include <memory>
#include <string>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace http      = beast::http;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
using tcp           = boost::asio::ip::tcp;

class ws : private boost::asio::noncopyable
{
  net::io_context &  _io;
  const std::string &_host;
  const std::string &_service;

public:
  ws(net::io_context &io, const std::string &host, const std::string &service)
    : _io(io)
    , _host(host)
    , _service(service)
  {}

  template <typename CompletionToken>
  auto async_open(CompletionToken &&token) ->
      typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                         void(const boost::system::error_code &,
                                              std::shared_ptr<socket>)>::return_type
  {
    auto sess = std::make_shared<socket>(_io);

    return sess->async_open(_host, _service, token);
  }
};
}  // namespace asio
}  // namespace websocketer
#endif
