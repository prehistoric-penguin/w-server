#pragma once

#include <folly/Memory.h>
#include <proxygen/httpserver/RequestHandler.h>

namespace w {
class DefaultHandler : public proxygen::RequestHandler {
public:
  virtual void
  onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;

  virtual void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

  virtual void onEOM() noexcept override;

  virtual void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override {}

  virtual void requestComplete() noexcept override { delete this; }

  virtual void onError(proxygen::ProxygenError err) noexcept override {
    delete this;
  }

protected:
  std::unique_ptr<proxygen::HTTPMessage> req_headers_;
  std::unique_ptr<folly::IOBuf> req_body_;
};

} // namespace w
