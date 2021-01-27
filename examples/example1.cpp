
#include "websocketer/websocketer.h"

#include <boost/asio.hpp>

using namespace websocketer::asio;

int main(void)
{
  boost::asio::io_service   io;
  websocketer::asio::client client(io);

  io.run();

  return 0;
}