#include "websocketer/websocketer.h"

#include <catch2/catch.hpp>
#include <string>

TEST_CASE("session")
{
  namespace beast = boost::beast;

  bool passed = false;

  std::string host("echo.websocket.org");
  std::string service("80");

  boost::asio::io_context    io;
  websocketer::asio::session session(io);
  beast::flat_buffer         buffer;
  std::string                data_to_send("hello world");

  session.async_open(host, service, [&](const boost::system::error_code &ec) {
    if (!ec)
    {
      session.async_write(data_to_send, [&](const boost::system::error_code &ec, std::size_t) {
        if (!ec)
        {
          session.async_read(buffer, [&](const boost::system::error_code &ec, std::size_t) {
            if (!ec)
            {
              session.async_close([&](const boost::system::error_code &ec) {
                if (!ec)
                {
                  passed = true;
                }
              });
            }
          });
        }
      });
    }
  });

  io.run();

  REQUIRE(passed);
}
