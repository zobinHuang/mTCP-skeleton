// system header
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// server header
#include "client.h"

// mtcp-app util header
#include <mtcp_skeleton_debug.h>

/*!
	\brief 	create a new socket, bind to specify local address
	\return	the created socket
 */
int init_socket(){
	// create socket for listening
	int sock;
    int option = 1;
	sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        MTCP_SKELETON_ERROR_ERRNO(errno)
		return -1;
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	// bind local address
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(SOCKET_CLIENT_IP);
    client_addr.sin_port = htons(SOCKET_CLIENT_PORT);
    int ret = bind(sock, (struct sockaddr*)&client_addr, sizeof(client_addr));
    if(ret < 0){
        MTCP_SKELETON_ERROR_ERRNO(errno)
        close(sock);
		MTCP_SKELETON_ERROR(
			"failed to bind socket to %s:%d, closed", SOCKET_CLIENT_IP, SOCKET_CLIENT_PORT)
		return -1;
    }
	MTCP_SKELETON_INFO_MESSAGE(
		"bind listen socket to %s:%d", SOCKET_CLIENT_IP, SOCKET_CLIENT_PORT)

	return sock;
}

/*!
	\brief 	connect socket to remote server
	\return	status code
 */
int connect_socket(int client_fd){
    struct sockaddr_in remote_addr;
    remote_addr.sin_family      = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(SOCKET_SERVER_IP);
    remote_addr.sin_port        = htons(SOCKET_SERVER_PORT);
    int ret = connect(client_fd, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if(ret < 0){
        MTCP_SKELETON_ERROR_ERRNO(errno)
		MTCP_SKELETON_ERROR(
			"failed to connect socket to %s:%d", SOCKET_SERVER_IP, SOCKET_SERVER_PORT)
		return -1;
    }

    return 0;
}