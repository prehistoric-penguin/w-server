find_library(IBERTY_LIBRARIES NAMES libiberty.a)
find_library(EVENT_LIBRARIES NAMES libevent.a)
find_library(NUMA_LIBRARIES NAMES libnuma.a)
find_library(LZMA_LIBRARIES NAMES liblzma.a)

message(STATUS "lib iberty library: " ${IBERTY_LIBRARIES})
message(STATUS "lib event library: " ${EVENT_LIBRARIES})
message(STATUS "numa library: " ${NUMA_LIBRARIES})
message(STATUS "lzma library:" ${LZMA_LIBRARIES})
