

/* barrier_test.c: various test/demonstration code for kernel barriers
	2/25/09
*/

#define _REENTRANT
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../sthreads/sthread.h"
#include "../sthreads/sync.h"
#include "include/asm/unistd.h"

_syscall1(int, barriercreate, 	int, num);
_syscall1(int, barrierdestroy, 	int, id);
_syscall1(int, barrierwait, 	int, id);


int dummyfunction(void *arg) {
  int id = (int) arg;
  int pid = fork();
  if (pid != 0)
    return 0;
  else {
    printf("dummy function num. %i created\n",id);
    // assumes that barrier 2 exists
    printf("barrierwait output=%i\n",barrierwait(2));
    printf("dummy function num. %i terminates\n",id);
    return 0;
  }
  return 4321;
}


int main() {

        sthread_t t1, t2, t3;
	int barrier1;

	// makes sure barrier 2 created
	barrier1 = barriercreate(3);
	barrier1 = barriercreate(3);
	barrier1 = barriercreate(3);
	
	// create threads
	if (sthread_init() == -1)
	  {
	    fprintf(stderr,"sthread_init(): %s\n",strerror(errno));
	    return -1;
	  }
	if (sthread_create(&t1,dummyfunction,(void*)1) == -1)
	  {  
	    fprintf(stderr,"thread 1 could not be created\n");
	    return -1;
	  }
	if (sthread_create(&t2,dummyfunction,(void*)2) == -1)
	  {  
	    fprintf(stderr,"thread 2 could not be created\n");
	    return -1;
	  }
	if (sthread_create(&t3,dummyfunction,(void*)3) == -1)
	  {  
	    fprintf(stderr,"thread 3 could not be created\n");
	    return -1;
	  }

	sthread_wake(t1);
	sthread_wake(t2);
	sthread_wake(t3);

	/*
	puts("[1a]: Calling barriercreare with num=0");	
	int barrier1 = barriercreate(0);
	perror("[1a]");
	*/

	/*
	fputs("Waiting for single-process barrier...", stderr);
	if ( barrierwait(barrier1) ) perror("inexplicable failure")
	else fputs("success!\n", stderr);
	*/
	return EXIT_SUCCESS;
}
