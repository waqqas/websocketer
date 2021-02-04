#include "websocketer/websocketer.h"

#include <catch2/catch.hpp>
#include <string>

TEST_CASE("handshake1")
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

  ws::async_resolve(resolver, host, service,
                    [&](const boost::system::error_code &ec, tcp::resolver::results_type results) {
                      if (!ec)
                      {
                        ws::async_connect(stream, results,
                                          [&](const boost::system::error_code &ec,
                                              const tcp::resolver::results_type::endpoint_type &) {
                                            if (!ec)
                                            {
                                              ws::async_handshake(
                                                  stream, host,
                                                  [&](const boost::system::error_code &ec) {
                                                    if (!ec)
                                                    {
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
