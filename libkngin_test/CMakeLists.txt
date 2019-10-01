cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads REQUIRED)

add_executable(libkngin_test
               log_test.cpp
               mutex_test.cpp
               thread_test.cpp
               rwlock_test.cpp
               cond_test.cpp
               threadpoll_test.cpp
               sync_queue_test.cpp
               sync_deque_test.cpp
               work_thread_test.cpp
               main.cpp)

target_link_libraries(libkngin_test
                      libkngin)
