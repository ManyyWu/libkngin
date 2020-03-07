cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus /EHa")
else()
    set(CMAKE_CXX_STANDARD 17)
endif(MSVC)

project(buffer_test)

add_executable(buffer_test buffer_test.cpp)
target_link_libraries(buffer_test kngin-static)
