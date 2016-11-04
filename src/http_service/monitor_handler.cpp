#include "monitor_handler.h"

#include <proxygen/httpserver/ResponseBuilder.h>

#include "monitor/stats.h"

namespace w {

void MonitorHandler::onEOM() noexcept {
  auto cmd = req_headers_->getDecodedQueryParam("command");
  LOG(INFO) << "request cmd:" << cmd;

  folly::fbstring resp;
  if (cmd == "stats") {
    resp = stats();
  } else {
    resp = "invalid request command";
  }

  proxygen::ResponseBuilder(downstream_)
      .status(200, "OK")
      .body(std::move(resp))
      .sendWithEOM();
}

folly::fbstring MonitorHandler::stats() noexcept {
  return Monitor::Report();
}

} // namespace w
