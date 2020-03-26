cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /Zc:__cplusplus")
else()
  set(CMAKE_CXX_STANDARD 17)
endif(MSVC)

project(libkngin)

set(VERSION "0.10")
set(SOVERSION "0.1")

# build option
set(CMAKE_BUILD_TYPE                "Debug")
if(MSVC)
else()
set(CMAKE_CXX_FLAGS_DEBUG           "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb3 -pg -rdynamic")
set(CMAKE_CXX_FLAGS_RELEASE         "$ENV{CXXFLAGS} -O0 -Wall -rdynamic")
endif(MSVC)
set(LIBRARY_OUTPUT_PATH             ${PROJECT_SOURCE_DIR}/build/bin)
set(RUNTIME_SHARED_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/shared)
set(CMAKE_INSTALL_PREFIX            ${PROJECT_SOURCE_DIR}/build)

# include
include_directories("${PROJECT_SOURCE_DIR}/libkngin/include")
include_directories("${PROJECT_SOURCE_DIR}/libkngin")

# source
file(GLOB KNGIN_SRC "${PROJECT_SOURCE_DIR}/src/src.cpp")

# library
add_library(kngin        SHARED ${KNGIN_SRC})
add_library(kngin-static STATIC ${KNGIN_SRC})

# pthread-win32
if(WIN32)
  set(CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/libs/pthreads_win32" ${CMAKE_PREFIX_PATH})
  find_package(pthreadw32 REQUIRED)
endif(WIN32)

# property
set(USE_POSIX_THREAD 1)
set(USE_WIN_THREAD 0)
if(USE_POSIX_THREAD)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_POSIX_SEMAPHORE)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_POSIX_COND)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_POSIX_MUTEX)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_POSIX_RMUTEX)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_POSIX_RWLOCK)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_POSIX_BARRIER)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_POSIX_THREAD)
endif(USE_POSIX_THREAD)
if(WIN32)
if(USE_WIN_THREAD)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_WIN_SEMAPHORE)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_WIN_COND)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_WIN_MUTEX)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_WIN_RMUTEX)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_WIN_RWLOCK)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_WIN_BARRIER)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_WIN_THREAD)
endif(USE_WIN_THREAD)
endif(WIN32)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_STD_FUNCTION_THREAD_PROC)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_STD_FUNCTION_LOG_CALLBACK)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_STD_FUNCTION_EVENT_LOOP_TASK)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_STD_FUNCTION_EVENT_LOOP_HANDLER)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_STD_FUNCTION_TIMER_HANDLER)
#target_compile_definitions(kngin PUBLIC -DENABLE_USE_MONOTONIC_TIMER)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_TIMERFD_TIMER)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_RELATIVE_LOG_PATH)
#target_compile_definitions(kngin PUBLIC -DENABLE_USE_ASYNC_LOGGER)
target_compile_definitions(kngin PUBLIC -DASYNC_LOGGER_TIMEOUT=3000ULL)
target_compile_definitions(kngin PUBLIC -DTIMER_REMAINING_PRESISION=0ULL)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_EPOLL_REACTOR)
target_compile_definitions(kngin PUBLIC -DENABLE_USE_STD_FUNCTION_SESSION_HANDLER)
target_compile_definitions(kngin PUBLIC -DENABLE_SESSION_NO_MUTEX)
target_compile_definitions(kngin PUBLIC -DENABLE_SESSION_USE_ET_MODE)

if(USE_POSIX_THREAD)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_POSIX_SEMAPHORE)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_POSIX_COND)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_POSIX_MUTEX)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_POSIX_RMUTEX)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_POSIX_RWLOCK)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_POSIX_BARRIER)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_POSIX_THREAD)
endif(USE_POSIX_THREAD)
if(USE_WIN_THREAD)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_WIN_SEMAPHORE)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_WIN_COND)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_WIN_MUTEX)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_WIN_RMUTEX)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_WIN_RWLOCK)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_WIN_BARRIER)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_WIN_THREAD)
endif(USE_WIN_THREAD)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_STD_FUNCTION_THREAD_PROC)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_STD_FUNCTION_LOG_CALLBACK)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_STD_FUNCTION_EVENT_LOOP_TASK)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_STD_FUNCTION_EVENT_LOOP_HANDLER)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_STD_FUNCTION_TIMER_HANDLER)
#target_compile_definitions(kngin PUBLIC -DENABLE_USE_MONOTONIC_TIMER)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_TIMERFD_TIMER)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_RELATIVE_LOG_PATH)
#target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_ASYNC_LOGGER)
target_compile_definitions(kngin-static PUBLIC -DASYNC_LOGGER_TIMEOUT=3000ULL)
target_compile_definitions(kngin-static PUBLIC -DTIMER_REMAINING_PRESISION=0ULL)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_EPOLL_REACTOR)
target_compile_definitions(kngin-static PUBLIC -DENABLE_USE_STD_FUNCTION_SESSION_HANDLER)
target_compile_definitions(kngin-static PUBLIC -DENABLE_SESSION_NO_MUTEX)
target_compile_definitions(kngin-static PUBLIC -DENABLE_SESSION_USE_ET_MODE)

set_target_properties(kngin PROPERTIES OUTPUT_NAME kngin)
set_target_properties(kngin PROPERTIES PREFIX      lib)
set_target_properties(kngin PROPERTIES VERSION     ${VERSION})
set_target_properties(kngin PROPERTIES SOVERSION   ${SOVERSION})

set_target_properties(kngin-static PROPERTIES OUTPUT_NAME kngin)
set_target_properties(kngin-static PROPERTIES PREFIX      lib)

set_target_properties(kngin PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(kngin-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

if(MSVC)
set_target_properties(kngin PROPERTIES COMPILE_FLAGS "/EHa")
set_target_properties(kngin-static PROPERTIES COMPILE_FLAGS "/EHa")
endif(MSVC)

# link
if(TARGET pthreadw32)
  target_link_libraries(kngin PUBLIC pthreadw32)
  target_link_libraries(kngin-static PUBLIC pthreadw32)
else()
  target_link_libraries(kngin PUBLIC -lpthread)
  target_link_libraries(kngin-static PUBLIC -lpthread)
endif()

# install
install(TARGETS kngin kngin-static
        RUNTIME       DESTINATION bin
        LIBRARY       DESTINATION shared
        ARCHIVE       DESTINATION lib
        PUBLIC_HEADER DESTINATION include
        )
install(FILES ${PROJECT_SOURCE_DIR}/libkngin/include/libkngin.h DESTINATION include)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/libkngin/include/kngin DESTINATION include)

add_subdirectory(test)