#pragma once

#include "default_handler.h"

namespace w {

class VersionHandler : public DefaultHandler {
public:
  virtual void onEOM() noexcept override;
};

} // namespace w
