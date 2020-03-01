cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

project(bits_test)

add_executable(bits_test ./bits_test.cpp)
target_link_libraries(bits_test kngin-static)
