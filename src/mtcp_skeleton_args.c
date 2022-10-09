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

// mtcp-skeleton header
#include <mtcp_skeleton_args.h>
#include <mtcp_skeleton_thread.h>
#include <mtcp_skeleton_debug.h>
#include <mtcp_skeleton_server_config.h>
#include <mtcp_skeleton_utils.h>

/*!
	\brief 	initialize server_config struct 
            (internal used by parse_args)
	\param	sc  the server_config struct
 */
static void _init_server_config(server_config *sc){
	sc->cpu_num = get_cpu_num();
	sc->cpu_limit = sc->cpu_num;
	sc->cpu_offset = 0;
    sc->backlog = 4096;
	sc->multi_threaded = false;
}

/*!
	\brief 	check whether config within server_config is valid 
            (internal used by parse_args)
	\param	sc  the server_config struct
	\return	bool instrutor to indicate whether the configuration is valid
 */
static bool _check_server_config(server_config *sc){
	// check cpu config
	if(sc->cpu_offset + sc->cpu_limit > sc->cpu_num && sc->multi_threaded){
		MTCP_SKELETON_ERROR_MESSAGE(
			"Invalid cpu configuration, \
			%d cores at all, given offset %d, given limit %d",
			sc->cpu_num, sc->cpu_offset, sc->cpu_limit
		)
		return false;
	}

	return true;
}

/*!
	\brief 	print helper message (internal used by parse_args)
 */
static void _print_help(){
	fprintf(stdout, "MTCP-APP Helper Information\n");
	fprintf(stdout, "-f           : path to the mtcp configuration file\n");
	fprintf(stdout, "-m [optional]: indicate to run under multi-threaded mode\n");
	fprintf(stdout, "-n [optional]: number of cores to run on, default to be all cpus\n");
	fprintf(stdout, "-c [optional]: cpu offset, start from which cpu core, default to be 0\n");
	fprintf(stdout, "-b [optional]: length of backlog, default to be 4096\n");
}

/*!
	\brief 	parse args
	\param	argc	number of args
	\param	argv	array of args
	\return	server configuration
 */
server_config* parse_args(int argc, char **argv){
	struct mtcp_conf mcfg;

    if (argc < 2) {
		MTCP_SKELETON_ERROR_MESSAGE("not enough arguments, use -h to check details")
		goto parse_args_exit;
	}

	server_config* sc = (server_config*)malloc(sizeof(server_config));
	if(!sc){
		MTCP_SKELETON_ERROR("failed to allocate memory for struct server_config")
		goto parse_args_exit;
	}

	// initailize server_config struct
	_init_server_config(sc);

	// parse parameters
	int o = 0;
	while (-1 != (o = getopt(argc, argv, "n:f:b:c:hm"))){
		switch(o){
			case 'n':	// setup cpu limit
			{
				int temp_cpu_limit = mtcp_skeleton_strtol(optarg, 10);
				if(temp_cpu_limit > sc->cpu_num){
					MTCP_SKELETON_ERROR_MESSAGE(
						"CPU limitation should less than actual number of cpu cores")
					MTCP_SKELETON_ERROR_MESSAGE(
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

			case 'c':	// setup cpu offset
			{
				int temp_cpu_offset = mtcp_skeleton_strtol(optarg, 10);
				if (temp_cpu_offset > sc->cpu_limit) {
					MTCP_SKELETON_ERROR_MESSAGE("starting CPU is way off limits");
					goto free_server_config;
				}
				sc->cpu_offset = temp_cpu_offset;
				break;
			}

			case 'b':	// setup backlog length
			{
				int temp_backlog = mtcp_skeleton_strtol(optarg, 10);
				mtcp_getconf(&mcfg);
				if(temp_backlog > mcfg.max_concurrency){
					MTCP_SKELETON_ERROR_MESSAGE(
						"backlog can not be set larger than CONFIG.max_concurrency")
					MTCP_SKELETON_ERROR_MESSAGE(
						"CONFIG.max_concurrency is %d, given backlog is %d",
						mcfg.max_concurrency, temp_backlog
					)
					goto free_server_config;
				}
				sc->backlog = temp_backlog;
				break;
			}

			case 'f':	// setup .conf file path
			{
				sc->conf_file = optarg;
				break;
			}

			case 'm':	// setup multi-threaded mode
			{
				sc->multi_threaded = true;
				break;
			}

			case 'h':	// print helper message
			{
				_print_help();
				goto free_server_config;
			}

			default:;
		}
	}

	// check whether overall configuration is valid
	if(!_check_server_config(sc))
		goto free_server_config;

	return sc;

free_server_config:
	free(sc);

parse_args_exit:
	return NULL;
}