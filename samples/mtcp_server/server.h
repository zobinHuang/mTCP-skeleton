#ifndef _SERVER_H_
#define _SERVER_H_

int init_epoll();
int init_listen_socket();
void* mtcp_server_thread(void *arg);
void accept_connection(int listen_fd, int epoll_fd, int *socket_list, int socket_num);
void process_recv(int recv_fd);
void close_sockets(int *socket_list, int socket_num);

#endif