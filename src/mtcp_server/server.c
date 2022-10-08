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
#include <sys/epoll.h>

// mtcp header
#include <mtcp_api.h>
#include <mtcp_epoll.h>

// libevent header
#include <event2/event.h>

// mtcp-app util header
#include <utils.h>
#include <mtcp_debug.h>

// server header
#include "server.h"

/*!
	\brief 	parse args
	\param	argc	number of args
	\param	argv	array of args
	\return	server configuration
 */
server_config* parse_args(int argc, char **argv){
	struct mtcp_conf mcfg;

    if (argc < 2) {
		MTCPAPP_ERROR_MESSAGE("please specify path of configuation file using -f")
		goto parse_args_exit;
	}

	server_config* sc = (server_config*)malloc(sizeof(server_config));
	if(!sc){
		MTCPAPP_ERROR("failed to allocate memory for struct server_config")
		goto parse_args_exit;
	}

	// obtain number of cpus
	sc->cpu_num = get_cpu_num();
	sc->cpu_limit = sc->cpu_num;

	// parse parameters
	int o = 0;
	while (-1 != (o = getopt(argc, argv, "N:f:"))){
		switch(o){
			case 'N':	// setup cpu limit
			{
				int temp_cpu_limit = mtcpapp_strtol(optarg, 10);
				if(temp_cpu_limit > sc->cpu_num){
					MTCPAPP_ERROR_MESSAGE(
						"CPU limitation should less than actual number of cpu cores")
					MTCPAPP_ERROR_MESSAGE(
						"%d cpu cores detected, given limitation %d cores",
						sc->cpu_num, temp_cpu_limit
					)
					goto free_server_config;
				}
				sc->cpu_limit = temp_cpu_limit;

				// setup cpu limit here before mtcp_init
				mtcp_getconf(&mcfg);
				mcfg.num_cores = sc->cpu_limit;
				mtcp_setconf(&mcfg);
				break;
			}

			case 'f':	// setup .conf file path
			{
				sc->conf_file = optarg;
				break;
			}

			default:;
		}
	}

	return sc;

free_server_config:
	free(sc);

parse_args_exit:
	return NULL;
}

/*!
	\brief 	create and config a new socket for server listenting
	\return	the created socket
 */
int init_epoll(){
	int epoll_fd = epoll_create1(0);
	if(epoll_fd == -1){
		MTCPAPP_ERROR_ERRNO(errno)
		return -1;
	}
	return epoll_fd;
}

/*!
	\brief 	create and config a new socket for server listenting
	\return	the created socket
 */
int init_listen_socket(){
	// create socket for listening
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        MTCPAPP_ERROR_ERRNO(errno)
		return -1;
    }

	// set up server address
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family  	= AF_INET;
    server_addr.sin_port 		= htons(SOCKET_SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SOCKET_SERVER_IP);

	// bind listen socket to address
	int ret = bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret < 0){
        MTCPAPP_ERROR_ERRNO(errno)
        close(sock);
		MTCPAPP_ERROR(
			"failed to bind socket to %s:%d, closed", SOCKET_SERVER_IP, SOCKET_SERVER_PORT)
		return -1;
    }
	MTCPAPP_INFO_MESSAGE(
		"bind listen socket to %s:%d", SOCKET_SERVER_IP, SOCKET_SERVER_PORT)

	// start listening
    ret = listen(sock, 0);
    if(ret < 0){
        MTCPAPP_ERROR_ERRNO(errno)
        return -1;
    }
	MTCPAPP_INFO_MESSAGE("bind socket start listening")

	return sock;
}

/*!
	\brief	accept new connection from listen socket
	\param 	listen_fd	the server listen socket
	\param	epoll_fd	the epoll handle
	\param 	socket_list	the global socket list
	\param	socket_num	the number of sockets in the given socket list
 */
void accept_connection(int listen_fd, int epoll_fd, int *socket_list, int socket_num){
	// initialize sockaddr_in
	struct sockaddr_in remote_addr;
	socklen_t len = sizeof(struct sockaddr_in);
	bzero(&remote_addr, len);

	// accept connection
	int con_fd = accept(listen_fd, (struct sockaddr*)&remote_addr, &len);
	if(con_fd < 0){
		MTCPAPP_ERROR_ERRNO(errno)
	}
	MTCPAPP_INFO_MESSAGE(
		"accept connection from %s:%d", 
		inet_ntoa(remote_addr.sin_addr),
		ntohs(remote_addr.sin_port)
	)

	// add newly accepted socket to global list
	socket_list[socket_num] = con_fd;
	socket_num++;

	// add to epoll fd
	struct epoll_event event;
	event.data.fd = con_fd;
	event.events =  EPOLLIN | EPOLLET;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, con_fd, &event);
}

/*!
	\brief 	process server receiving from remote client
	\param	recv_fd	socket that receive bytes from remote client	 	
 */
void process_recv(int recv_fd){
	char recv_buf[SOCKET_RECV_BUF_SIZE];
	int ret = recv(recv_fd, recv_buf, SOCKET_RECV_BUF_SIZE, 0);
	if(ret < 0){
		close(recv_fd);
		MTCPAPP_WARNING_MESSAGE("failed to recv from socket %d, closed", recv_fd)
	}
	recv_buf[ret] = '\0';
	MTCPAPP_INFO_MESSAGE("Recv: %s", recv_buf);
}

/*!
	\brief 	close all sockets in the given socket list
	\param	socket_list	the given socket list
	\param	socket_num	number of sockets in the given socket list
 */
void close_sockets(int *socket_list, int socket_num){
	for(int i=0; i<socket_num; i++){
		int s = socket_list[i];
		if(s < 0){
			MTCPAPP_WARNING_MESSAGE(
				"cant't close non-exist socket %d", s)
			continue;
		}

		close(s);
	}
}