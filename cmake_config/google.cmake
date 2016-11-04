find_path(GTEST_HEADERS gtest/gtest.h)
find_library(GTEST_LIBRARIES NAMES libgtest.a)
find_library(GTEST_MAIN_LIBRARIES NAMES libgtest_main.a)
message(STATUS "gtest header:" ${GTEST_HEADERS})
message(STATUS "gtest library:" ${GTEST_LIBRARIES})
message(STATUS "gtest main library:" ${GTEST_MAIN_LIBRARIES})


find_path(GFLAGS_HEADERS gflags/gflags.h)
find_library(GFLAGS_LIBRARIES NAMES libgflags.a)
message(STATUS "gflags header:" ${GFLAGS_HEADERS})
message(STATUS "gflags library:" ${GFLAGS_LIBRARIES})


find_path(GLOG_HEADERS glog/logging.h)
find_library(GLOG_LIBRARIES NAMES libglog.a)
message(STATUS "glog include: " ${GLOG_HEADERS})
message(STATUS "glog library: " ${GLOG_LIBRARIES})


find_path(SNAPPY_HEADERS snappy.h)
find_library(SNAPPY_LIBRARIES NAMES libsnappy.a)
message(STATUS "snappy header:" ${SNAPPY_HEADERS})
message(STATUS "snappy library: " ${SNAPPY_LIBRARIES})


find_library(DOUBLE_CONVERSION_LIBRARIES NAMES libdouble-conversion.a)
message(STATUS "double_conversion library: " ${DOUBLE_CONVERSION_LIBRARIES})
