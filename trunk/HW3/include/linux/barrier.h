/* barrier.h
	Implementation of barrier synchronization primitive for Linux 2.6.11.12
	HEADER FILE
	2/25/2009
	Ben Warfield, Mengu Sukan, Thierry Bertin-Mahieux
*/


#include <linux/wait.h>


/**
 * Barrier structure
 */
struct barrier_struct {
  unsigned int bID;                  // barrier ID
  spinlock_t spin_lock;		// spin lock
  int waiting_count;		// items in queue
  int initial_count;		// original N, or barrier size
  wait_queue_head_t queue;	// queue
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
