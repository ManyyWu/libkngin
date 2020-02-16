cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus /EHa")
else()
set(CMAKE_CXX_STANDARD 17)
endif(MSVC)

project(libkngin_net)

# sources
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

# libraries
add_library(kngin_net
            ${KNGIN_NET_SRC}
            ${KNGIN_NET_TCP_SRC}
            ${KNGIN_NET_UDP_SRC}
            )
target_link_libraries(kngin_net PUBLIC kngin_core)
