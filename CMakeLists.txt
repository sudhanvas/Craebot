cmake_minimum_required (VERSION 2.8)

find_program (CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set_property (GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif ()

set (POD_NAME craebot)
include (cmake/pods.cmake)
include (cmake/zcmtypes.cmake)
zcmtypes_build ()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

# Required of clang-tidy to work.
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory (src)
