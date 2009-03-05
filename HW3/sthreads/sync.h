/*
 * NAME, etc.
 *
 * sync.h
 */

#ifndef _STHREAD_SYNC_H_
#define _STHREAD_SYNC_H_

typedef struct sthread_thread_queue sthread_queue_t; /* opaque to users */

struct sthread_sem_struct {
  unsigned long spin_lock;		/* spinlock for test_and_set */
  int initial_semaphore;		/* initial capacity */
  int semaphore;				/* current semaphore level */
  sthread_queue_t *queuehead;	/* head of wait queue */
  sthread_queue_t **nextqtail;	/* pointer to tail of wait queue */
  int destroyed;				/* has somebody called "destroy" on this? */
};

typedef struct sthread_sem_struct sthread_sem_t;

int sthread_sem_init(sthread_sem_t *sem, int count);
int sthread_sem_destroy(sthread_sem_t *sem);
int sthread_sem_down(sthread_sem_t *sem);
int sthread_sem_try_down(sthread_sem_t *sem);
int sthread_sem_up(sthread_sem_t *sem);

#endif
