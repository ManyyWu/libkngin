cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# head files
set(KNGIN_CORE_INC_DIR "../core/")
set(KNGIN_INC_DIR ${KNGIN_CORE_INC_DIR})
include_directories(${KNGIN_INC_DIR})

# source files
file(GLOB KNGIN_CORE_SRC "*.cpp")
add_library(libkngin_core ${KNGIN_CORE_SRC})

# libraries
add_library(libkngin ${KNGIN_SRC})
if(WIN32)
    find_package(PTHREADW32 REQUIRED)
endif(WIN32)
link_libraries(libkngin libkngin_core libkngin_net)
set(CMAKE_PREFIX_PATH "../pthreads_win32" ${CMAKE_PREFIX_PATH})
if(TARGET pthreadw32)
    target_link_libraries(libkngin PUBLIC pthreadw32)
else()
    target_link_libraries(libkngin PUBLIC -lpthread)
endif()
