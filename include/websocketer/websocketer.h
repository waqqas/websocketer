#ifndef WEBSOCKETER_H
#define WEBSOCKETER_H

#include <boost/asio.hpp>
#include <boost/core/ignore_unused.hpp>

namespace websocketer {
namespace asio {
using namespace boost::asio;
class client
{
public:
  client(io_context &io)
    : _io(io)
  {
    boost::ignore_unused(_io);
  }

private:
  io_context &_io;
};
}  // namespace asio
}  // namespace websocketer

#endif
