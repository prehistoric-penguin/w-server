cmake_minimum_required(VERSION 2.8)
project(w-server)

add_definitions (-g -O2 -std=c++11)

set(WSERVER "w-server")

#link_libraries(-static-libstdc++ -static-libgcc)
# include config
FILE(GLOB CMAKE_CONFIGS ${CMAKE_SOURCE_DIR}/cmake_config/*.cmake)
foreach(config ${CMAKE_CONFIGS})
  include(${config})
endforeach()

set(SOURCE_DIR "src")

include_directories(
  ${JEMALLOC_HEADERS}
  ${ROCKS_HEADERS}
  ${SNAPPY_HEADERS}
  ${GLOG_HEADERS}
  ${GTEST_HEADERS}
  ${GFLAGS_HEADERS}
  ${FOLLY_HEADERS}
  ${WANGLE_HEADERS}
  ${PROXYGEN_HEADERS}
  ${Boost_INCLUDE_DIRS}
  ${SOURCE_DIR}
)

file(GLOB_RECURSE GEN_CPP_SRC "src/gen-cpp2/*.cpp")
file(GLOB_RECURSE HTTP_SERVICE_SRC "src/http_service/*.cpp")
file(GLOB_RECURSE MONITOR_SRC "src/monitor/*.cpp")
file(GLOB_RECURSE THRIFT_SERVICE_SRC "src/thrift_service/*.cpp")
set(MAIN_FILE "src/main.cpp")

set(COMMON_LIB_SRC
  ${GEN_CPP_SRC}
  ${HTTP_SERVICE_SRC}
  ${MONITOR_SRC}
  ${THRIFT_SERVICE_SRC}
)

# build common library
add_library(server_common
  ${COMMON_LIB_SRC}
)

add_executable(${WSERVER}
  ${MAIN_FILE}
)


set(DEPEND_LIBS
  ${FOLLY_LIBRARIES}
  ${FBTHRIFT_LIBRARIES}
  ${ROCKSDB_LIBRRARIES}
  ${LIB_JEMMALLOC}
  #${COMPRESSION_LIBS}
  ${Boost_LIBRARIES}
  #${ENCRYPT_LIBRARIES}
  ${GLOG_LIBRARIES}
  ${GFLAGS_LIBRARIES}
  ${DOUBLE_CONVERSION_LIBRARIES}
  ${IBERTY_LIBRARIES}
  ${NUMA_LIBRARIES}
  ${JEMALLOC_LIBRARIES}
  ${LIBUNWIND_LIBRARIES}
  libthrift.a
  libthriftcpp2.a
  libthriftz.a
  libsaslstubs.a

  ${PROXYGEN_HTTP_SERVER_LIBRARIES}
  ${PROXYGEN_LIBRARIES}
  ${WANGLE_LIBRARIES} 
  ${FOLLY_LIBRARIES}

  #${LZMA_LIBRARIES}
  pthread
  dl
  rt
  event
  crypto
  crypt
  k5crypto
  krb5
  lzma
  snappy
  gssapi_krb5
  ssl
  sasl2
  libbz2.a
  liblz4.a
  z
)

# link w-server
target_link_libraries(${WSERVER}
  server_common
  ${DEPEND_LIBS}
)

file(GLOB_RECURSE TEST_SRC "test/*.cpp")

link_libraries(
  server_common
  ${DEPEND_LIBS}
  ${GTEST_LIBRARIES}
)

# build unit test
foreach(i ${TEST_SRC})
  get_filename_component(name ${i} NAME_WE)
  add_executable(${name} ${i})
endforeach()

# build dist package
set(DIST_ROOT_DIR "./w-server-dist/")
set(DIST_MAIN_DIR "${DIST_ROOT_DIR}/usr/local/w-server")
set(DIST_ETC_DIR "${DIST_ROOT_DIR}/etc")
set(DIST_DEBIAN_DIR "${DIST_ROOT_DIR}/DEBIAN")

add_custom_target(dist
  COMMAND rm -rf ${DIST_ROOT_DIR}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_MAIN_DIR}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_MAIN_DIR}/bin
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_MAIN_DIR}/distribute
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_MAIN_DIR}/log
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_ETC_DIR}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_ETC_DIR}/init.d
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_DEBIAN_DIR}
  
  COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/distribute/run.sh ${DIST_MAIN_DIR}
  COMMAND chmod +x ${DIST_MAIN_DIR}/run.sh
  
  COMMAND cp -f ${CMAKE_CURRENT_BINARY_DIR}/${WSERVER} ${DIST_MAIN_DIR}/bin
  COMMAND cp -rf ${CMAKE_CURRENT_SOURCE_DIR}/distribute/conf/* ${DIST_MAIN_DIR}/distribute
  
  COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/distribute/z-server-service ${DIST_ETC_DIR}/init.d
  COMMAND chmod +x ${DIST_ETC_DIR}/init.d/z-server-service
  
  COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/distribute/control ${DIST_DEBIAN_DIR}
  
  COMMAND tar -czvf w-server-dist.tar.gz ${DIST_ROOT_DIR}
  COMMAND dpkg-deb -b ${DIST_ROOT_DIR}
  DEPENDS ${WSERVER}
)
