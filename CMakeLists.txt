cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)

project(libkngin LANGUAGES C CXX)
project(libkngin_test LANGUAGES C CXX)

add_subdirectory(libkngin)
add_subdirectory(libkngin_test)
