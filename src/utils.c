#include <unistd.h>
#include <errno.h>
#include <numa.h>
#include <sched.h>
#include <sys/stat.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "mtcp_debug.h"

int get_cpu_num(){
	return sysconf(_SC_NPROCESSORS_ONLN);
}

int mtcpapp_strtol(const char *nptr, int base){
	int rval;
	char *endptr;

	errno = 0;
	rval = strtol(nptr, &endptr, 10);
	/* check for strtol errors */
	if ((errno == ERANGE && (rval == LONG_MAX ||
				 rval == LONG_MIN))
	    || (errno != 0 && rval == 0)) {
		perror("strtol");
		exit(EXIT_FAILURE);
	}
	if (endptr == nptr) {
		MTCPAPP_ERROR_MESSAGE("Parsing strtol error!");
		exit(EXIT_FAILURE);
	}

	return rval;
}