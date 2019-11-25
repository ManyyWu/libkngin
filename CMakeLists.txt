cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)

project(kngin         LANGUAGES C CXX)
project(libkngin_test LANGUAGES C CXX)

# build option
set(CMAKE_BUILD_TYPE        "Debug")
set(CMAKE_CXX_FLAGS_DEBUG   "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg -rdynamic")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O0 -Wall -rdynamic")
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build/bin/)
set(LIBRARY_OUTPUT_PATH     ${PROJECT_SOURCE_DIR}/build/lib/)

# set global include directory
set(KNGIN_SOURCE_DIR "${PROJECT_SOURCE_DIR}/libkngin/" CACHE INTERNAL "project source directory" )

add_subdirectory(libkngin)
add_subdirectory(libkngin_test)
