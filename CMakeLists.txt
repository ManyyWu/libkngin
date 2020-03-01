cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

project(libkngin)

set(VERSION "0.10")
set(SOVERSION "0.1")

# build option
set(CMAKE_BUILD_TYPE        "Debug")
set(CMAKE_CXX_FLAGS_DEBUG   "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb3 -pg -rdynamic")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O0 -Wall -rdynamic")
set(LIBRARY_OUTPUT_PATH     ${PROJECT_SOURCE_DIR}/build/test/)
set(CMAKE_INSTALL_PREFIX    ${PROJECT_SOURCE_DIR}/build)

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
    target_link_libraries(kngin-static PUBLIC pthreadw32)
else()
    target_link_libraries(kngin PUBLIC -lpthread)
    target_link_libraries(kngin-static PUBLIC -lpthread)
endif()

# property
set_target_properties(kngin PROPERTIES
                      OUTPUT_NAME   "kngin"
                      VERSION       ${VERSION}
                      SOVERSION     ${SOVERSION}
                      )
set_target_properties(kngin-static PROPERTIES
                      OUTPUT_NAME    "kngin"
                      )

set_target_properties(kngin PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(kngin-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# install
install(TARGETS kngin kngin-static
        RUNTIME       DESTINATION bin
        LIBRARY       DESTINATION share
        ARCHIVE       DESTINATION lib
        PUBLIC_HEADER DESTINATION include
        )
install(FILES libkngin/include/libkngin.h DESTINATION include)
