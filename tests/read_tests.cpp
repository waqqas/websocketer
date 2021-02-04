#include "websocketer/websocketer.h"

#include <catch2/catch.hpp>
#include <iostream>
#include <string>

TEST_CASE("echo")
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
  beast::flat_buffer                   buffer;
  std::string                          data_to_send("hello world");

  ws::async_open(resolver, stream, host, service, [&](const boost::system::error_code &ec) {
    if (!ec)
    {
      ws::async_write(stream, data_to_send, [&](const boost::system::error_code &ec, std::size_t) {
        if (!ec)
        {
          ws::async_read(stream, buffer, [&](const boost::system::error_code &ec, std::size_t) {
            if (!ec)
            {
              std::cout << beast::make_printable(buffer.data()) << std::endl;
              passed = true;
            }
          });
        }
      });
    }
  });

  io.run();

  REQUIRE(passed);
}
