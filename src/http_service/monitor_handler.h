#pragma once

#include "default_handler.h"

#include <folly/FBString.h>

namespace w {

class MonitorHandler : public DefaultHandler {
public:
  virtual void onEOM() noexcept override;

private:
  folly::fbstring stats() noexcept;
};

} // namespace w
