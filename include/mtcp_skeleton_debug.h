#ifndef _MTCP_SKELETON_DEBUG_UTILS_H_
#define _MTCP_SKELETON_DEBUG_UTILS_H_

#include <stdio.h>
#include <stdbool.h>

// enable this macro to print debug info
#define CADS_DEBUG_INFO_ENABLE true

// print errno
#define MTCP_SKELETON_ERROR_ERRNO(errno) \
{\
MTCP_SKELETON_ERROR("Errno - %s (errno: %d)\n", strerror(errno), errno)\
}

// print error message with details
#define MTCP_SKELETON_ERROR(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[101m\033[97m MTCP-App Error \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, ";\n\
\033[33mfile:\033[0m       %s;\n\
\033[33mfunction:\033[0m   %s;\n\
\033[33mline:\033[0m       %d;\n", __FILE__, __func__, __LINE__);\
}

// print error message
#define MTCP_SKELETON_ERROR_MESSAGE(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[101m\033[97m MTCP-App Error \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, "\n");\
}

// print warning message with details
#define MTCP_SKELETON_WARNING(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[103m\033[97m MTCP-App Warning \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, ";\n\
\033[33mfile:\033[0m       %s;\n\
\033[33mfunction:\033[0m   %s;\n\
\033[33mline:\033[0m       %d;\n", __FILE__, __func__, __LINE__);\
}

// print error message
#define MTCP_SKELETON_WARNING_MESSAGE(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[103m\033[97m MTCP-App Warning \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, "\n");\
}

// print info message with details
#if(CADS_DEBUG_INFO_ENABLE)
#define MTCP_SKELETON_INFO(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[104m\033[97m MTCP-App Info \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, ";\n\
\033[33mfile:\033[0m       %s;\n\
\033[33mfunction:\033[0m   %s;\n\
\033[33mline:\033[0m       %d;\n", __FILE__, __func__, __LINE__);\
}
#else
#define MTCP_SKELETON_INFO(...)
#endif

// print info message
#if(CADS_DEBUG_INFO_ENABLE)
#define MTCP_SKELETON_INFO_MESSAGE(...) \
{\
fprintf(stderr, "\033[31m%s\033[0m \033[104m\033[97m MTCP-App Info \033[0m ",  __TIME__);\
fprintf(stderr, __VA_ARGS__);\
fprintf(stderr, "\n");\
}
#else
#define MTCP_SKELETON_INFO_MESSAGE(...)
#endif

#endif