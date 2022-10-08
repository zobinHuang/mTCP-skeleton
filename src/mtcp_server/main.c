// system header
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <limits.h>

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

int main(int argc, char **argv){
    // parse command line parameters
    server_config* config = parse_args(argc, argv);
    if(!config)
        goto exit;
    
    // init mtcp
    int ret = mtcp_init(config->conf_file);
    if(ret){
		MTCPAPP_ERROR_MESSAGE("failed to init mtcp")
		goto free_server_config;
	}

free_server_config:
    free(config);

exit:
    return 0;
}