cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(libkngin)

# build option
set(CMAKE_BUILD_TYPE        "Debug")
if(MSVC)
else()
set(CMAKE_CXX_FLAGS_DEBUG   "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb3 -pg -rdynamic")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O0 -Wall -rdynamic")
endif(MSVC)
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build/bin/)
set(LIBRARY_OUTPUT_PATH     ${PROJECT_SOURCE_DIR}/build/lib/)

# include directories
include_directories("${PROJECT_SOURCE_DIR}/libkngin")
include_directories("${PROJECT_SOURCE_DIR}")

add_subdirectory(libkngin)
add_subdirectory(libkngin_test)
