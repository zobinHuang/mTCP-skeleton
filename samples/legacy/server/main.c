// system header
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>

// mtcp header
#include <mtcp_api.h>
#include <mtcp_epoll.h>

// libevent header
#include <event2/event.h>

// mtcp-app util header
#include <mtcp_skeleton_debug.h>

// server header
#include "server.h"

struct epoll_event 	event_list[EPOLL_MAX_EVENTS];
int	socket_list[SOCKET_MAX_NUM], socket_num = 0;

int main(){
	// create epoll instance
	int epoll_fd = init_epoll();
	if(epoll_fd == -1)
		goto exit;

	// initialize listen socket
	int listen_fd = init_listen_socket(epoll_fd);
	if(listen_fd == -1)
		goto close_listen_socket;

	// init epoll event for listening
	struct epoll_event event_listen;
	event_listen.events = EPOLLIN | EPOLLET;
	event_listen.data.fd = listen_fd;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event_listen)){
		MTCP_SKELETON_ERROR("Errno - %s (errno: %d)\n", strerror(errno), errno)
		goto close_epoll_fd;
	}

	// init epoll event for stopping server via command line input
	struct epoll_event event_stop;
	event_stop.events = EPOLLIN;
	event_stop.data.fd = 0;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event_stop)){
		MTCP_SKELETON_ERROR("Errno - %s (errno: %d)\n", strerror(errno), errno)
		goto close_epoll_fd;
	}

	int event_count;
	while(true){
		event_count = epoll_wait(epoll_fd, event_list, EPOLL_MAX_EVENTS, EPOLL_TIMEOUT);
		// epoll error
		if(event_count < 0){
			MTCP_SKELETON_ERROR("Errno - %s (errno: %d)\n", strerror(errno), errno)
			goto close_epoll_fd;
		}

		// epoll timeout
		if(event_count == 0)
			continue;

		// process event
		for(int i = 0; i < event_count; i++){
			// handle error event
			if ((event_list[i].events & EPOLLERR) ||
                (event_list[i].events & EPOLLHUP) ||
                !(event_list[i].events & EPOLLIN))
            {
              close(event_list[i].data.fd);
			  MTCP_SKELETON_WARNING_MESSAGE(
				"Error occurs on socket %d, closed", event_list[i].data.fd)
            }

			// handle new connection event
			if(event_list[i].data.fd == listen_fd){
				accept_connection(listen_fd, epoll_fd, socket_list, socket_num);
				continue;
			}

			// handle stop event
			if(event_list[i].data.fd == 0){
				close_sockets(socket_list, socket_num);
				goto close_listen_socket;
			}

			// handler normal client message
			process_recv(event_list[i].data.fd);
		}
	}

close_listen_socket:
	if(listen_fd >= 0){
		close(listen_fd);
		MTCP_SKELETON_INFO_MESSAGE(
			"close listen socket %d", listen_fd)
	}
	else
		MTCP_SKELETON_WARNING_MESSAGE(
			"cant't close non-exist socket %d", listen_fd)

close_epoll_fd:
	if(epoll_fd >= 0){
		close(epoll_fd);
		MTCP_SKELETON_INFO_MESSAGE(
			"close epoll handle %d", epoll_fd)
	}
	else
		MTCP_SKELETON_WARNING_MESSAGE(
			"cant't close non-exist epoll instance %d", epoll_fd)

exit:
	return 0;
}