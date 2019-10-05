cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# head files
set(KNGIN_CORE_INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/../core/)
set(KNGIN_NET_INC_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/
    ${CMAKE_CURRENT_BINARY_DIR}/udp/
    ${CMAKE_CURRENT_BINARY_DIR}/tcp/
    )
set(KNGIN_INC_DIR
    ${KNGIN_CORE_INC_DIR}
    ${KNGIN_NET_INC_DIR}
    # Other path
    )
include_directories(${KNGIN_INC_DIR})

# source files
file(GLOB KNGIN_NET_SRC "*.cpp")

# libraries
add_library(libkngin_net ${KNGIN_NET_SRC})
