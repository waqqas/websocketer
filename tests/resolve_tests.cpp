#include "websocketer/websocketer.h"

#include <catch2/catch.hpp>
#include <iostream>
#include <string>

TEST_CASE("resolve google")
{
  namespace ws = websocketer::asio;
  using tcp    = boost::asio::ip::tcp;

  bool passed = false;

  boost::asio::io_context io;
  tcp::resolver           resolver(io);
  std::string             host("www.google.com");
  std::string             service("80");

  ws::async_resolve(resolver, host, service,
                    [&](const boost::system::error_code &ec, tcp::resolver::results_type results) {
                      if (!ec)
                      {
                        for (auto &result : results)
                        {
                          std::cout << "result: " << result.endpoint() << std::endl;
                        }
                        passed = true;
                      }
                      else
                      {
                        passed = false;
                      }
                    });
  io.run();

  REQUIRE(passed);
}