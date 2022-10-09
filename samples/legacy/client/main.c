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

int main(){
    int sock = init_socket();
    if(sock == -1)
        goto exit;
    
    int ret = connect_socket(sock);
    if(ret == -1)
        goto close_socket;

    char sendbuf[SOCKET_SEND_BUF_SIZE];
    memset(sendbuf, '\0', sizeof(sendbuf));
    strcpy(sendbuf, "Hi!");
    ret = send(sock, sendbuf, strlen(sendbuf)+1, 0);
    if(ret <= 0){
        MTCP_SKELETON_ERROR_ERRNO(errno)
        goto close_socket;
    }
    MTCP_SKELETON_INFO_MESSAGE("send '%s' to remote server", sendbuf);

close_socket:
	if(sock >= 0){
		close(sock);
		MTCP_SKELETON_INFO_MESSAGE(
			"close socket %d", sock)
	}
	else
		MTCP_SKELETON_WARNING_MESSAGE(
			"cant't close non-exist socket %d", sock)

exit:
    return 0;
}