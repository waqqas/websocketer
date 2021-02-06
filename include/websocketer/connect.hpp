#ifndef WEBSOCKETER_CONNECT_HPP
#define WEBSOCKETER_CONNECT_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace websocketer {
namespace asio {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
using tcp           = boost::asio::ip::tcp;

namespace details {
template <typename Stream>
struct async_intiate_connect
{
  websocket::stream<Stream> &        _stream;
  const tcp::resolver::results_type &results_;

  template <typename Self>
  void operator()(Self &self)
  {
    beast::get_lowest_layer(_stream).async_connect(results_, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error,
                  const tcp::resolver::results_type::endpoint_type &ep)
  {
    self.complete(error, ep);
  }
};

}  // namespace details

template <typename Stream, typename CompletionToken>
auto async_connect(websocket::stream<Stream> &stream, const tcp::resolver::results_type &results,
                   CompletionToken &&token) ->
    typename boost::asio::async_result<
        typename std::decay<CompletionToken>::type,
        void(const boost::system::error_code &,
             const tcp::resolver::results_type::endpoint_type &)>::return_type
{
  return boost::asio::async_compose<CompletionToken,
                                    void(const boost::system::error_code &,
                                         const tcp::resolver::results_type::endpoint_type &)>(
      details::async_intiate_connect<Stream>{stream, results}, token, stream);
}

}  // namespace asio
}  // namespace websocketer
#endif
