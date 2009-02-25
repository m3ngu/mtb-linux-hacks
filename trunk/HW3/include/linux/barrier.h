/* barrier.h
	Implementation of barrier synchronization primitive for Linux 2.6.11.12
	HEADER FILE
	2/25/2009
	Ben Warfield, Mengu Sukan, Thierry Bertin-Mahieux
*/



/**
 * Barrier structure
 */
struct barrier_struct {
  int bID;                   // barrier ID
  int spin_lock;             // spin lock
  int waiting_count;         // items in queue
  int initial_count;         // original N, or barrier size
  waitqueue_head_t *queue; /* needs some tightening up ;-) */  // queue
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
