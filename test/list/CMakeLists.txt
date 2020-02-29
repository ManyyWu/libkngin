cmake_minimum_required(VERSION 3.13)
project(test)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(test main.cpp kernel_list.h list.h)

target_link_libraries (test boost_system pthread)

#add_executable(server boost_server_test.cpp)
#target_link_libraries (server boost_system pthread)
#
#add_executable(client boost_client_test.cpp )
#target_link_libraries (client boost_system pthread)

#set(BOOST_ROOT /usr/include/boost)
#
#find_package(Boost COMPONENTS regex system thread REQUIRED)
#
#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#
#    MESSAGE( STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}.")
#    MESSAGE( STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}.")
#    MESSAGE( STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}.")
#
#    add_executable(test boost_test.cpp)
#    target_link_libraries (test ${Boost_LIBRARIES})
#endif()
