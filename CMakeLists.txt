cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

project(libkngin)

# build option
set(CMAKE_BUILD_TYPE        "Debug")
set(CMAKE_CXX_FLAGS_DEBUG   "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb3 -pg -rdynamic")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O0 -Wall -rdynamic")

# include
include_directories("libkngin/include")
include_directories("libkngin")

# source
file(GLOB KNGIN_SRC "src/src.cpp")

# library
add_library(kngin        SHARED ${KNGIN_SRC})
add_library(kngin-static STATIC ${KNGIN_SRC})

# pthread-win32
set(CMAKE_PREFIX_PATH "libs/pthreads_win32" ${CMAKE_PREFIX_PATH})
if(WIN32)
    find_package(PTHREADW32 REQUIRED)
endif(WIN32)

# link
if(TARGET pthreadw32)
    target_link_libraries(kngin PUBLIC pthreadw32)
else()
    target_link_libraries(kngin PUBLIC -lpthread)
endif()

# property
set_target_properties(kngin
                      PROPERTIES
                      OUTPUT_NAME   "kngin"
                      PREFIX        "lib"
                      PUBLIC_HEADER "libkngin/include/libkngin.h"
                      )
set_target_properties(kngin-static
                      PROPERTIES
                      OUTPUT_NAME   "kngin"
                      PREFIX        "lib"
#                      PUBLIC_HEADER "libkngin/include/libkngin.h"
                      )

# install
install(TARGETS kngin kngin-static
        RUNTIME       DESTINATION ${PROJECT_SOURCE_DIR}/build/bin
        LIBRARY       DESTINATION ${PROJECT_SOURCE_DIR}/build/share
        ARCHIVE       DESTINATION ${PROJECT_SOURCE_DIR}/build/lib
        PUBLIC_HEADER DESTINATION ${PROJECT_SOURCE_DIR}/build/include
        )
