cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

find_package(Threads REQUIRED)

add_executable(libkngin_test
               log_test.cpp
               mutex_test.cpp
               thread_test.cpp
               rwlock_test.cpp
               cond_test.cpp
               threadpoll_test.cpp
               msg_test.cpp
               main.cpp)

target_link_libraries(libkngin_test
                      libkngin)
