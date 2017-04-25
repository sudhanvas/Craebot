cmake_minimum_required (VERSION 2.6)

set (POD_NAME craebot)
include (cmake/pods.cmake)
include (cmake/lcmtypes.cmake)
lcmtypes_build ()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

add_subdirectory (src)
