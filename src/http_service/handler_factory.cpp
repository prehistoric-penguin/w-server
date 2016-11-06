#include "handler_factory.h"
#include "default_handler.h"
#include "monitor_handler.h"
#include "version_handler.h"

namespace w {

using namespace proxygen;

RequestHandler *
HttpHandlerFactory::onRequest(RequestHandler *,
                              HTTPMessage *http_message) noexcept {
  auto path = http_message->getPath();
  LOG(INFO) << "path =" << path;
  if (path == "/monitor") {
    return new MonitorHandler();
  }

  if (path == "/version") {
    return new VersionHandler();
  }

  return new DefaultHandler();
}

} // namespace w
