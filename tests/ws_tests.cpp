#include "websocketer/ws.h"

#include <catch2/catch.hpp>
#include <string>

TEST_CASE("ws")
{
  bool passed = false;

  std::string host("echo.websocket.org");
  std::string service("80");

  boost::asio::io_context    io;
  websocketer::asio::ws ws(io, host, service);

  ws.async_open( [&](const boost::system::error_code &ec) {
    if (!ec)
    {
      passed = true;
    }
  });

  io.run();

  REQUIRE(passed);
}
