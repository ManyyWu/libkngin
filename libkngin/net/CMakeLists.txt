cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)

# net
#set(KNGIN_PARENT_DIR        ..)
#set(KNGIN_NET_INC_DIR       .)
#set(KNGIN_NET_UDP_INC_DIR   udp)
#set(KNGIN_NET_TCP_INC_DIR   tcp)
file(GLOB KNGIN_NET_SRC     "*.cpp")
file(GLOB KNGIN_NET_UDP_SRC "udp/*.cpp")
file(GLOB KNGIN_NET_TCP_SRC "tcp/*.cpp")

# show files
message(STATUS "source files in /libkngin/net:")
foreach(file IN LISTS KNGIN_NET_SRC)
    message(STATUS "  >>" ${file})
endforeach(file)
message(STATUS "  source files in /libkngin/net/udp:")
foreach(file IN LISTS KNGIN_NET_UDP_SRC)
    message(STATUS "    >>" ${file})
endforeach(file)
message(STATUS "  source files in /libkngin/net/tcp:")
foreach(file IN LISTS KNGIN_NET_TCP_SRC)
    message(STATUS "    >>" ${file})
endforeach(file)

# include
MESSAGE(STATUS "KNGIN_SOURCE_DIR: ${KNGIN_SOURCE_DIR}")
include_directories(${KNGIN_SOURCE_DIR})
#include_directories(${KNGIN_PARENT_DIR})
#include_directories(${KNGIN_NET_INC_DIR})
#include_directories(${KNGIN_NET_TCP_INC_DIR})
#include_directories(${KNGIN_NET_UDP_INC_DIR})

# libraries
add_library(kngin_net
            ${KNGIN_NET_SRC}
            ${KNGIN_NET_TCP_SRC}
            ${KNGIN_NET_UDP_SRC}
           )
