cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

project(system_error_test)

add_executable(system_error_test system_error.cpp)
target_link_libraries(system_error_test kngin-static)
