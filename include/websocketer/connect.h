#ifndef WEBSOCKETER_CONNECT_H
#define WEBSOCKETER_CONNECT_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <string>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
using tcp           = boost::asio::ip::tcp;

namespace details {
struct async_intiate_connect
{
  websocket::stream<beast::tcp_stream> &stream_;
  const tcp::resolver::results_type &   results_;

  template <typename Self>
  void operator()(Self &self)
  {
    beast::get_lowest_layer(stream_).async_connect(results_, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error,
                  const tcp::resolver::results_type::endpoint_type & ep)
  {
    self.complete(error, ep);
  }
};

}  // namespace details

template <typename CompletionToken>
auto async_connect(websocket::stream<beast::tcp_stream> &stream,
                   const tcp::resolver::results_type &results, CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &, const tcp::resolver::results_type::endpoint_type &)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &, const tcp::resolver::results_type::endpoint_type &)>(
      details::async_intiate_connect{stream, results}, token, stream);
}

}  // namespace asio
}  // namespace websocketer
#endif