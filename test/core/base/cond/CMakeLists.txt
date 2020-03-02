cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus /EHa")
else()
    set(CMAKE_CXX_STANDARD 17)
endif(MSVC)

project(cond_test)

add_executable(cond_test cond_test.cpp)
target_link_libraries(cond_test kngin-static)
