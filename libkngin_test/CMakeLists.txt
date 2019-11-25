cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)

# head files
include_directories(".")

# source files
file(GLOB KNGIN_TEST_SRC *.cpp)

# library
add_executable(libkngin_test ${KNGIN_TEST_SRC})

# link
target_link_libraries(libkngin_test
                      kngin)
