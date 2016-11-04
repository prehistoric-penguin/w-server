#include <iostream>

#include <vector>
#include <thread>

#include <gtest/gtest.h>
#include <folly/Singleton.h>
#include <folly/json.h>
#include <folly/stats/Histogram.h>
#include <folly/stats/Histogram-defs.h>

#include "monitor/stats.h"

// a infinite loop here
// check CPU usage in TOP command
TEST(monitor, json_str) {
  constexpr int n = 200000;
  const std::string key = "general";
  const std::string key2 = "general.fin";

  std::vector<std::thread> thrs;

  for (int i = 0; i < 4; ++i) {
    thrs.emplace_back([&]() {
      while (true) {
        for (int j = 0; j < n; ++j) {
          w::Marker m(key);

          w::Marker m2(key2);
          w::Marker m3(key2);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    });
  }

  for (auto &t : thrs)
    t.join();
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
