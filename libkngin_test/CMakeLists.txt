cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)

project(libkngin_test LANGUAGES C CXX)

# head files
include_directories(".")

# source files
file(GLOB KNGIN_TEST_SRC *.cpp)
file(GLOB KNGIN_TEST_SRC_SFS simple_ftp_server/*.cpp)

# library
add_executable(libkngin_test
               ${KNGIN_TEST_SRC}
               ${KNGIN_TEST_SRC_SFS})

# link
target_link_libraries(libkngin_test kngin_net)
