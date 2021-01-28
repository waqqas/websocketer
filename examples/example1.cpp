
#include "websocketer/websocketer.h"

#include <boost/asio.hpp>

using namespace websocketer::asio;

int main(void)
{
  boost::asio::io_service io;
  websocketer::asio::ws   client(io, "wss://echo.websocket.org", "80");

  client.async_write("hello",
                     [&](const boost::system::error_code &ec, const std::size_t bytes_transferred) {
                       if (!ec)
                       {
                         std::cout << "bytes_transferred: " << bytes_transferred << std::endl;
                       }
                       else
                       {
                         std::cerr << "get error: " << ec.message() << std::endl;
                       }
                     });

  io.run();

  return 0;
}