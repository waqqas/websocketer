#include "websocketer/websocketer.h"

#include <catch2/catch.hpp>
#include <string>

TEST_CASE("open1")
{
  namespace beast     = boost::beast;
  namespace websocket = beast::websocket;
  namespace http      = beast::http;
  namespace ws        = websocketer::asio;
  using tcp           = boost::asio::ip::tcp;

  bool passed = false;

  boost::asio::io_context io;

  tcp::resolver                        resolver(io);
  std::string                          host("echo.websocket.org");
  std::string                          service("80");
  websocket::stream<beast::tcp_stream> stream(io);

  ws::async_open(resolver, stream, host, service, [&](const boost::system::error_code &ec) {
    if (!ec)
    {
      passed = true;
    }
  });

  io.run();

  REQUIRE(passed);
}
