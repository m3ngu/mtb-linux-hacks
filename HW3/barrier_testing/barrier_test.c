

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
	int id = (int)arg;
	printf("dummy function num. %i created\n",id);
	// assumes that barrier 2 exists
	printf("barrierwait output=%i\n",barrierwait(2));
	printf("dummy function num. %i terminates\n",id);
	return 0;
}


int main() {

	puts("[1b]: Calling barrierdestroy with id=-1");	
	int ret = barrierdestroy(-1);
	perror("[1b]");
	
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

	//sthread_wake(t1);
	//sthread_wake(t2);
	//sthread_wake(t3);

	/*
	puts("[1a]: Calling barriercreare with num=0");	
	int barrier1 = barriercreate(0);
	perror("[1a]");
	

	int barrier2 = barriercreate(10);
	int barrier3 = barriercreate(20);
	int barrier4 = barriercreate(30);
	
	ret = barrierdestroy(barrier3);
	ret = barrierdestroy(barrier2);
	ret = barrierdestroy(barrier4);
	
	fputs("Waiting for single-process barrier...", stderr);
	if ( barrierwait(barrier1) ) perror("inexplicable failure")
	else fputs("success!\n", stderr);
	*/
	return EXIT_SUCCESS;
}
