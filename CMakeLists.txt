cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)

project(kngin         LANGUAGES C CXX)
project(libkngin_test LANGUAGES C CXX)

# build option
set(CMAKE_BUILD_TYPE        "Debug")
set(CMAKE_CXX_FLAGS_DEBUG   "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O0 -Wall")
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build/bin/)
set(LIBRARY_OUTPUT_PATH     ${PROJECT_SOURCE_DIR}/build/lib/)

add_subdirectory(libkngin)
add_subdirectory(libkngin_test)
