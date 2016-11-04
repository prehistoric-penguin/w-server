#include "stats.h"

#include <thread>

#include <folly/dynamic.h>
#include <folly/io/async/EventBase.h>
#include <folly/json.h>
#include <folly/Memory.h>
#include <folly/Singleton.h>
#include <folly/stats/TimeseriesHistogram.h>
#include <folly/stats/TimeseriesHistogram-defs.h>

namespace w {

namespace {
enum Levels { HALF_MINUTE, NUM_LEVELS };

const std::chrono::seconds kDurations[] = {std::chrono::seconds(30)};
} // namespace anonymous

class MonitorImpl {
public:
  using data_t =
      std::unordered_map<std::string, folly::TimeseriesHistogram<int>>;

  MonitorImpl();
  std::string Report();
  void Add(const std::string &key, int value);

private:
  data_t data_;
};

MonitorImpl::MonitorImpl() {}

std::string MonitorImpl::Report() {
  folly::dynamic mark = folly::dynamic::object();
  auto now = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::steady_clock::now().time_since_epoch());
  auto level = Levels::HALF_MINUTE;
  auto duration = kDurations[level].count();

  for (auto &kv : data_) {
    auto &key = kv.first;
    auto &histogram = kv.second;
    histogram.update(now);

    auto count = histogram.count(level);
    auto qps = count / duration;
    auto min = histogram.getPercentileEstimate(0, level);
    auto max = histogram.getPercentileEstimate(100, level);
    auto avg = long(histogram.avg(level));
    auto dist_99 = histogram.getPercentileEstimate(99, level);
    auto dist_80 = histogram.getPercentileEstimate(80, level);

    folly::dynamic inner_key_mark =
        folly::dynamic::object("accu", count)
                              ("qps", qps)
                              ("min", min)
                              ("max", max)
                              ("avg", avg)
                              ("dist99", dist_99)
                              ("dist80", dist_80);
    mark.insert(key, std::move(inner_key_mark));
  }

  return folly::toJson(mark).toStdString();
}

void MonitorImpl::Add(const std::string &key, int value) {
  auto now = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::steady_clock::now().time_since_epoch());

  auto it = data_.find(key);
  if (it == data_.end()) {
    auto kv = data_.emplace(key, folly::TimeseriesHistogram<int>(
                                     10, 0, 10 * 1024 * 1024,
                                     folly::MultiLevelTimeSeries<int>(
                                         10, Levels::NUM_LEVELS, kDurations)));
    it = kv.first;
  }
  it->second.addValue(now, value);
}

Monitor::Monitor() : impl_(folly::make_unique<MonitorImpl>()) {}

std::string Monitor::DoReport() { return impl_->Report(); }

std::string Monitor::Report() {
  return folly::Singleton<Monitor>::try_get()->DoReport();
}

void Monitor::Add(const std::string &key, int value) { impl_->Add(key, value); }

folly::Singleton<Monitor> gmonitor;

Marker::~Marker() {
  auto interval = std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::steady_clock::now() - time_point_).count();
  folly::Singleton<Monitor>::try_get()->Add(key_, interval);
}

} // namespace w
