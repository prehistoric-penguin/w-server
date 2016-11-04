#include "default_handler.h"

#include <proxygen/httpserver/ResponseBuilder.h>

namespace w {
void DefaultHandler::onRequest(
    std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {
  req_headers_ = std::move(headers);
}

void DefaultHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
  if (req_body_) {
    req_body_->prependChain(std::move(body));
  } else {
    req_body_ = std::move(body);
  }
}

void DefaultHandler::onEOM() noexcept {
  proxygen::ResponseBuilder(downstream_)
      .status(200, "OK")
      .body("Hello , W server on service!!!\n")
      .sendWithEOM();
}
} // namespace w
