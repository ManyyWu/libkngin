cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

# build option
set(CMAKE_BUILD_TYPE        "Debug")
set(CMAKE_CXX_FLAGS_DEBUG   "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb3 -pg -rdynamic")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O0 -Wall -rdynamic")
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build/bin/)
set(LIBRARY_OUTPUT_PATH     ${PROJECT_SOURCE_DIR}/build/lib/)

# include directories
include_directories("${PROJECT_SOURCE_DIR}/libkngin")
include_directories("${PROJECT_SOURCE_DIR}")

add_subdirectory(libkngin)
add_subdirectory(libkngin_test)
