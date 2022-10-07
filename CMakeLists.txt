# cmake version
cmake_minimum_required(VERSION 3.16.3)

# project info
project(mTCPapp LANGUAGES C)

# set executable output path
set(PATH_EXECUTABLE bin)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ../${PATH_EXECUTABLE})
SET(EXECUTABLE_OUTPUT_PATH ../${PATH_EXECUTABLE})

# set library output path
set(PATH_LIBRARY lib)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ../${PATH_LIBRARY})
SET(LIBRARY_OUTPUT_PATH ../${PATH_LIBRARY})

# find mTCP pre-built library
set(PATH_LIBMTCP ${CMAKE_CURRENT_LIST_DIR}/third_party/mtcp)
set(PATH_LIBMTCP_LIB ${CMAKE_CURRENT_LIST_DIR}/third_party/mtcp)
find_library(LIB_MTCP NAMES mtcp HINTS ${PATH_LIBMTCP_LIB})

# check whether DPDK exist
find_package(PkgConfig REQUIRED)
if (PKG_CONFIG_FOUND)
       pkg_check_modules(DPDK "libdpdk")
       if (DPDK_FOUND)
            message(STATUS "found dpdk via pkg-config")
       endif()
endif()

# obtain DPDK ldflags
execute_process(
    COMMAND pkg-config --libs libdpdk
    OUTPUT_VARIABLE DPDK_LDFLAGS
)
string(STRIP ${DPDK_LDFLAGS} DPDK_LDFLAGS)

# obtain libevent libraries
set(PATH_LIBEVENT ${CMAKE_CURRENT_LIST_DIR}/third_party/libevent)
set(PATH_LIBEVENT_LIB ${PATH_LIBEVENT}/build/lib)
find_library(LIB_EVENT_CORE NAMES event_core HINTS ${PATH_LIBEVENT_LIB})
find_library(LIB_EVENT_EXTRA NAMES event_extra HINTS ${PATH_LIBEVENT_LIB})
find_library(LIB_EVENT_OPENSSL NAMES event_openssl HINTS ${PATH_LIBEVENT_LIB})
find_library(LIB_EVENT_PTHREADS NAMES event_pthreads HINTS ${PATH_LIBEVENT_LIB})
find_library(LIB_EVENT NAMES event HINTS ${PATH_LIBEVENT_LIB})

# ====================== Application Executables ======================
SET(PATH_APP src)
file(GLOB_RECURSE SRC_SERVER ${PATH_APP}/server.c)
add_executable(epoll_server ${SRC_SERVER})

set(TB_TARGETS 
    epoll_server
)

foreach( tb_target ${TB_TARGETS} )
    # add pre-built library
    target_link_libraries(${tb_target} ${LIB_MTCP})
    target_link_libraries(${tb_target} ${LIB_EVENT_CORE})
    target_link_libraries(${tb_target} ${LIB_EVENT_EXTRA})
    target_link_libraries(${tb_target} ${LIB_EVENT_OPENSSL})
    target_link_libraries(${tb_target} ${LIB_EVENT_PTHREADS})
    target_link_libraries(${tb_target} ${LIB_EVENT})
    target_link_libraries(${tb_target} ${DPDK_LDFLAGS})
    
    target_link_libraries(${tb_target} -lpthread -lm -lgmp)

    # language feature
    target_compile_features(${tb_target} PUBLIC c_std_11)

    # include path
    target_include_directories(${tb_target} PUBLIC ${PATH_LIBMTCP}/include)
    target_include_directories(${tb_target} PUBLIC ${PATH_LIBEVENT}/include)
    target_include_directories(${tb_target} PUBLIC ${PATH_LIBEVENT}/build/include)
endforeach( tb_target ${TB_TARGETS} )