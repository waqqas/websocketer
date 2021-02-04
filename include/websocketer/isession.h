#ifndef WEBOSCKETER_ISESSION_H
#define WEBOSCKETER_ISESSION_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
using tcp           = boost::asio::ip::tcp;

namespace details {
class isession
{
public:
  tcp::resolver                        _resolver;
  websocket::stream<beast::tcp_stream> _stream;

public:
  isession(net::io_context &io)
    : _resolver(io)
    , _stream(io)
  {}
};

}  // namespace details
}  // namespace asio
}  // namespace websocketer

#endif
