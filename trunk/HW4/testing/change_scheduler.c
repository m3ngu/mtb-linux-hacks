/*#include "linux/sched.h"  Causes a lot of warnings */
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * small C program that takes a PID and changes the
 * policy to UWRR. Should run that program as root
 */

#define DEF_PRIO	50  /* Needs to be between 0 and 99 */
#define SCHED_UWRR	4

int main (int argc, char *argv[]) {
	
	/* Check parameter */
	if (argc != 2) {
		printf("Usage: %s pid\n", argv[0]);
		return -EINVAL;
	}
	
	/* Do we need to check this ? */
	int pid = atoi(argv[1]);
	
	struct sched_param param = { .sched_priority = DEF_PRIO };
	int rc;
	
	/* SCHED_UWRR = 4 */
	rc = sched_setscheduler(pid, SCHED_UWRR, &param);
	
	if (rc) {
		perror("Couldn't change scheduling policy");
	}
	
	return rc;
}
