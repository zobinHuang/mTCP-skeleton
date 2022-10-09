#ifndef _MTCP_SKELETON_SERVER_CONFIG_H_
#define _MTCP_SKELETON_SERVER_CONFIG_H_

// epoll config
#define EPOLL_MAX_EVENTS    100
#define EPOLL_TIMEOUT       3000

// socket config
#define SOCKET_MAX_NUM          50
#define SOCKET_RECV_BUF_SIZE    128
#define SOCKET_SERVER_IP        "10.0.20.1"
#define SOCKET_SERVER_PORT      10000

typedef struct server_config{
    int     cpu_num;
    int     cpu_limit;
    int     cpu_offset;
    int     backlog;
    bool    multi_threaded;
    char    *conf_file;
} server_config;

int init_epoll();
int init_listen_socket();
void* mtcp_server_thread(void *arg);
void accept_connection(int listen_fd, int epoll_fd, int *socket_list, int socket_num);
void process_recv(int recv_fd);
void close_sockets(int *socket_list, int socket_num);


#endif