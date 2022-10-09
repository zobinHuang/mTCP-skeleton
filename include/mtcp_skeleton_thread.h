#ifndef _THREAD_H_
#define _THREAD_H_

#include <stdbool.h>
#include <mtcp_skeleton_server_config.h>

// thread config
#define THREAD_MAX_CPUS     16
#define THREAD_MAX_ARG      32

typedef struct thread_arg{
    int     number;
    void*   args[THREAD_MAX_ARG];
}thread_arg;

extern server_config   	*config;
extern pthread_t		threads[THREAD_MAX_CPUS];
extern int             	thread_cores[THREAD_MAX_CPUS];
extern bool         	thread_dones[THREAD_MAX_CPUS];

void signal_handler(int sig_num);
thread_arg** create_thread_arg_array();
void run_threads(void *thread_func, thread_arg** thread_args_array);

#endif