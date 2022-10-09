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

// mtcp-skeleton header
#include <mtcp_skeleton_args.h>
#include <mtcp_skeleton_utils.h>
#include <mtcp_skeleton_debug.h>
#include <mtcp_skeleton_thread.h>
#include <mtcp_skeleton_server_config.h>

#include "server.h"

int main(int argc, char **argv){
    // parse command line parameters
    server_config* sc = parse_args(argc, argv);
    if(!sc)
        goto exit;
    MTCP_SKELETON_INFO_MESSAGE("parsed command line arguments")

    // init mtcp
    int ret = mtcp_init(sc->conf_file);
    if(ret){
		MTCP_SKELETON_ERROR_MESSAGE("failed to init mtcp")
		goto free_server_config;
	}
    MTCP_SKELETON_INFO_MESSAGE("initialized mtcp stack")

    // add to global server config
    config = sc;

    // register signal handler to mtcp
	mtcp_register_signal(SIGINT, signal_handler);
    MTCP_SKELETON_INFO_MESSAGE("resgitered signal handler to mtcp")
    
    // create thread args array
    thread_arg **thread_args = create_thread_arg_array();
    if(!thread_args){
        MTCP_SKELETON_ERROR_MESSAGE("failed to create thread argument array")
        goto destroy_mtcp;
    }

    // create and join thread
    run_threads(epoll_server_thread, thread_args);
    
destroy_mtcp:
    mtcp_destroy();

free_server_config:
    free(sc);

exit:
    return 0;
}