#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * small C program that takes a PID and changes the
 * policy to UWRR. Should run that program as root
 */

int main (int argc, char *argv[]) {
	
	/* Check parameter */
	if (argc != 4) {
		printf("Usage: %s pid policy prio\n", argv[0]);
		return -EINVAL;
	}
	
	/* Do we need to check these ? */
	int pid    = atoi(argv[1]);
	int policy = atoi(argv[2]);
	int prio   = atoi(argv[3]);
	
	struct sched_param param = { .sched_priority = prio };
	int rc;
	
	/* SCHED_UWRR = 4 */
	rc = sched_setscheduler(pid, policy, &param);
	
	if (rc) {
		perror("Couldn't change scheduling policy");
	}
	
	return rc;
}
