cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus")
else()
    set(CMAKE_CXX_STANDARD 17)
endif(MSVC)

project(address_test)

add_executable(address_test address_test.cpp)
target_link_libraries(address_test kngin-static)
