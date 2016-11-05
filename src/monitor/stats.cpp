#include "stats.h"
#include "stats-defs.h"

namespace w {

Monitor::Monitor() : impl_(folly::make_unique<MonitorImpl>()) {}

std::string Monitor::DoReport() { return impl_->Report(); }

std::string Monitor::Report() {
  return folly::Singleton<Monitor>::try_get()->DoReport();
}

void Monitor::Add(const std::string &key, int value) {
  impl_->AsyncAdd(key, value);
}

folly::Singleton<Monitor> gmonitor;

Marker::~Marker() {
  auto interval = std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::steady_clock::now() - time_point_).count();
  folly::Singleton<Monitor>::try_get()->Add(key_, interval);
}

} // namespace w
