

/* barrier_test.c: various test/demonstration code for kernel barriers
	2/25/09
*/
#define _REENTRANT
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include "asm/unistd.h"

_syscall1(int, barriercreate, 	int, num)
_syscall1(int, barrierdestroy, 	int, id)
_syscall1(int, barrierwait, 	int, id)

#define _spawn_all(ar, func, arg)  \
	for (int i = 0; i < sizeof(ar)/sizeof(int); i++) { \
		ar[i] = magic_fork(func, arg); \
	}

#define _wait_all(ar) \
	for (int i = 0; i < sizeof(ar)/sizeof(int); i++) { 	\
		int status; 									\
		waitpid(ar[i], &status, 0);						\
	}

	
	
/* this prints the process ID, and prints to stdout, so the interleaving is happier */

void better_perror(const char *s) {
/*
	char *errmsg = strerror(errno);
	int pid;
	puts("still alive...");
	pid = getpid();
	printf("In PID %d; %s: %s\n", pid, s, errmsg);// strerror(tmp));
	*/
	perror(s);
}

/**
 * The children calls the func(void*)
 */
int magic_fork( void (*func) (void* ), void* arg )
{
  int pid = fork();
  if (pid == 0) // child
    {
      func(arg);
      exit(0);
    }
  else return pid;
}

#define BSIZE1  4 



/**
 * Tests 1 tasks with barrier of size 1
 */
void test1() {


}

/**
 * Tests M tasks with barrier of size N, N < M
 * Then test destroy with tasks waiting
 */
 
void test2_helper(void *arg) {
	int barrier_id = (int) arg;
	int mypid = getpid();
	printf("[2b] child %d is approaching the barrier\n", mypid);
	int status = barrierwait(barrier_id);
	if ( -1 == status) {
		printf("[2c] child %d left barrier with error\n", mypid);
	} else if (0 == status) {
		printf("[2b] child %d left barrier normally\n", mypid);
	}
}
 
void test2() {
	int kiddies[5];
	puts("[2a] creating a barrier of size 3");
	int barrier_id = barriercreate(3);
	if (barrier_id > 0) {
		printf("[2b] launching 5 processes at a size-3 barrier\n");
		_spawn_all(kiddies, test2_helper, (void *) barrier_id);
		sleep(1);
		printf("[2c] destroying barrier with (hopefully) 2 processes at it\n");
		int left = barrierdestroy(barrier_id);
		if (2 == left) {
			printf("[2c] got back correct number of waiting processes\n");
		} else if ( -1 == left) {
			perror("[2c] unexpected failure");
		} else {
			printf("[2c] got back %d instead of 2\n", left);
		}
		_wait_all(kiddies);
		printf("[2b] reaped child processes\n");
	} else {
		perror("[2a] barrier creation failed");
	}
}

/**
 * Tests when we try to open multiple barriers simultaneously
 */
void test3() {


}


/**
 * Tests N tasks with barrier of size N
 */
void test4() {


}


// task counter
static int test5_counter = 1;
// helper function for test 5
void helper5(void* arg) {
  int id = test5_counter; test5_counter++;
  int bID = (int) arg;
  printf("task %i get in queue with barrier %i\n",id,bID);
  barrierwait(bID);
  printf("task %i left barrier %i\n",id,bID);
}


/**
 * Tests N tasks with barrier of size N+1, wake everybody,
 * they stay in queue, then we launch the N+1 task
 */
void test5() {
  int i, status;
  printf("[5a]: create a barrier of size 3, bid=");
  int bID = barriercreate(3); printf("%i\n",bID);
  int t1 = magic_fork( helper5, (void*) bID );
  int t2 = magic_fork( helper5, (void*) bID );
  printf("[5b]: send wake signals to the two processes\n");
  sleep(1);
  kill( getppid() ,SIGCONT);
  kill( getppid() ,SIGCONT);
  sleep(1);
  printf("[5c]: send a third task to the barrier\n");
  int t3 = magic_fork( helper5, (void*) bID );
  for (i = 0; i < 3; i++)
    wait(&status);
  printf("[5d]: test done\n");
}




int main() {

  test5(); exit(0);

	puts("[1b]: Calling barrierdestroy with id=-1");	
	int ret = barrierdestroy(-1);
	if (0 > ret) perror("Error in global destroy");
	else printf("Destroyed barriers had %d waiting processes\n", ret);
	test2();
	perror("[1b]");

	int barrier1 = barriercreate(1);
	printf("barrier 1 returns: %i\n",barrier1);
	int status = 0;
	
	printf("[2a]: approaching barrier %d (size 1)\n", barrier1);
	status = barrierwait(barrier1);
	
	if (status) better_perror("[2a] Error waiting for barrier 1");
	else puts("[2a] Successfully waited for nobody");
	if (1) {
		status = barrierdestroy(barrier1);
		printf("Survived with status %d\n", status);
		if (status) better_perror("[2b] failed to destroy barrier 1");
		else puts("[2b] destroyed barrier 1 correctly");
	} else {
		puts("Skipping barrier destruction");
	}
	
	/* 2c: barrier of four, fork off three, wait a second, and go */
	barrier1 = barriercreate(BSIZE1);
	if ( 0 > barrier1 ) { better_perror("[2c] failed to create barrier"); }
	else {
		printf("[2a]: created barrier %d (size %d)\n", barrier1, BSIZE1);

		int childpids[BSIZE1 - 1];
		for (int i = 0; i < BSIZE1 - 1; i++) {
			int pid = fork();
			if ( 0 > pid) { better_perror("[2c] FORK FAILED"); }
			else if (0 != pid) {
				printf("[2c] child %d is now waiting for the barrier\n", pid);
				status = barrierwait(barrier1);
				if (status) {
					better_perror("unexpected error on wait");
				} else {
					printf("[2c] child %d woke up, will re-queue\n", pid);
				}
				status = barrierwait(barrier1);
				if (status) {
					better_perror("unexpected error on wait");
				} else {
					printf("[2c] child %d woke up and will now exit\n", pid);				
				}
				exit(0);
			} else {
				childpids[i] = pid; /* store for reaping later */
			}
		}
		printf("[2c]: parent will wait a second, then go to the barrier\n");
		sleep(1);
		status = barrierwait(barrier1);
		if (status) {better_perror("parent found error on wait"); }
		else { puts("[2c] parent re-queueing"); }
		status = barrierwait(barrier1);
		if (status) {better_perror("parent found error on 2nd wait"); }
		else { puts("[2c] parent left barrier, reaping children"); }
		for (int i = 0; i < BSIZE1 - 1; i++) {
			int childstatus;
			waitpid(childpids[i], &childstatus, 0);
		}
		puts("[2c] children reaped");
		status = barrierdestroy(barrier1);
		if (status) {puts("[2d] didn't expect that..."); }
		else {  puts("Destroyed barrier cleanly"); }
		status = barrierdestroy(barrier1);
		if (status) { 
			perror("[2e] double-destroy of barrier");
		}
		else puts("[2e] unexpected success in double-destruction");
	}
	return 0;
}
