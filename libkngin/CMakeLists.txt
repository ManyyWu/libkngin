cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)

#if(WIN32)
#    find_package(PTHREADW32 REQUIRED)
#endif(WIN32)
project(libkngin_core)
project(libkngin_net)
add_subdirectory(core)
add_subdirectory(net)

set(KNGIN_CORE_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/core/)
set(KNGIN_NET_INC_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/net/
    ${CMAKE_CURRENT_BINARY_DIR}/net/udp/
    ${CMAKE_CURRENT_BINARY_DIR}/net/tcp/
    )
set(KNGIN_INC_DIR
    ${KNGIN_CORE_INC_DIR}
    ${KNGIN_NET_INC_DIR}
    # Other path
    )
include_directories(${KNGIN_INC_DIR})

# head files
#set(KNGIN_CORE_INC_DIR "core/")
#set(KNGIN_NET_INC_DIR
#    "net/"
#    "net/udp/"
#    "net/tcp/"
#    )
#if(WIN32)
#set(PTHREADS_WIN32_INC_DIR "../pthreads_win32/include/")
#endif(WIN32)
#set(KNGIN_INC_DIR
#    ${KNGIN_CORE_INC_DIR}
#    ${KNGIN_NET_INC_DIR}
#    ${PTHREADS_WIN32_INC_DIR}
#    # Other path
#    )
#include_directories(${KNGIN_INC_DIR})

# source files
#file(GLOB KNGIN_CORE_SRC "core/*.cpp")
#file(GLOB KNGIN_NET_SRC  "net/*.cpp")
#set(KNGIN_SRC
#    ${KNGIN_CORE_SRC}
#    ${KNGIN_NET_SRC}
#    )
#
## library
#add_library(libkngin ${KNGIN_SRC})
#link_libraries(libkngin libkngin_core libkngin_net)
#
#set(CMAKE_PREFIX_PATH "../pthreads_win32" ${CMAKE_PREFIX_PATH})
#if(TARGET pthreadw32)
#    target_link_libraries(libkngin PUBLIC pthreadw32)
#else()
#    target_link_libraries(libkngin PUBLIC -lpthread)
#endif()
