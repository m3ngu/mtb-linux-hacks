/*
 * NAME, etc.
 *
 * sync.h
 */

#ifndef _STHREAD_SYNC_H_
#define _STHREAD_SYNC_H_

typedef struct sthread_thread_queue sthread_queue_t;

struct sthread_sem_struct {
  unsigned long spin_lock;
  int initial_semaphore;
  int semaphore;
  sthread_queue_t *queuehead; /* needs some tightening up ;-) */
  sthread_queue_t **nextqtail;
  int destroyed;
};

typedef struct sthread_sem_struct sthread_sem_t;

int sthread_sem_init(sthread_sem_t *sem, int count);
int sthread_sem_destroy(sthread_sem_t *sem);
int sthread_sem_down(sthread_sem_t *sem);
int sthread_sem_try_down(sthread_sem_t *sem);
int sthread_sem_up(sthread_sem_t *sem);

#endif
