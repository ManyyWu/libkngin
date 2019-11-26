cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)

project(libkngin_core)

# sources
file(GLOB KNGIN_CORE_SRC "*.cpp")

# print source files
message(STATUS "source files in /libkngin/core:")
foreach(file IN LISTS KNGIN_CORE_SRC)
    message(STATUS "  >>" ${file})
endforeach(file)

# pthread-win32
set(CMAKE_PREFIX_PATH "../../pthreads_win32" ${CMAKE_PREFIX_PATH})
if(WIN32)
    find_package(PTHREADW32 REQUIRED)
endif(WIN32)

# libraries
add_library(kngin_core ${KNGIN_CORE_SRC})

# link
if(TARGET pthreadw32)
    target_link_libraries(kngin_core PUBLIC pthreadw32)
else()
    target_link_libraries(kngin_core PUBLIC -lpthread)
endif()
