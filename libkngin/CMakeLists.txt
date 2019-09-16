cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(KNGIN_CORE_INC_DIR
    core/)

set(KNGIN_CORE_SRC
    core/common.cpp
    core/error.cpp
    core/log.cpp
    core/time.cpp
    core/thread.cpp
    core/logmgr.cpp
    core/threadpool.cpp
    core/mutex.h
    core/rwlock.h
    core/cond.h
    core/msg.cpp
)

set(KNGIN_INC_DIR
    ${KNGIN_CORE_INC_DIR}
    # Other library
    )

set(KNGIN_SRC
    ${KNGIN_CORE_SRC}
    # Other path
    )

add_library(libkngin
            ${KNGIN_SRC}
            )
