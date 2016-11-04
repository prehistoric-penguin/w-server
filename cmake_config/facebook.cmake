find_path(JEMALLOC_HEADERS jemalloc/jemalloc.h)
find_library(JEMALLOC_LIBRARIES NAMES libjemalloc.a)
message(STATUS "jemalloc header:" ${JEMALLOC_HEADERS})
message(STATUS "jemalloc library:" ${JEMALLOC_LIBRARIES})


find_path(ROCKS_HEADERS rocksdb/db.h)
find_library(ROCKSDB_LIBRRARIES NAMES librocksdb.a)
message(STATUS "rocksdb header:" ${ROCKS_HEADERS})
message(STATUS "rocksdb library:" ${ROCKSDB_LIBRRARIES})


find_path(FOLLY_HEADERS folly/AtomicHashMap.h)
find_library(FOLLY_LIBRARIES NAMES libfolly.a)
message(STATUS "folly header:" ${FOLLY_HEADERS})
message(STATUS "folly library:" ${FOLLY_LIBRARIES})


find_path(WANGLE_HEADERS wangle/acceptor)
find_library(WANGLE_LIBRARIES NAMES libwangle.a)
message(STATUS "wangle header:" ${WANGLE_HEADERS})
message(STATUS "lib wangle library:" ${WANGLE_LIBRARIES})


find_path(PROXYGEN_HEADERS proxygen/httpserver/HTTPServer.h)
find_library(PROXYGEN_LIBRARIES NAMES libproxygenlib.a)
find_library(PROXYGEN_HTTP_SERVER_LIBRARIES NAMES libproxygenhttpserver.a)
message(STATUS "proxygen header:" ${PROXYGEN_HEADERS})
message(STATUS "proxygen library:" ${PROXYGEN_LIBRARIES})
message(STATUS "proxygen http server library:" ${PROXYGEN_HTTP_SERVER_LIBRARIES})

find_library(FBTHRIFT_LIBRARY libthrift.a)
find_library(FBTHRIFTCPP2_LIBRARY libthriftcpp2.a)
find_library(FBTHRIFTZ_LIBRARY libthriftz.a)
find_library(FBTHRIFTSTUBS_LIBRARY libsaslstubs.a)

set(FBTHRIFT_LIBRARIES
  ${FBTHRIFT_LIBRARY}
  ${FBTHRIFTCPP2_LIBRARY}
  ${FBTHRIFTZ_LIBRARY}
  ${FBTHRIFTSTUBS_LIBRARY}
)

message(STATUS "fbthrift libraries: " ${FBTHRIFT_LIBRARIES})
