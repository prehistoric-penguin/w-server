#include "server.h"

#include <folly/ThreadName.h>

#include "handler_factory.h"

namespace w {
namespace {
constexpr const char *kThreadsName = "HTTPServer";
}

void HttpServer::AsyncStart() {
  using namespace proxygen;
  using Protocol = HTTPServer::Protocol;

  HTTPServerOptions options;

  options.threads = threads_;
  options.idleTimeout = std::chrono::milliseconds(idletimeout_);
  options.shutdownOn = {SIGINT, SIGTERM};
  options.enableContentCompression = true;
  options.handlerFactories =
      RequestHandlerChain()
          .addThen(folly::make_unique<HttpHandlerFactory>())
          .build();

  http_server_ = folly::make_unique<HTTPServer>(std::move(options));

  std::vector<HTTPServer::IPConfig> IPs = {
      {folly::SocketAddress("0.0.0.0", port_, true), Protocol::HTTP},
  };

  http_server_->bind(IPs);

  std::thread thr([&]() {
    folly::setThreadName(kThreadsName);
    http_server_->start();
  });

  LOG(INFO) << "Async start HTTP service on port:" << port_
            << ",with worker threads num:" << threads_
            << ",idle timeout ms:" << idletimeout_;

  thr.detach();
}
} // namespace w
