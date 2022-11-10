#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/queue.h>
#include <assert.h>
#include <limits.h>

#include <mtcp_api.h>
#include <mtcp_epoll.h>
#include <libmemcached/memcached.h>

int main(int argc, char **argv){
    // init mtcp
    const int core_limit = 1;
    static char* conf_file = "../mtcp_conf/server.conf";

    struct mtcp_conf mcfg;
    mtcp_getconf(&mcfg);
    mcfg.num_cores = core_limit;
    mtcp_setconf(&mcfg);

    int res = mtcp_init(conf_file);
    if (res) {
        fprintf(stderr, "failed to init mtcp");
        exit(1);
    }

    const char *config_string= "--SERVER=10.0.109.2:11103";
    memcached_st *memc= memcached(config_string, strlen(config_string));
    fprintf(stdout, "number of servers: %u\n", memc->number_of_hosts);
    
    char *key= "foo";
    char *value= "value";
    memcached_return_t rc= memcached_set(memc, key, strlen(key), value, strlen(value), (time_t)0, (uint32_t)0);
    if (rc != MEMCACHED_SUCCESS){
        fprintf(stdout, "failed to set key-value pair\n");
    }
    
    memcached_free(memc);

    return 0;
}