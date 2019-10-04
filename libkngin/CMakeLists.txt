cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)

# head files
set(KNGIN_CORE_INC_DIR
    "core/"
    )
set(KNGIN_NET_INC_DIR
    "net/"
    "net/udp/"
    "net/tcp/"
    )
if(WIN32)
set(PTHREADS_WIN32_INC_DIR
    "../pthreads_win32/include/"
    )
endif(WIN32)
set(KNGIN_INC_DIR
    ${KNGIN_CORE_INC_DIR}
    ${KNGIN_NET_INC_DIR}
    ${PTHREADS_WIN32_INC_DIR}
    # Other path
    )
include_directories(${KNGIN_INC_DIR})

# source files
set(KNGIN_CORE_SRC
    "core/common.cpp"
    "core/error.cpp"
    "core/buffer.cpp"
    "core/exception.cpp"
    "core/log.cpp"
    "core/timestamp.cpp"
    "core/thread.cpp"
    "core/logmgr.cpp"
    "core/mutex.cpp"
    "core/rwlock.cpp"
    "core/cond.cpp"
#    "core/threadpool.cpp"
#    "core/task_base.cpp"
#    "net/msg.cpp"
#    "net/work_task.cpp"
#    "net/work_thread.cpp"
    )
set(KNGIN_NET_SRC
    "net/net_buffer.cpp"
    "net/epoll.cpp"
    "net/epoll_event.cpp"
    "net/msg.cpp"
#    "net/work_task.cpp"
#    "net/work_thread.cpp"
    "net/socket.cpp"
    "net/event_loop.cpp"
    "net/event.cpp"
    "net/tcp/tcp_connection.cpp"
    "net/tcp/tcp_client.cpp"
    )
set(KNGIN_SRC
    ${KNGIN_CORE_SRC}
    ${KNGIN_NET_SRC}
    # Other path
    )

# library
set(CMAKE_PREFIX_PATH "../pthreads_win32" ${CMAKE_PREFIX_PATH})

if(WIN32)
    find_package(PTHREADW32 REQUIRED)
endif(WIN32)

add_library(libkngin ${KNGIN_SRC})

if(TARGET pthreadw32)
    target_link_libraries(libkngin PUBLIC pthreadw32)
else()
    target_link_libraries(libkngin PUBLIC -lpthread)
endif()
