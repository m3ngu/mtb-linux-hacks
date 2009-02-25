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
  int spin_lock;             // spin lock
  int waiting_count;         // items in queue
  int initial_count;         // original N, or barrier size
  void *queue; /* needs some tightening up ;-) */  // queue
};
