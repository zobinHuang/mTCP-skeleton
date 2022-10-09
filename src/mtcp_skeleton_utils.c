#include <unistd.h>
#include <errno.h>
#include <numa.h>
#include <sched.h>
#include <sys/stat.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

#include <mtcp_skeleton_thread.h>
#include <mtcp_skeleton_debug.h>
#include <mtcp_skeleton_utils.h>

int get_cpu_num(){
	return sysconf(_SC_NPROCESSORS_ONLN);
}

int mtcp_skeleton_strtol(const char *nptr, int base){
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
		MTCP_SKELETON_ERROR_MESSAGE("Parsing strtol error!");
		exit(EXIT_FAILURE);
	}

	return rval;
}