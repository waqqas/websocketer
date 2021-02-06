#include "websocketer/websocketer.hpp"

#include <benchmark/benchmark.h>
#include <boost/asio.hpp>

static void BM_Echo(benchmark::State &state)
{
  namespace beast = boost::beast;
  namespace ws    = websocketer::asio;

  boost::asio::io_context io;
  ws::ws                  client(io, "echo.websocket.org", "80");

  client.async_open([&](const boost::system::error_code &ec, std::shared_ptr<ws::socket> socket) {
    if (!ec)
    {
      std::string        data_to_send("hello world");
      beast::flat_buffer buffer;
      for (auto _ : state)
      {
        socket->async_write(data_to_send, [&](const boost::system::error_code &,
                                              std::shared_ptr<ws::socket>, std::size_t) {
          if (!ec)
          {
            socket->async_read(buffer, [&](const boost::system::error_code &,
                                           std::shared_ptr<ws::socket>, std::size_t) {});
          }
        });
      }
      socket->async_close([&](const boost::system::error_code &, std::shared_ptr<ws::socket>) {});
    }
  });
  io.run();
}
// Register the function as a benchmark
BENCHMARK(BM_Echo);