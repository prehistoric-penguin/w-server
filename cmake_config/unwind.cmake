find_library(UNWIND_LIBRARIE_PLATFORM_SPECIFIC NAMES libunwind-x86_64.a)
find_library(UNWIND_LIBRARIE NAMES libunwind.a)

if (NOT UNWIND_LIBRARIE)
    MESSAGE(FATAL_ERROR "failed to find unwind generic library")
endif ()

if (NOT UNWIND_LIBRARIE_PLATFORM_SPECIFIC)
    MESSAGE(FATAL_ERROR "failed to find unwind platform specific library")
endif ()

SET(LIBUNWIND_LIBRARIES ${UNWIND_LIBRARIE} ${UNWIND_LIBRARIE_PLATFORM_SPECIFIC})  
message(STATUS "libunwind libraries: " ${LIBUNWIND_LIBRARIES})
