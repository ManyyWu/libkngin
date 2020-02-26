cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus /EHa")
else()
set(CMAKE_CXX_STANDARD 17)
endif(MSVC)

# head files
include_directories(".")

# source files
file(GLOB KNGIN_TEST_SRC *.cpp)
file(GLOB KNGIN_TEST_SRC_SFS simple_ftp_server/*.cpp)

# library
add_executable(libkngin_test
               ${KNGIN_TEST_SRC}
               ${KNGIN_TEST_SRC_SFS})

# link
target_link_libraries(libkngin_test kngin_net)