cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

project(exception_test)

add_executable(exception_test ./exception.cpp)
target_link_libraries(exception_test kngin-static)
