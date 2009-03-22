/*#include "linux/sched.h"  Causes a lot of warnings */
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * small C program that takes a PID and changes the
 * policy back to normal. Should run that program as root
 */

int main (int argc, char *argv[]) {
	
	/* Check parameter */
	if (argc != 2) {
		printf("Usage: %s pid\n", argv[0]);
		return -EINVAL;
	}
	
	/* Do we need to check this ? */
	int pid = atoi(argv[1]);
	
	struct sched_param param = { .sched_priority = 0 };
	int rc;
	
	/* SCHED_UWRR = 4 */
	rc = sched_setscheduler(pid, 0, &param);
	
	if (rc) {
		perror("Couldn't change scheduling policy");
	}
	
	return rc;
}
