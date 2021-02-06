#include "websocketer/websocketer.hpp"

#include <benchmark/benchmark.h>
#include <boost/asio.hpp>

static void BM_Websocket(benchmark::State &state)
{
  boost::asio::io_context io;
  websocketer::asio::ws   client(io, "echo.websocket.org", "http");
  // Perform setup here
  for (auto _ : state)
  {
  }
}
// Register the function as a benchmark
BENCHMARK(BM_Websocket);