/*
 * NAME, etc.
 *
 * sync.c
 *
 * Synchronization routines for SThread
 */

#define _REENTRANT

#include <stdlib.h>
#include "sthread.h"
#include "sync.h"

//#define STHREAD_DEBUG

#ifdef STHREAD_DEBUG
#include <stdio.h>
#define DEBUG(s) fprintf(stderr,"%s\n",s)
#define SILLY(args)	(stderr, args)
#define DEBUGF(args) fprintf SILLY(args)
#else
#define DEBUG(s)
#define DEBUGF
#endif

struct sthread_thread_queue {
	sthread_t thread;
	struct sthread_thread_queue *next;
};

#define _spin_lock(s) 	while (test_and_set( &((s)->spin_lock))) { ; /* spin */}
#define _spin_unlock(s) ( (s)->spin_lock = 0 )
#define _sem_init(v)	 	{ 						\
							.spin_lock = 0,			\
							.initial_semaphore = v,	\
							.semaphore = v,			\
							.queuehead = NULL, 		\
							.nextqtail = NULL,		\
							.destroyed = 0,			\
							}
/* not safe vs. function calls */
#define _sem_avail(s)	( (s)->semaphore > 0 && NULL == (s)->queuehead)
#define _sem_valid_check(s) if (s->destroyed) { _spin_unlock(s); return -1;}
#define _sem_lock_and_check(s)	_spin_lock(s); _sem_valid_check(s);


/* XXX need a block comment here */
sthread_queue_t *_make_queue_elem();


/*
 * Semaphore routines
 */

int sthread_sem_init(sthread_sem_t *sem, int count)
{
	if ( 0 >= count ) return -1; /* invalid count */
	sthread_sem_t tmp = _sem_init(count);
	DEBUG("In init_sem");
	*sem  = tmp;
	sem->nextqtail = &(sem->queuehead);
#ifdef STHREAD_DEBUG
	fprintf(stderr, "sem->head is %p\n", sem->queuehead);
	fprintf(stderr, "&sem->head is %p\n", &(sem->queuehead));
	fprintf(stderr, "sem->nextqtail is %p\n", sem->nextqtail);
	fprintf(stderr, "*(sem->nextqtail) is %p\n", *(sem->nextqtail));
#endif
	return 0;
	/*
		initialize semaphore to count
		remember init count
		initialize queue to empty
		set guard to 0
		return structure (no locks, no nuthin!)
	*/
}

int sthread_sem_destroy(sthread_sem_t *sem)
{
	_sem_lock_and_check(sem);
	sem->destroyed = 1;
	if ( sem->semaphore != sem->initial_semaphore ) {
		/* error condition */
		return -1;
	}
	_spin_unlock(sem); /* interesting point, this... */
	return 0;
	/*
		obtain spinlock
		check semaphore back to initial count, if not -> error
		check for waiters: if any
			return error (or just wake everybody?)
		else 
			destroy struct
			return 0
	*/
}

int sthread_sem_down(sthread_sem_t *sem)
{
	/* short-circuit attempt: try to get the semaphore without blocking */
	DEBUG("Entering full sem_down");
	if ( 0 == sthread_sem_try_down(sem) ) return 0;
	sthread_queue_t *q = _make_queue_elem();

	_sem_lock_and_check(sem);	/* BEGIN CRITICAL SECTION */
	if ( _sem_avail(sem) ) { /* well, won't we look silly */
		--sem->semaphore;
		_spin_unlock(sem);
		free(q);
		DEBUG("Semaphore freed up while allocating queue element");
		return 0;
	}
	DEBUG("Enqueueing self");
#ifdef STHREAD_DEBUG
	fprintf(stderr, "Queue stuff: q is %p \n", q);
	fprintf(stderr, "sem->head is %p\n", sem->queuehead);
	fprintf(stderr, "&sem->head is %p\n", &(sem->queuehead));
	fprintf(stderr, "sem->nextqtail is %p\n", sem->nextqtail);
	fprintf(stderr, "*(sem->nextqtail) is %p\n", *(sem->nextqtail));
#endif

	*(sem->nextqtail) = q;	/* enqueue self */
	sem->nextqtail = &(q->next);

#ifdef STHREAD_DEBUG
	fprintf(stderr, "Post-enqueue stuff: q is %p \n", q);
	fprintf(stderr, "sem->head is %p\n", sem->queuehead);
	fprintf(stderr, "&sem->head is %p\n", &(sem->queuehead));
	fprintf(stderr, "sem->nextqtail is %p\n", sem->nextqtail);
	fprintf(stderr, "*(sem->nextqtail) is %p\n", *(sem->nextqtail));

#endif


	for (;;) {
		_spin_unlock(sem);	/* EXIT CRIT */
		DEBUG("putting thread to sleep");
		sthread_suspend();	/* snooze */
		DEBUG("waiting thread woke up");
		_sem_lock_and_check(sem); /* REENTER */
		if ( 0 < sem->semaphore && q == sem->queuehead ) {
			/* we may proceed */
			break;
			DEBUG("Semaphore free: proceeding");
		} 
	}
	/* we hold the lock now, so just do the standard manipulations */
	--sem->semaphore;
	/* dequeue self */
	DEBUG("Dequeueing self");
	sem->queuehead = sem->queuehead->next; 
	if ( NULL == sem->queuehead ) {
		sem->nextqtail = &(sem->queuehead);
	} else { /* ( NULL != sem->queuehead ) */
		sthread_wake(sem->queuehead->thread);
	}
#ifdef STHREAD_DEBUG
	fprintf(stderr, "sem->head is %p\n", sem->queuehead);
	fprintf(stderr, "&sem->head is %p\n", &(sem->queuehead));
	fprintf(stderr, "sem->nextqtail is %p\n", sem->nextqtail);
	fprintf(stderr, "*(sem->nextqtail) is %p\n", *(sem->nextqtail));
#endif
	_spin_unlock(sem); /* EXIT */
	free(q);
	return 0;

	
	/* 
	before anything, call sthread_sem_try_down
		// 		pro: avoids potentially useless memory allocation
		//			 makes common case fast
		//		con: requires taking spin lock twice
	get spin_lock
	check semaphore level
	if available AND no waiters:
		decrement count
		release spin lock
		free unused queue element
		return
	else 
		increment waiting count
		enqueue self, using preallocated queue element
		release spinlock
		loop:
		yield // then get woken up
		obtain spinlock
		if not at head of queue
			release spinlock
			goto loop;
		else
			decrement waiting count
			dequeue self
			decrement count
			if queue not empty, wake next
				(it may need to snooze, but ... well, think about that)
			release spinlock
			free queue element
			return
	 */
}

int sthread_sem_try_down(sthread_sem_t *sem)
{
	DEBUG("Entering try_down");
	_sem_lock_and_check(sem);
	if ( _sem_avail(sem) ) {
		--sem->semaphore;
		_spin_unlock(sem);
		DEBUG("semaphore available!");
		return 0;
	} else {
		_spin_unlock(sem);
		DEBUG("semaphore unavailable.");
		return -1;
	}
	/*
		obtain spin lock
		check semaphore count
		if available AND no waiters
			decrement
			release spin lock
			return 0
		else 
			release spin lock
			return -1
	*/
}

int sthread_sem_up(sthread_sem_t *sem)
{
	DEBUG("trying to raise semaphore...");
	_sem_lock_and_check(sem);
	++sem->semaphore;
	if ( NULL != sem->queuehead ) {
		DEBUG("found waiters: waking first");
		sthread_wake(sem->queuehead->thread);
	} else {
		DEBUG("Found no waiters");
	}
	_spin_unlock(sem);
	DEBUG("lock released: leaving sem_up");
	return 0;
	/* obtain spin lock
		increment semaphore count
		if waiters
			wake first waiter
		release spin lock
		return
	*/

}

sthread_queue_t *_make_queue_elem() {
	sthread_t this = sthread_self();
	sthread_queue_t *q = malloc(sizeof(sthread_queue_t));
	if (NULL == q) return NULL; /* oh dear */
	q->next = NULL;
	q->thread = this;
	return q;
}
