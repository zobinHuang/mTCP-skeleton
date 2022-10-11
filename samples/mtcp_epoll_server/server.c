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
#include <malloc.h>
#include <pthread.h>
#include <assert.h>
#include <sys/epoll.h>

// mtcp header
#include <mtcp_api.h>
#include <mtcp_epoll.h>

// libevent header
#include <event2/event.h>

// mtcp-skeleton header
#include <mtcp_skeleton_utils.h>
#include <mtcp_skeleton_debug.h>
#include <mtcp_skeleton_thread.h>
#include <mtcp_skeleton_server_config.h>

#include "server.h"

/*!
	\brief	initialize epoll server thread
			(internal called by epoll_server_thread)
	\param	arg	parameters passed to this thread
	\return	context of current server thread
 */
epoll_server_thread_context* _init_thread(void *arg){
	thread_arg _arg = *(thread_arg*)arg;

	// config thread context
	epoll_server_thread_context* ctx 
		= (epoll_server_thread_context*)malloc(sizeof(epoll_server_thread_context));
	if(!ctx){
		MTCP_SKELETON_ERROR_MESSAGE_THREAD(*(int*)_arg.args[0],
			"failed to allocate memory for epoll_server_thread_context")
		return NULL;
	}

	// init context
	ctx->num_socket = 0;
	ctx->mtcp_context = NULL;
	ctx->mtcp_epoll = -1;
	ctx->core = *(int*)_arg.args[0];
	MTCP_SKELETON_INFO_MESSAGE_THREAD(ctx->core,
		"launch sever thread");
	for(int i=0; i<EPOLL_MAX_EVENTS; i++){
		ctx->socket_list[i] = -1;
	}
	
	// affinitize application thread to a CPU core
	// #if HT_SUPPORT
	// 	mtcp_core_affinitize(ctx->core + (config->cpu_num/2));
	// #else
	// 	mtcp_core_affinitize(ctx->core);
	// #endif

	// create mtcp context, this will spawn an mtcp thread
	ctx->mtcp_context = mtcp_create_context(ctx->core);
	if (!ctx->mtcp_context) {
		MTCP_SKELETON_ERROR_MESSAGE_THREAD(ctx->core,
			"failed to create mtcp context");
		goto free_thread_context;
	}

	// create mtcp epoll descriptor
	ctx->mtcp_epoll = mtcp_epoll_create1(ctx->mtcp_context, 0);
	if(ctx->mtcp_epoll < 0) {
		MTCP_SKELETON_ERROR_MESSAGE_THREAD(ctx->core,
			"failed to create epoll descriptor");
		goto detroy_mtcp_context;
	}

	return ctx;

detroy_mtcp_context:
	mtcp_destroy_context(ctx->mtcp_context);

free_thread_context:
	free(ctx);

init_thread_exit:
	return NULL;
}

/*!
	\brief	create and config a new socket for server listenting
			(internal called by epoll_server_thread)
	\param	ctx	context of current server thread
	\return	the created socket
 */
int _create_listen_socket(epoll_server_thread_context *ctx){
	// create mtcp socket
	int sock = mtcp_socket(ctx->mtcp_context, AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		MTCP_SKELETON_ERROR_MESSAGE_THREAD(ctx->core,
			"failed to create mtcp socket")
		goto create_socket_exit;
	}

	// set the created socket as nonblocking
	int ret = mtcp_setsock_nonblock(ctx->mtcp_context, sock);
	if(ret < 0){
		MTCP_SKELETON_ERROR_MESSAGE_THREAD(ctx->core,
			"failed to set the created socket into non-blocking mode")
		goto close_socket;
	}

	// bind socket
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SOCKET_PORT);
	ret = mtcp_bind(ctx->mtcp_context, sock, 
			(struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
	if (ret < 0) {
		MTCP_SKELETON_ERROR_MESSAGE_THREAD(ctx->core,
			"failed bind created socket to port %d", SOCKET_PORT)
		goto close_socket;
	}
	MTCP_SKELETON_INFO_MESSAGE_THREAD(ctx->core,
		"bind to address %s:%d", SOCKET_IP, SOCKET_PORT)

	// listen
	ret = mtcp_listen(ctx->mtcp_context, sock, config->backlog);
	if(ret < 0){
		MTCP_SKELETON_ERROR_MESSAGE_THREAD(ctx->core,
			"failed listen socket")
		goto close_socket;
	}
	MTCP_SKELETON_INFO_MESSAGE_THREAD(ctx->core,
		"set the created socket as listen mode, backlog: %d", config->backlog)

	// create epoll event
	struct mtcp_epoll_event event;
	event.events = MTCP_EPOLLIN;
	event.data.sockid = sock;
	if(mtcp_epoll_ctl(
		ctx->mtcp_context, 
		ctx->mtcp_epoll, 
		MTCP_EPOLL_CTL_ADD, 
		sock, 
		&event)<0
	){
		MTCP_SKELETON_ERROR_MESSAGE_THREAD(ctx->core,
			"failed add MTCP_EPOLLIN event to the listen socket")
		goto close_socket;
	}
	
	return sock;

close_socket:
	mtcp_close(ctx->mtcp_context, sock);

create_socket_exit:
	return -1;
}

/*!
	\brief	close client connection
			(internal called by epoll_server_thread)
	\param	ctx		context of current server thread
	\param	sock	the socket to close
 */
void _close_connection(epoll_server_thread_context *ctx, int sock){
	// close socket within mtcp
	mtcp_epoll_ctl(
		ctx->mtcp_context, ctx->mtcp_epoll, MTCP_EPOLL_CTL_DEL, sock, NULL);
	mtcp_close(ctx->mtcp_context, sock);

	MTCP_SKELETON_WARNING_MESSAGE_THREAD(ctx->core,
		"socket %d connection closed", sock)

	// remove from socket list
	for(int i=0; i<EPOLL_MAX_EVENTS; i++){
		if(sock == ctx->socket_list[i]){
			ctx->socket_list[i] = -1;
			break;
		}
	}
}

/*!
	\brief	accpet client connection
			(internal called by epoll_server_thread)
	\param	ctx			context of current server thread
	\param	listen_sock	the listening socket
 */
int _accept_connection(epoll_server_thread_context *ctx, int listen_sock){
	// accept
	MTCP_SKELETON_INFO_MESSAGE_THREAD(ctx->core,
		"Try to accept connection")
	int new_sock = mtcp_accept(ctx->mtcp_context, listen_sock, NULL, NULL);

	// check whether valid
	if(new_sock >= 0){
		// check whether exceed the number of maximum number of flows
		if(new_sock >= EPOLL_MAX_EVENTS){
			assert(ctx->num_socket >= EPOLL_MAX_EVENTS);
			MTCP_SKELETON_WARNING_MESSAGE_THREAD(ctx->core,
				"abandon connection, reach maximum number of %d flows", EPOLL_MAX_EVENTS)
			goto close_socket;
		}

		// register mtcp epoll event
		struct mtcp_epoll_event event;
		event.events = MTCP_EPOLLIN;
		event.data.sockid = new_sock;
		int ret = mtcp_setsock_nonblock(ctx->mtcp_context, new_sock);
		if(ret < 0){
		MTCP_SKELETON_ERROR_MESSAGE_THREAD(ctx->core,
			"failed to set the newly accepted socket into non-blocking mode")
			goto close_socket;
		}
		mtcp_epoll_ctl(
			ctx->mtcp_context, ctx->mtcp_epoll, MTCP_EPOLL_CTL_ADD, new_sock, &event);
	
		// record in socket list
		for(int i=0; i<EPOLL_MAX_EVENTS; i++){
			if(ctx->socket_list[i] == -1){
				ctx->socket_list[i] = new_sock;
				ctx->num_socket += 1;
				break;
			}
		}

		MTCP_SKELETON_INFO_MESSAGE_THREAD(ctx->core,
			"accept new connection, socket id: %d", new_sock)

	} else { // failed acception
		if(errno != EAGAIN)
			MTCP_SKELETON_ERROR_ERRNO_THREAD(ctx->core, errno)
		goto exit_accept;
	}

	return new_sock;

close_socket:
	mtcp_close(ctx->mtcp_context, new_sock);

exit_accept:
	return -1;
}

void* epoll_server_thread(void *arg){
	// create the thread context, including
	// [1] mtcp context
	// [2] mtcp epoll descriptor
	epoll_server_thread_context* ctx = _init_thread(arg);
	if(!ctx)
		goto thread_exit;

	// create mtcp epoll event lists
	struct mtcp_epoll_event *events = (struct mtcp_epoll_event*)
		calloc(EPOLL_MAX_EVENTS, sizeof(struct mtcp_epoll_event));
	if(!events){
		MTCP_SKELETON_ERROR_MESSAGE_THREAD(ctx->core,
			"failed to allocate memory for mtcp epoll event")
		goto free_thread_context;
	}

	// create listen socket
	int listen_sock = _create_listen_socket(ctx);
	if(listen_sock < 0)
		goto free_mtcp_event;
	MTCP_SKELETON_INFO_MESSAGE_THREAD(ctx->core,
		"finish create and config the listen socket, enter epoll loop")

	// thread epoll loop
	int num_events = 0;
	while(!thread_dones[ctx->core]){
		MTCP_SKELETON_INFO_MESSAGE_THREAD(ctx->core,
			"listen socket: %d, epoll: %d", listen_sock, ctx->mtcp_epoll)
		num_events = mtcp_epoll_wait(
			ctx->mtcp_context, ctx->mtcp_epoll, events, EPOLL_MAX_EVENTS, EPOLL_TIMEOUT);
		
		// process epoll error
		if(num_events < 0){
			if(errno != EINTR)
				MTCP_SKELETON_ERROR_ERRNO_THREAD(ctx->core,errno)
			goto free_listen_socket;
		}

		bool waiting_acception = false;

		// process all available epoll events 
		for (int i=0; i<num_events; i++){
			if(events[i].data.sockid == listen_sock){
				// acception event, lazy processed
				waiting_acception = true;
				MTCP_SKELETON_INFO_MESSAGE_THREAD(ctx->core,
					"detect connect event, delay to process it")
			} else if(events[i].events & MTCP_EPOLLERR){
				// report error
				MTCP_SKELETON_WARNING_MESSAGE_THREAD(ctx->core,
					"error ocurred on socket %d", events[i].data.sockid)
				
				// ontain error details
				int err;
				socklen_t len = sizeof(err);
				if (mtcp_getsockopt(ctx->mtcp_context, events[i].data.sockid, 
						SOL_SOCKET, SO_ERROR, (void *)&err, &len) == 0) {
					if (err != ETIMEDOUT)
						MTCP_SKELETON_ERROR_ERRNO_THREAD(ctx->core, err)
					else
						MTCP_SKELETON_ERROR_MESSAGE_THREAD(ctx->core,
							"ETIMEDOUT error")
				} else {
					MTCP_SKELETON_WARNING_MESSAGE_THREAD(ctx->core,
						"failed to obtain error details")
				}
				_close_connection(ctx, events[i].data.sockid);
			} else if(events[i].events & MTCP_EPOLLIN){
				// read event
				// TODO
			} else if(events[i].events & MTCP_EPOLLOUT){
				// send event
				// TODO
			} else {
				MTCP_SKELETON_WARNING_MESSAGE_THREAD(ctx->core,
					"unknown event type %u", events[i].events)
			}
		}

		// accept all client connections
		if(waiting_acception){
			while(true){
				int ret = _accept_connection(ctx, listen_sock);
				if(ret == -1)
					break;
			}
		}
	}

free_listen_socket:
	mtcp_close(ctx->mtcp_context, listen_sock);

free_mtcp_event:
	free(events);

free_thread_context:
	MTCP_SKELETON_INFO_MESSAGE_THREAD(ctx->core,
		"sever thread quit");
	mtcp_destroy_context(ctx->mtcp_context);
	free(ctx);

thread_exit:
	pthread_exit(NULL);
}