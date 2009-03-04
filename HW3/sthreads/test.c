#define _REENTRANT
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "sthread.h"
#include "sync.h"

struct sem_thread_info {
	int thread_id;
	sthread_sem_t *sem;
};

int threadmain(void *arg) {
  int threadno = (int)arg;
  for (;;) {
    printf("thread %d: I'm going to sleep\n", threadno);
    sthread_suspend();
    printf("thread %d: I woke up!\n", threadno);
  }
  return 0;
}

int threadsem(void *arg) {
	struct sem_thread_info *info = (struct sem_thread_info *) arg;
	int tid = info->thread_id;
	sthread_sem_t *sem =  info->sem;
	if ( sthread_sem_down(sem) ) fputs("wait, is that supposed to happen?", stderr);
	fprintf(stderr, "Thread %d got the semaphore!\n", tid);
	sleep(2);
	fprintf(stderr, "Thread %d is returning the semaphore\n", tid);
	sthread_sem_up(sem);
	return 0;
}

int main(int argc, char *argv[]) {
	sthread_t thr1, thr2, thr3, thr4, thrA, thrB;
	sthread_sem_t sem1;
	struct sem_thread_info 
		info1 = { .thread_id = 1, .sem = &sem1 },
		info2 = { .thread_id = 2, .sem = &sem1 },
		info3 = { .thread_id = 3, .sem = &sem1 },
		info4 = { .thread_id = 4, .sem = &sem1 };
	if (sthread_init() == -1)
		fprintf(stderr, "%s: sthread_init: %s\n", argv[0], strerror(errno));
	
	puts("*** basic thread tests ***");

	if (sthread_create(&thrA, threadmain, (void *)1) == -1)
	fprintf(stderr, "%s: sthread_create: %s\n", argv[0], strerror(errno));
	
	if (sthread_create(&thrB, threadmain, (void *)2) == -1)
	fprintf(stderr, "%s: sthread_create: %s\n", argv[0], strerror(errno));
	
	sleep(1);
	sthread_wake(thrA);
	sleep(1);
	sthread_wake(thrB);
	sleep(1);
	sthread_wake(thrA);
	sthread_wake(thrB);
	sleep(1);

	puts("*** semaphore tests: one semaphore of size 2, 4 threads ***");
	sthread_sem_init(&sem1,2);
  	if (sthread_create(&thr1, threadsem, (void *) &info1) == -1)
		fprintf(stderr, "%s: sthread_create: %s\n", argv[0], strerror(errno)); 
	sleep(1);
	if (sthread_create(&thr2, threadsem, (void *) &info2) == -1)
		fprintf(stderr, "%s: sthread_create: %s\n", argv[0], strerror(errno));  
	if (sthread_create(&thr3, threadsem, (void *) &info3) == -1)
		fprintf(stderr, "%s: sthread_create: %s\n", argv[0], strerror(errno));
    if (sthread_create(&thr4, threadsem, (void *) &info4) == -1)
		fprintf(stderr, "%s: sthread_create: %s\n", argv[0], strerror(errno));

	sleep(10);
	return 0;
}
