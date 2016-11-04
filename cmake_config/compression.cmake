find_library(LIB_BZ2 NAMES libbz2.a)
find_library(LIB_ZIP NAMES libzip.a)
find_library(LIB_LZ4 NAMES liblz4.a)
find_library(LIB_Z NAMES libz.a)
find_library(LIB_SNAPPY NAMES libsnappy.a)
find_library(LIB_LZMA NAMES liblzma.a)


set(COMPRESSION_LIBRARIES
  ${LIB_BZ2}
  ${LIB_ZIP}
  ${LIB_LZ4}
  ${LIB_Z}
  ${LIB_SNAPPY}
  ${LIB_LZMA}
)


message(STATUS "compression libraries:" ${COMPRESSION_LIBRARIES})
