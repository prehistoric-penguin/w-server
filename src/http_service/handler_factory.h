#pragma once

#include <boost/noncopyable.hpp>

#include <proxygen/httpserver/RequestHandlerFactory.h>

namespace w {
using namespace proxygen;

class HttpHandlerFactory : public RequestHandlerFactory,
                           private boost::noncopyable {
public:
  HttpHandlerFactory() = default;
  ~HttpHandlerFactory() = default;

  void onServerStart() noexcept override {}

  void onServerStop() noexcept override {}

  RequestHandler *onRequest(RequestHandler *,
                            HTTPMessage *http_message) noexcept override;

private:
};
} // namespace w
