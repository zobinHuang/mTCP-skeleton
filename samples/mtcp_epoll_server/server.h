#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdbool.h>

// mtcp header
#include <mtcp_api.h>
#include <mtcp_epoll.h>

#define HT_SUPPORT  false

#define EPOLL_MAX_EVENTS 1024
#define EPOLL_TIMEOUT 3000

#define SOCKET_IP   "10.0.109.2"
#define SOCKET_PORT 10000

typedef struct epoll_server_thread_context{
    int     core;
    int     mtcp_epoll;
    int     num_socket;
    int     socket_list[EPOLL_MAX_EVENTS];
    mctx_t  mtcp_context;
} epoll_server_thread_context;

void* epoll_server_thread(void *arg);

#endif