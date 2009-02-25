/*
 * NAME, etc.
 *
 * sync.h
 */

#ifndef _STHREAD_SYNC_H_
#define _STHREAD_SYNC_H_


struct sthread_sem_struct {
  int spin_lock;
  int waiting_count;
  int semaphore;
  void *queue; /* needs some tightening up ;-) */
};

typedef struct sthread_sem_struct sthread_sem_t;

int sthread_sem_init(sthread_sem_t *sem, int count);
int sthread_sem_destroy(sthread_sem_t *sem);
int sthread_sem_down(sthread_sem_t *sem);
int sthread_sem_try_down(sthread_sem_t *sem);
int sthread_sem_up(sthread_sem_t *sem);

#endif
