cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)

project(libkngin LANGUAGES C CXX)

add_subdirectory(core)
add_subdirectory(net)
