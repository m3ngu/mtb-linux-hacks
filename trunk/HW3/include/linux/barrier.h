ema/* barrier.h
	Implementation of barrier synchronization primitive for Linux 2.6.11.12
	HEADER FILE
	2/25/2009
	Ben Warfield, Mengu Sukan, Thierry Bertin-Mahieux
*/


#include <linux/wait.h>
#include <linux/init.h> // if we need to init the barrier_list... or in the .c?


/**
 * Barrier structure
 */
struct barrier_struct {
  unsigned int bID;                  // barrier ID
  spinlock_t spin_lock;		// spin lock
  int waiting_count;		// items in queue
  int initial_count;		// original N, or barrier size
  wait_queue_head_t *queue;	// queue
  int destroyed;			// set to 1 if we're in the destruction function
};



/**
 * Creates a barrier
 */
int barriercreate(int num);

/**
 * Destroys a barrier
 */
int barrierdestroy(int barrierID);

/**
 * Wait on the barrier, or release everyon if you're the Nth one
 */
int barrierwait(int barrierID);


int _get_barrier(struct barrier_struct* b, int barrierID);


int _add_barrier_node(struct barrier_struct* b);


unsigned int _next_id(void);
