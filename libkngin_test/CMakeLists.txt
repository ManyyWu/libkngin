cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)

project(libkngin_test LANGUAGES C CXX)

# head files
include_directories(".")

# source files
file(GLOB KNGIN_TEST_SRC *.cpp)

# library
add_executable(libkngin_test ${KNGIN_TEST_SRC})

# link
target_link_libraries(libkngin_test kngin_net)
