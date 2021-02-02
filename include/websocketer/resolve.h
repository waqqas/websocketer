#ifndef WEBSOCKETER_RESOVLE_H
#define WEBSOCKETER_RESOVLE_H

#include <boost/asio.hpp>
#include <string>

namespace websocketer {
namespace asio {
using tcp = boost::asio::ip::tcp;

namespace details {

struct async_intiate_resolve
{
  tcp::resolver &resolver;
  std::string &  host;
  std::string &  service;

  template <typename Self>
  void operator()(Self &self)
  {
    resolver.async_resolve(host, service, std::move(self));
  }

  template <typename Self>
  void operator()(Self &self, const boost::system::error_code &error,
                  const tcp::resolver::results_type &results)
  {
    self.complete(error, results);
  }
};

}  // namespace details

template <typename CompletionToken>
auto async_resolve(tcp::resolver &resolver, std::string &host, std::string &service,
                   CompletionToken &&token) ->
    typename boost::asio::async_result<typename std::decay<CompletionToken>::type,
                                       void(const boost::system::error_code &,
                                            const tcp::resolver::results_type &)>::return_type
{
  return boost::asio::async_compose<CompletionToken, void(const boost::system::error_code &,
                                                          const tcp::resolver::results_type &)>(
      details::async_intiate_resolve{resolver, host, service}, token, resolver);
}

}  // namespace asio
}  // namespace websocketer
#endif
