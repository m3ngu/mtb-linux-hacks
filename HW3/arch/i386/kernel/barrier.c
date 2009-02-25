/* barrier.c
	Implementation of barrier synchronization primitive for Linux 2.6.11.12
	2/25/2009
	Ben Warfield, Mengu Sukan, Thierry Bertin-Mahieux
*/


/**
 * Creates a barrier
 */
int barriercreate(int num)
{
  /*
    create a new barrier struct
    set the initial_count = num
    set wait_count = 0
    make sure queue* is null
    add the barrier to the barrier list...?
    set the id
    returns the ID (not the slot, we could have destroyed barriers)
  */
  return -666666666
}

/**
 * Destroys a barrier
 */
int barrierdestroy(int barrierID)
{
  /*
    find the barrier with ID, make sure it exists
    checks if people in queue 
         we handle it, wake up!!!!!
	 return error
    destroy the struct, fix the barrier list
    returns.... what? 0 or -1?
  */
  return -77777777;
}

/**
 * Wait on the barrier, or release everyon if you're the Nth one
 */
int barrierwait(int barrierID)
{
  /*
    find the barrier with the proper ID (error if doesn't exists)
    waiting_count++
    if waiting_count == initial_count
       wake up every one
       clear queue
       waiting_count = 0
       go
    else
       get in queue
       wait
  */
  return -888888888;
}


/**
 * helper function, gets a barrier with a given ID
 * uses a spinlock
 */
int get_barrier(struct barrier_struct* b, int barrierID)
{
  /*
    go through the list, checks ID, return pointer
    if not found, error
  */
  return -999999999;
}

