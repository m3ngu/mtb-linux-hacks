#include <sched.h> 
#include <errno.h>
#include <time.h>

#define SECONDS_TO_RUN 30

int main(){
	
	// TODO: Now, I'm thinking that we should not set the scheduling
	// policy here.  Scheduling policy can only be changed by root, but
	// we want to call this program with alice, bob, carol, etc.
	
	// We should probably just kick this infitinite loop process off here
	// and have another small C program that takes a PID and changes the
	// policy to UWRR.  We can run that program as root
	
	/*
	 * Change the scheduling policy of this program
	 */
	/*
	struct sched_param param = { .sched_priority = 50 };
	int rc;
	// pid=0 current process
	rc = sched_setscheduler(0, SCHED_RR, &param);
	
	
	if (rc) {
		perror("Couldn't change scheduling policy");
	}
	*/
	
	//TODO: infinite loop, for now this runs for a minute
	clock_t curr_time, prev_time;
	prev_time = 0;
	short seconds_passed = 0;
	
	while(seconds_passed < SECONDS_TO_RUN) {
		curr_time = clock();
		if (curr_time - prev_time > CLOCKS_PER_SEC) {
			seconds_passed++;
			prev_time = curr_time;
		}
	}
	
	return 0;
}
