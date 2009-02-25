/*
 * NAME, etc.
 *
 * sync.c
 *
 * Synchronization routines for SThread
 */

#define _REENTRANT

#include "sthread.h"


/*
 * Semaphore routines
 */

int sthread_sem_init(sthread_sem_t *sem, int count)
{
	/*
		allocate structure
		initialize semaphore to count
		initialize queue to empty
		set guard to 0
		return structure (no locks, no nuthin!)
	*/
}

int sthread_sem_destroy(sthread_sem_t *sem)
{
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
	/* 
	before anything, allocated a queue element (may sleep)
		// alternate plan: try sthread_sem_try_down first
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
	if unavailable
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
			release spinlock
			free queue element
			return
	 */
}

int sthread_sem_try_down(sthread_sem_t *sem)
{
	/*
		obtain spin lock
		check semaphore count
		if available AND no waiters
			decrement
			release spin lock
			return 0
		else 
			return -1
	*/
}

int sthread_sem_up(sthread_sem_t *sem)
{
	/* obtain spin lock
		increment semaphore count
		if waiters
			wake first waiter
		release spin lock
		return
	*/

}


