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
    returns the ID
  */
  return -666666666
}

/**
 * Destroys a barrier
 */
int barrierdestroy(int barrierID)
{
  /*
    find the 
  */
}

/**
 * Wait on the barrier, or release everyon if you're the Nth one
 */
int barrierwait(int barrierID);



