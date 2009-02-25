/* barrier.c
	Implementation of barrier synchronization primitive for Linux 2.6.11.12
	2/25/2009
	Ben Warfield, Mengu Sukan, Thierry Bertin-Mahieux
*/
/* necessary globals: */

static unsigned int next_id = 1;
/* optionally: */
static unsigned int last_destroyed = 0;

/* this is the wrong type: if possible, we should just piggy-back
	on some existing linked-list kernel structure.
*/
static void *barrier_list_head;


/**
 * Creates a barrier
 */
int sys_barriercreate(int num)
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
int sys_barrierdestroy(int barrierID)
{
  /*
    find the barrier with ID, make sure it exists
    lock barrier
    set barrier.destroyed
    checks if people in queue 
        wake them up
    otherwise:
    	destroy the struct, fix the barrier list
    unlock the barrier
    return number of awoken processes
  */
  return -77777777;
}

/**
 * Wait on the barrier, or release everyone if you're the Nth one
 */
int sys_barrierwait(int barrierID)
{
  /*
  	set return value to 0
    find the barrier with the proper ID (error if doesn't exists)
    	also error if it's been destroyed but not cleaned up
    lock barrier
	waiting_count++
	if waiting_count == initial_count
	   wake up every one
	   clear queue
	   waiting_count = 0
	   release lock
	   go
    else
       get in queue
       release lock
       wait
       if barrier is marked destroyed
       		set return value to -1
       		lock barrier 
       		decrement waiting_count
       		if waiting count is now 0, destroy/clean up the barrier
       		unlock barrier

    return the return value
  */
  return -888888888;
}


/**
 * helper function, gets a barrier with a given ID
 * uses a spinlock
 */
int _get_barrier(struct barrier_struct* b, int barrierID)
{
  /*
    go through the list, checks ID, return pointer
    if not found, error
  */
  return -999999999;
}

unsigned int _next_id(void) {
	/*
		start with current value of next_id
		check if it's in use
		if so, increment and try again until you find one that isn't
		inefficient as hell, but honestly shouldn't ever be an issue
	*/
}
