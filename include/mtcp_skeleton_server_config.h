#ifndef _MTCP_SKELETON_SERVER_CONFIG_H_
#define _MTCP_SKELETON_SERVER_CONFIG_H_

typedef struct server_config{
    int     cpu_num;
    int     cpu_limit;
    int     cpu_offset;
    int     backlog;
    bool    multi_threaded;
    char    *conf_file;
} server_config;

#endif