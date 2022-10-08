#ifndef _SERVER_CONFIG_H_
#define _SERVER_CONFIG_H_

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
    char    *conf_file;
} server_config;

int init_epoll();
int init_listen_socket();
server_config* parse_args(int argc, char **argv);
void accept_connection(int listen_fd, int epoll_fd, int *socket_list, int socket_num);
void process_recv(int recv_fd);
void close_sockets(int *socket_list, int socket_num);


#endif