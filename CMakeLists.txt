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
set(PATH_LIBMTCP_LIB ${PATH_LIBMTCP}/mtcp/lib)
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

# ====================== MTCP-skeleton Library ======================
SET(PATH_LIB_MTCP_SKELETON_SRC src)
file(GLOB_RECURSE SRC_LIB_MTCP_SKELETON ${PATH_LIB_MTCP_SKELETON_SRC}/*.c)
add_library(mtcp_skeleton ${SRC_LIB_MTCP_SKELETON})
target_include_directories(mtcp_skeleton PUBLIC include)
target_include_directories(mtcp_skeleton PUBLIC ${PATH_LIBMTCP}/mtcp/include)
target_include_directories(mtcp_skeleton PUBLIC ${PATH_LIBEVENT}/include)
target_include_directories(mtcp_skeleton PUBLIC ${PATH_LIBEVENT}/build/include)

# ====================== Sample Executables ======================
# path to sample code
SET(PATH_SAMPLE_SRC samples)

# legacy server code
SET(PATH_SAMPLE_LEGACY_SERVER_SRC ${PATH_SAMPLE_SRC}/legacy/server)
file(GLOB_RECURSE SRC_LEGACY_SERVER ${PATH_SAMPLE_LEGACY_SERVER_SRC}/*.c)
add_executable(sample_legacy_server ${SRC_LEGACY_SERVER})

# legacy client code
SET(PATH_SAMPLE_CLIENT_SRC ${PATH_SAMPLE_SRC}/legacy/client)
file(GLOB_RECURSE SRC_LEGACY_CLIENT ${PATH_SAMPLE_CLIENT_SRC}/*.c)
add_executable(sample_legacy_client ${SRC_LEGACY_CLIENT})

# mtcp server code
SET(PATH_SAMPLE_MTCP_SERVER_SRC ${PATH_SAMPLE_SRC}/mtcp_epoll_server)
file(GLOB_RECURSE SRC_MTCP_SERVER ${PATH_SAMPLE_MTCP_SERVER_SRC}/*.c)
add_executable(sample_mtcp_epoll_server ${SRC_MTCP_SERVER})

# mtcp server code
SET(PATH_SAMPLE_MTCP_CLIENT_SRC ${PATH_SAMPLE_SRC}/mtcp_client)
file(GLOB_RECURSE SRC_MTCP_CLIENT ${PATH_SAMPLE_MTCP_CLIENT_SRC}/*.c)
add_executable(sample_mtcp_client ${SRC_MTCP_CLIENT})

# ====================== Utils Executables ======================
# path to sample code
SET(PATH_UTILS_SRC utils)

# dpdk test code
SET(PATH_UTILS_DPDK_TEST_SRC ${PATH_UTILS_SRC}/dpdk_test)
file(GLOB_RECURSE SRC_DPDK_TEST ${PATH_UTILS_DPDK_TEST_SRC}/*.c)
add_executable(utils_dpdk_test ${SRC_DPDK_TEST})

set(TB_TARGETS 
    sample_legacy_server
    sample_legacy_client
    sample_mtcp_epoll_server
    sample_mtcp_client
    utils_dpdk_test
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

    # add mtcp-skeleton library
    target_link_libraries(${tb_target} mtcp_skeleton)

    # language feature
    target_compile_features(${tb_target} PUBLIC c_std_11)

    # include path
    target_include_directories(${tb_target} PUBLIC include)
    target_include_directories(${tb_target} PUBLIC ${PATH_LIBMTCP}/mtcp/include)
    target_include_directories(${tb_target} PUBLIC ${PATH_LIBEVENT}/include)
    target_include_directories(${tb_target} PUBLIC ${PATH_LIBEVENT}/build/include)
endforeach( tb_target ${TB_TARGETS} )