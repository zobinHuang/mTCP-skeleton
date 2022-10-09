#ifndef _MTCP_SKELETON_DEBUG_UTILS_H_
#define _MTCP_SKELETON_DEBUG_UTILS_H_

#include <stdio.h>
#include <stdbool.h>

/*!
    USAGE of Debug Utils:
    This header provide several useful printer macros for debugging

    [1] MTCP_SKELETON_ERROR_ERRNO(errno)
        Print message error information based on given errno

        MTCP_SKELETON_ERROR_ERRNO_THREAD(core_id,errno)
        Print per-thread message error information based on given errno

    [2] MTCP_SKELETON_ERROR(...)
        - Print error message in details (i.e. with specific code address)

        MTCP_SKELETON_ERROR_THREAD(core_id,...)
        - Print per-thread error message in details (i.e. with specific code address)

        MTCP_SKELETON_ERROR_MESSAGE(...)
        - Print error message

        MTCP_SKELETON_ERROR_MESSAGE_THREAD(core_id,...)
        - Print per-thread error message
    
    [3] MTCP_SKELETON_WARNING(...)
        - Print warning message in details (i.e. with specific code address)

        MTCP_SKELETON_WARNING_THREAD(core_id,...)
        - Print per-thread warning message in details (i.e. with specific code address)

        MTCP_SKELETON_WARNING_MESSAGE(...)
        - Print warning message

        MTCP_SKELETON_WARNING_MESSAGE_THREAD(core_id,...)
        - Print per-thread warning message
    
    [4] MTCP_SKELETON_INFO(...)
        - Print info message in details (i.e. with specific code address)

        MTCP_SKELETON_INFO_THREAD(core_id,...)
        - Print per-thread info message in details (i.e. with specific code address)

        MTCP_SKELETON_INFO_MESSAGE(...)
        - Print info message

        MTCP_SKELETON_INFO_MESSAGE_THREAD(core_id,...)
        - Print per-thread info message
 */

// enable this macro to print debug info
#define CADS_DEBUG_INFO_ENABLE true

// print errno
#define MTCP_SKELETON_ERROR_ERRNO(errno) \
{\
MTCP_SKELETON_ERROR("Errno - %s (errno: %d)\n", strerror(errno), errno)\
}

// print per-thread errno
#define MTCP_SKELETON_ERROR_ERRNO_THREAD(core_id,errno) \
{\
MTCP_SKELETON_ERROR_MESSAGE("Thead on core %d reports:", core_id) \
MTCP_SKELETON_ERROR_ERRNO(errno) \
}

// print error message with details
#define MTCP_SKELETON_ERROR(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[101m\033[97m MTCP-App Error \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, ";\n\
\033[33mfile:\033[0m       %s;\n\
\033[33mfunction:\033[0m   %s;\n\
\033[33mline:\033[0m       %d;\n\n", __FILE__, __func__, __LINE__);\
}

// print error message
#define MTCP_SKELETON_ERROR_MESSAGE(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[101m\033[97m MTCP-App Error \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, "\n\n");\
}

// print per-thread error message with details
#define MTCP_SKELETON_ERROR_THREAD(core_id,...) \
{\
MTCP_SKELETON_ERROR_MESSAGE("Thead on core %d reports:", core_id)\
MTCP_SKELETON_ERROR(__VA_ARGS__)\
}

// print per-thread error message
#define MTCP_SKELETON_ERROR_MESSAGE_THREAD(core_id,...) \
{\
MTCP_SKELETON_ERROR_MESSAGE("Thead on core %d reports:", core_id)\
MTCP_SKELETON_ERROR_MESSAGE(__VA_ARGS__)\
}

// print warning message with details
#define MTCP_SKELETON_WARNING(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[103m\033[97m MTCP-App Warning \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, ";\n\
\033[33mfile:\033[0m       %s;\n\
\033[33mfunction:\033[0m   %s;\n\
\033[33mline:\033[0m       %d;\n\n", __FILE__, __func__, __LINE__);\
}

// print warning message
#define MTCP_SKELETON_WARNING_MESSAGE(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[103m\033[97m MTCP-App Warning \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, "\n\n");\
}

// print per-thread warning message with details
#define MTCP_SKELETON_WARNING_THREAD(core_id,...) \
{\
MTCP_SKELETON_WARNING_MESSAGE("Thead on core %d reports:", core_id)\
MTCP_SKELETON_WARNING(__VA_ARGS__)\
}

// print per-thread warning message
#define MTCP_SKELETON_WARNING_MESSAGE_THREAD(core_id,...) \
{\
MTCP_SKELETON_WARNING_MESSAGE("Thead on core %d reports:", core_id)\
MTCP_SKELETON_WARNING_MESSAGE(__VA_ARGS__)\
}


#if(CADS_DEBUG_INFO_ENABLE)
// print info message with details
#define MTCP_SKELETON_INFO(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[104m\033[97m MTCP-App Info \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, ";\n\
\033[33mfile:\033[0m       %s;\n\
\033[33mfunction:\033[0m   %s;\n\
\033[33mline:\033[0m       %d;\n\n", __FILE__, __func__, __LINE__);\
}

// print info message
#define MTCP_SKELETON_INFO_MESSAGE(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[104m\033[97m MTCP-App Info \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, "\n\n");\
}

// print per-thread info message with details
#define MTCP_SKELETON_INFO_THREAD(core_id,...) \
{\
MTCP_SKELETON_INFO_MESSAGE("Thead on core %d reports:", core_id)\
MTCP_SKELETON_INFO(__VA_ARGS__)\
}

// print per-thread info message
#define MTCP_SKELETON_INFO_MESSAGE_THREAD(core_id,...) \
{\
MTCP_SKELETON_INFO_MESSAGE("Thead on core %d reports:", core_id)\
MTCP_SKELETON_INFO_MESSAGE(__VA_ARGS__)\
}
#else
#define MTCP_SKELETON_INFO(...)
#define MTCP_SKELETON_INFO_MESSAGE(...)
#define MTCP_SKELETON_INFO_THREAD(core_id,...)
#define MTCP_SKELETON_INFO_MESSAGE_THREAD(core_id,...)
#endif

#endif