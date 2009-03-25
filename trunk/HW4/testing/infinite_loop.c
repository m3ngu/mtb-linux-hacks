#include <sched.h> 
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_SECONDS 9999

int main (int argc, char *argv[]) {

	int policy;
	policy = sched_getscheduler(0);
	clock_t curr_time, prev_time;

	prev_time = 0;
	unsigned short seconds_passed = 0, max_seconds = MAX_SECONDS;
	
	if (argc == 2) {
		max_seconds = atoi(argv[1]);
	}
	
	while(seconds_passed < max_seconds) {
		int new_policy = sched_getscheduler(0);
		if ( new_policy != policy ) {
			printf("PID %d: Policy changed from %d to %d\n", getpid(), policy, new_policy);
			policy = new_policy;
		}
		curr_time = clock();
		if (curr_time - prev_time > CLOCKS_PER_SEC) {
			seconds_passed++;
			prev_time = curr_time;
		}
	}
	
	return 0;
}
