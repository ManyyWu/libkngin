cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus /EHa")
else()
    set(CMAKE_CXX_STANDARD 17)
endif(MSVC)

project(system_error_test)

add_executable(system_error_test system_error_test.cpp)
target_link_libraries(system_error_test kngin-static)
