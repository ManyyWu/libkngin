cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)

#find_package(Threads REQUIRED)

# core
set(KNGIN_CORE_INC_DIR    ../libkngin/core)

# net
set(KNGIN_NET_INC_DIR     ../libkngin/net)
set(KNGIN_NET_UDP_INC_DIR ../libkngin/net/udp)
set(KNGIN_NET_TCP_INC_DIR ../libkngin/net/tcp)

#head files
include_directories(".")
include_directories(${KNGIN_CORE_INC_DIR})
include_directories(${KNGIN_NET_INC_DIR})
include_directories(${KNGIN_NET_TCP_INC_DIR})
include_directories(${KNGIN_NET_UDP_INC_DIR})

file(GLOB KNGIN_TEST_SRC *.cpp)
add_executable(libkngin_test ${KNGIN_TEST_SRC})
#add_executable(libkngin_test
#               log_test.cpp
#               mutex_test.cpp
#               thread_test.cpp
#               rwlock_test.cpp
#               cond_test.cpp
##               threadpoll_test.cpp
#               sync_queue_test.cpp
#               sync_deque_test.cpp
##               work_thread_test.cpp
#               buffer_test.cpp
#               test.cpp
#               main.cpp
#               )

target_link_libraries(libkngin_test
                      kngin)
