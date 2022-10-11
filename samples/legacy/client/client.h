#ifndef _CLIENT_CONFIG_H_
#define _CLIENT_CONFIG_H_

int init_socket();
int connect_socket(int client_fd);

// socket config
#define SOCKET_SEND_BUF_SIZE    128
#define SOCKET_RECV_BUF_SIZE    128
#define SOCKET_SERVER_IP        "10.0.20.2"
#define SOCKET_SERVER_PORT      80
#define SOCKET_CLIENT_IP        "10.0.20.3"
#define SOCKET_CLIENT_PORT      10000

#endif
