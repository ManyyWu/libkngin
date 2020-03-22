cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus")
else()
    set(CMAKE_CXX_STANDARD 17)
endif(MSVC)

project(rmutex_test)

add_executable(rmutex_test rmutex_test.cpp)
target_link_libraries(rmutex_test kngin-static)
