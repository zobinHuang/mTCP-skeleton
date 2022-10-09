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
#include <mtcp_skeleton_debug.h>
#include <mtcp_skeleton_server_config.h>
#include <mtcp_skeleton_utils.h>
#include <mtcp_skeleton_thread.h>

server_config 	*config;
pthread_t		threads[THREAD_MAX_CPUS];
int         	thread_cores[THREAD_MAX_CPUS];
bool         	thread_dones[THREAD_MAX_CPUS];

/*!
	\brief 	signal handler
	\param	argc	index of signal
 */
void signal_handler(int sig_num){
	for(int i=0; i<config->cpu_limit; i++){
		if (threads[i] == pthread_self()) {
            MTCP_SKELETON_INFO_MESSAGE("server thread %d got SIGINT\n", i)
			thread_dones[i] = true;
		} else {
			if (!thread_dones[i])
				pthread_kill(threads[i], sig_num);
		}
	}
}

thread_arg** create_thread_arg_array(){
	int num_threads = 1;
	if(config->multi_threaded)
		num_threads = config->cpu_limit;

	// create array
	thread_arg **arg_array = (thread_arg**)malloc(sizeof(thread_arg*)*num_threads);
	if(!arg_array)
		return NULL;
	
	for(int i=0; i<num_threads; i++){
		thread_arg *arg = (thread_arg*)malloc(sizeof(thread_arg));
		if(!arg)
			return NULL;
		arg->number = 0;
		arg_array[i] = arg;
	}

	return arg_array;
}

/*!
	\brief 	launch and join thread(s) based on server configuration
	\param	thread_func			function of thread(s) to execute
	\param	thread_args_array	array that loads pointer(s) to 
								thread_args_array for each thread
 */
void run_threads(void *thread_func, thread_arg** thread_args_array){
	if(config->multi_threaded){
		int core = -1;
		int offset = config->cpu_offset;

		// create threads
		for(int i=0; i<config->cpu_limit; i++){
			core = i+offset;
			thread_cores[core] = core;
			thread_dones[core] = false;
			
			// append core number to the last argument
			thread_args_array[i]->args[thread_args_array[i]->number] 
				= (void*)&thread_cores[core];

			if(pthread_create(
					&threads[core],
					NULL,
					thread_func,
					thread_args_array ? (void*)thread_args_array[i] : NULL)
			){
				MTCP_SKELETON_ERROR_MESSAGE("failed to launch thread on core %d, exit", core)
				exit(EXIT_FAILURE);
			}
		}

		// join threads
		for(int i=0; i<config->cpu_limit; i++){
			core = i+offset;
			pthread_join(threads[core], NULL);

			// TODO: deal with thread return value
		}
	
	} else { // single-threaded
		thread_cores[0] = 0;
		thread_dones[0] = false;

		// append core number to the last argument
		thread_args_array[0]->args[thread_args_array[0]->number] 
			= (void*)&thread_cores[0];

		if(pthread_create(
				&threads[0],
				NULL,
				thread_func,
				thread_args_array ? (void*)thread_args_array[0] : NULL)
		){
			MTCP_SKELETON_ERROR_MESSAGE("failed to launch thread on core 0, exit")
			exit(EXIT_FAILURE);
		}

		pthread_join(threads[0], NULL);

		// TODO: deal with thread return value
	}

	
}