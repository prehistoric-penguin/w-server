#include <iostream>

#include <gtest/gtest.h>
#include <folly/Singleton.h>
#include <folly/json.h>

#include "monitor/stats.h"

// for we use 30 sec as time interval in stats
// can not get actual qps when test running smaller than it,
// just check the key and accu here

TEST(monitor, json_str) {
  constexpr int n = 1000000;
  const std::string key = "general";
  const std::string key2 = "general.fin";
  for (int i = 0; i < n; ++i) {
    w::Marker m(key);

    w::Marker m2(key2);
    w::Marker m3(key2);
  }
  std::string json_str = w::Monitor::Report();
  auto dynamic_obj = folly::parseJson(folly::StringPiece(json_str));

  EXPECT_TRUE(dynamic_obj.find(key) != dynamic_obj.items().end());
  EXPECT_EQ(dynamic_obj[key]["accu"], n);
  EXPECT_EQ(dynamic_obj[key2]["accu"], n * 2);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv); 
  return RUN_ALL_TESTS(); 
}
