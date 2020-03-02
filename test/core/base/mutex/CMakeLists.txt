cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus /EHa")
else()
    set(CMAKE_CXX_STANDARD 17)
endif(MSVC)

project(mutex_test)

add_executable(mutex_test mutex_test.cpp)
target_link_libraries(mutex_test kngin-static)
