#include "stats.h"

#include <thread>
#include <atomic>

#include <glog/logging.h>
#include <folly/dynamic.h>
#include <folly/io/async/EventBase.h>
#include <folly/json.h>
#include <folly/Likely.h>
#include <folly/Memory.h>
#include <folly/Singleton.h>
#include <folly/SpinLock.h>
#include <folly/MPMCQueue.h>
#include <folly/stats/Histogram.h>
#include <folly/stats/Histogram-defs.h>
#include <folly/stats/TimeseriesHistogram.h>
#include <folly/stats/TimeseriesHistogram-defs.h>

namespace w {

namespace {
enum Levels { HALF_MINUTE, NUM_LEVELS };

const std::chrono::seconds kDurations[] = {std::chrono::seconds(30)};

constexpr int kBucketSize = 10;
constexpr int kBucketNum = 10;
constexpr int kMinHistogramValue = 0;
constexpr int kMaxHistogramValue = 10 * 1024 * 1024;
constexpr int kMaxGlobalQueueSize = 5000000;
constexpr int kPreCalculateTimerInterval = 30;
constexpr int kConsumeTimerInterval = 1;

using queue_item_t = std::pair<std::string, int>;

folly::MPMCQueue<queue_item_t> gQueue(kMaxGlobalQueueSize);

std::string gPreCalculatedStats;

folly::SpinLockMslImpl gPreCalculatedStatsLock;
} // namespace anonymous

class MonitorImpl {
public:
  using data_t =
      std::unordered_map<std::string, folly::TimeseriesHistogram<int>>;

  MonitorImpl();
  ~MonitorImpl();
  std::string Report();
  void Add(const std::string &key, int value);
  void AsyncAdd(const std::string &key, int value);

private:
  std::string Calculate();
  void PreCalculate();
  void ConsumeGlobalQueue();
  void AddPreCalculaterTimer();
  void AddConsumeGolbalQueueTimer();

  data_t data_;
  folly::EventBase eb_;
  std::thread thr_;
  std::unique_ptr<folly::AsyncTimeout> precalc_timer_;
  std::unique_ptr<folly::AsyncTimeout> consume_timer_;
};

MonitorImpl::MonitorImpl() : thr_([&]() { eb_.loopForever(); }) {
  LOG(INFO) << "ctor" << std::endl;
  eb_.waitUntilRunning();
  eb_.runInEventBaseThread([&]() { eb_.setName("Monitor"); });

  AddPreCalculaterTimer();
  AddConsumeGolbalQueueTimer();
}

MonitorImpl::~MonitorImpl() {
  LOG(INFO) << "dtor" << std::endl;
  eb_.runInEventBaseThread([&]() {
    precalc_timer_->cancelTimeout();
    consume_timer_.reset();
  });
  eb_.runInEventBaseThreadAndWait([&]() { eb_.terminateLoopSoon(); });

  thr_.join();
}

void MonitorImpl::AddPreCalculaterTimer() {
  eb_.runInEventBaseThread([&]() {
    precalc_timer_ = std::move(folly::AsyncTimeout::schedule(
        std::chrono::seconds(kPreCalculateTimerInterval), eb_,
        [&]() noexcept { PreCalculate(); }));
  });
}

void MonitorImpl::AddConsumeGolbalQueueTimer() {
  eb_.runInEventBaseThread([&]() {
    consume_timer_ = std::move(folly::AsyncTimeout::schedule(
        std::chrono::seconds(kConsumeTimerInterval), eb_,
        [&]() noexcept { ConsumeGlobalQueue(); }));
  });
}

void MonitorImpl::PreCalculate() {
  LOG(INFO) << "PreCalculate" << std::endl;
  std::string stats = Calculate();

  {
    folly::SpinLockGuardImpl<folly::SpinLockMslImpl> guard(
        gPreCalculatedStatsLock);
    gPreCalculatedStats = std::move(stats);
  }

  AddPreCalculaterTimer();
}

void MonitorImpl::ConsumeGlobalQueue() {
  LOG(INFO) << "ConsumeGlobalQueue" << std::endl;
  queue_item_t item;

  // drain out the queue
  // when the queue is very very long, it may block the whole event loop

  // aggregation by key
  // REFER: comment of TimeseriesHistogram.h, function addValues
  std::unordered_map<std::string, folly::Histogram<int>> tmp_map;
  while (gQueue.read(item)) {
    auto it = tmp_map.find(item.first);
    if (it == tmp_map.end()) {
      auto res_pair = tmp_map.emplace(
          item.first, folly::Histogram<int>(kBucketSize, kMinHistogramValue,
                                            kMaxHistogramValue));
      it = res_pair.first;
    }
    it->second.addValue(item.second);
  }

  auto now = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::steady_clock::now().time_since_epoch());

  // add by entire histogram
  for (const auto &kv : tmp_map) {
    auto it = data_.find(kv.first);
    if (UNLIKELY(it == data_.end())) {
      auto res_pair = data_.emplace(
          kv.first, folly::TimeseriesHistogram<int>(
                        kBucketSize, kMinHistogramValue, kMaxHistogramValue,
                        folly::MultiLevelTimeSeries<int>(
                            kBucketNum, Levels::NUM_LEVELS, kDurations)));
      it = res_pair.first;
    }
    it->second.addValues(now, kv.second);
  }

  AddConsumeGolbalQueueTimer();
}

std::string MonitorImpl::Report() {
  folly::SpinLockGuardImpl<folly::SpinLockMslImpl> guard(
      gPreCalculatedStatsLock);
  return gPreCalculatedStats;
}

std::string MonitorImpl::Calculate() {
  LOG(INFO) << "Calculate" << std::endl;

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
        folly::dynamic::object("accu", count)("qps", qps)("min", min)(
            "max", max)("avg", avg)("dist99", dist_99)("dist80", dist_80);
    mark.insert(key, std::move(inner_key_mark));
  }

  return folly::toJson(mark).toStdString();
}

void MonitorImpl::Add(const std::string &key, int value) {
  auto now = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::steady_clock::now().time_since_epoch());

  auto it = data_.find(key);
  if (UNLIKELY(it == data_.end())) {
    auto res_pair = data_.emplace(
        key, folly::TimeseriesHistogram<int>(
                 kBucketSize, kMinHistogramValue, kMaxHistogramValue,
                 folly::MultiLevelTimeSeries<int>(
                     kBucketNum, Levels::NUM_LEVELS, kDurations)));
    it = res_pair.first;
  }
  it->second.addValue(now, value);
}

void MonitorImpl::AsyncAdd(const std::string &key, int value) {
  gQueue.write(std::make_pair(key, value));
}

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
