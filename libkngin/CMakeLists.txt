cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# head files
set(KNGIN_CORE_INC_DIR
    "core/"
    )

set(PTHREADS_WIN32_INC_DIR
    "../pthreads_win32/include/"
    )

set(KNGIN_INC_DIR
    ${KNGIN_CORE_INC_DIR}
    ${PTHREADS_WIN32_INC_DIR}
    # Other path
    )

# source files
set(KNGIN_CORE_SRC
    "core/common.cpp"
    "core/error.cpp"
    "core/log.cpp"
    "core/time.cpp"
    "core/thread.cpp"
    "core/logmgr.cpp"
    "core/msg.cpp"
    "core/mutex.cpp"
    "core/rwlock.cpp"
    "core/cond.cpp"
    "core/threadpool.cpp"
    "core/task_base.cpp"
    "core/work_task.cpp"
    "core/work_thread.cpp"
)

set(KNGIN_SRC
    ${KNGIN_CORE_SRC}
    # Other path
    )

# library
set(CMAKE_PREFIX_PATH "../pthreads_win32" ${CMAKE_PREFIX_PATH})

if(WIN32)
    find_package(PTHREADW32 REQUIRED)
endif(WIN32)

add_library(libkngin
            ${KNGIN_SRC}
            )

if(TARGET pthreadw32)
    target_link_libraries(libkngin PUBLIC pthreadw32)
else()
    target_link_libraries(libkngin PUBLIC -lpthread)
endif()
