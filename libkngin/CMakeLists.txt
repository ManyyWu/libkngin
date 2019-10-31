cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)

# core
set(KNGIN_CORE_INC_DIR   core)
file(GLOB KNGIN_CORE_SRC "core/*.cpp")

# net
set(KNGIN_NET_INC_DIR         net)
set(KNGIN_NET_UDP_INC_DIR     net/udp)
set(KNGIN_NET_TCP_INC_DIR     net/tcp)
file(GLOB KNGIN_NET_SRC       "net/*.cpp")
file(GLOB KNGIN_NET_UDP_SRC   "net/udp/*.cpp")
file(GLOB KNGIN_NET_TCP_SRC   "net/tcp/*.cpp")

# pthread-win32
set(CMAKE_PREFIX_PATH "../pthreads_win32" ${CMAKE_PREFIX_PATH})
if(WIN32)
    find_package(PTHREADW32 REQUIRED)
endif(WIN32)

# head directories
include_directories(".")
include_directories(${KNGIN_CORE_INC_DIR})
include_directories(${KNGIN_NET_INC_DIR})
include_directories(${KNGIN_NET_TCP_INC_DIR})
include_directories(${KNGIN_NET_UDP_INC_DIR})

# list information
message(STATUS "======================== information ========================")
message(STATUS "source files:")
foreach(file IN LISTS KNGIN_CORE_SRC)
    message(STATUS "  >>" ${file})
endforeach(file)
foreach(file IN LISTS KNGIN_NET_SRC)
    message(STATUS "  >>" ${file})
endforeach(file)
foreach(file IN LISTS KNGIN_NET_UDP_SRC)
    message(STATUS "    >>" ${file})
endforeach(file)
foreach(file IN LISTS KNGIN_NET_TCP_SRC)
    message(STATUS "    >>" ${file})
endforeach(file)
message(STATUS "head directories:")
message(STATUS "  >>" ${KNGIN_CORE_INC_DIR})
message(STATUS "  >>" ${KNGIN_NET_INC_DIR})
message(STATUS "  >>" ${KNGIN_NET_TCP_INC_DIR})
message(STATUS "  >>" ${KNGIN_NET_UDP_INC_DIR})
message(STATUS "")
message(STATUS "======================== build start ========================")

# libraries
add_library(kngin
            ${KNGIN_CORE_SRC}
            ${KNGIN_NET_SRC}
            ${KNGIN_NET_TCP_SRC}
            ${KNGIN_NET_UDP_SRC})

# link
if(TARGET pthreadw32)
    target_link_libraries(kngin PUBLIC pthreadw32)
else()
    target_link_libraries(kngin PUBLIC -lpthread)
endif()
