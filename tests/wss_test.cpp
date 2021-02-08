#include "websocketer/websocketer.hpp"

#include <catch2/catch.hpp>
#include <memory>
#include <string>

TEST_CASE("wss")
{
  namespace beast = boost::beast;
  namespace ws    = websocketer::asio;

  bool passed = false;

  std::string host("echo.websocket.org");
  std::string service("443");

  boost::asio::io_context io;
  ws::wss                 client(io, host, service);
  beast::flat_buffer      buffer;
  std::string             data_to_send("hello world");

  client.async_open([&data_to_send, &buffer, &passed](const boost::system::error_code &ec,
                                                      std::shared_ptr<ws::ssocket>     socket) {
    if (!ec)
    {
      socket->async_write(data_to_send, [&buffer, &passed](const boost::system::error_code &ec,
                                                           std::shared_ptr<ws::ssocket>     socket,
                                                           std::size_t) {
        if (!ec)
        {
          socket->async_read(buffer, [&passed](const boost::system::error_code &ec,
                                               std::shared_ptr<ws::ssocket> socket, std::size_t) {
            if (!ec)
            {
              socket->async_close(
                  [&](const boost::system::error_code &ec, std::shared_ptr<ws::ssocket>) {
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
