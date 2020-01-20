cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)

project(kngin_core)

# sources
file(GLOB KNGIN_CORE_BASE_SRC  "base/*.cpp")
file(GLOB KNGIN_CORE_EVENT_SRC "event/*.cpp")

# print source files
message(STATUS "source files in /libkngin/core/base:")
foreach(file IN LISTS KNGIN_CORE_BASE_SRC)
    message(STATUS "  >>" ${file})
endforeach(file)
message(STATUS "source files in /libkngin/core/event:")
foreach(file IN LISTS KNGIN_CORE_EVENT_SRC)
    message(STATUS "  >>" ${file})
endforeach(file)

# pthread-win32
set(CMAKE_PREFIX_PATH "../../pthreads_win32" ${CMAKE_PREFIX_PATH})
if(WIN32)
    find_package(PTHREADW32 REQUIRED)
endif(WIN32)

# libraries
add_library(kngin_core 
            ${KNGIN_CORE_BASE_SRC}
            ${KNGIN_CORE_EVENT_SRC}
            )

# link
if(TARGET pthreadw32)
    target_link_libraries(kngin_core PUBLIC pthreadw32)
else()
    target_link_libraries(kngin_core PUBLIC -lpthread)
endif()
