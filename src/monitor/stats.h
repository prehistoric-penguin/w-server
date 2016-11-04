#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <boost/noncopyable.hpp>

namespace w {

class MonitorImpl;

class Monitor : private boost::noncopyable {
public:
  Monitor();
  ~Monitor() = default;
  std::string DoReport();
  void Add(const std::string &key, int value);

  static std::string Report();
private:
  std::unique_ptr<MonitorImpl> impl_;
};

class Marker {
public:
  Marker(const std::string &key)
      : key_(key), time_point_(std::chrono::steady_clock::now()) {}

  ~Marker();

private:
  std::string key_;
  std::chrono::steady_clock::time_point time_point_;
};

} // namespace w
