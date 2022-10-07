# cmake version
cmake_minimum_required(VERSION 3.16.3)

# project info
project(mTCPapp LANGUAGES C)

# set executable output path
set(PATH_EXECUTABLE bin)
execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ../${PATH_EXECUTABLE})
SET(EXECUTABLE_OUTPUT_PATH ../${PATH_EXECUTABLE})

# find mTCP pre-built library
find_library(MTCP_LIBRARY NAMES mtcp libmtcp 
    HINTS ${CMAKE_CURRENT_LIST_DIR}/lib)

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

# ====================== utils Library ======================
file(GLOB_RECURSE SRC_UTILS "utils/*.c")
add_library(UTILS_LIBRARY ${SRC_UTILS})
target_include_directories(UTILS_LIBRARY PUBLIC utils/include)
target_include_directories(UTILS_LIBRARY PUBLIC include)

# ====================== Application Executables ======================
SET(PATH_APP src)
file(GLOB_RECURSE SRC_EPSERVER ${PATH_APP}/epserver.c)
add_executable(epserver ${SRC_EPSERVER})

set(TB_TARGETS 
    epserver
)

foreach( tb_target ${TB_TARGETS} )
    # add library
    target_link_libraries(${tb_target} UTILS_LIBRARY)

    # add pre-built library
    target_link_libraries(${tb_target} ${MTCP_LIBRARY})
    target_link_libraries(${tb_target} ${DPDK_LDFLAGS})
    target_link_libraries(${tb_target} -lpthread -lm -lgmp)

    # language feature
    target_compile_features(${tb_target} PUBLIC c_std_11)

    # include path
    target_include_directories(${tb_target} PUBLIC /usr/local/include)
    target_include_directories(${tb_target} PUBLIC include/io_engine)
    target_include_directories(${tb_target} PUBLIC include/mtcp)
    target_include_directories(${tb_target} PUBLIC utils/include)
endforeach( tb_target ${TB_TARGETS} )