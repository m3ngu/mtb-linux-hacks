/* barrier.c
	Implementation of barrier synchronization primitive for Linux 2.6.11.12
	2/25/2009
	Ben Warfield, Mengu Sukan, Thierry Bertin-Mahieux
*/


#include <linux/list.h>
#include <linux/unistd.h> // do we need this one
#include <linux/linkage.h> // asmlinkage
#include <linux/errno.h>
#include <linux/barrier.h>
#include <linux/sched.h>


/* necessary globals: */
static unsigned int next_id = 1;
/* optionally: */
static unsigned int last_destroyed = 0;

/* this is the wrong type: if possible, we should just piggy-back
	on some existing linked-list kernel structure.
*/
//static void *barrier_list_head;


/**
 * Right implementation using linux/list.h
 */
// list node
struct barrier_node {
  struct list_head list; //linux kernel list implementation//
  struct barrier_struct *barrier;
};
// actual list
static struct barrier_node *barrier_list;




/**
 * Creates a barrier
 */
asmlinkage int sys_barriercreate(int num)
{
  // check input
  if(num < 1) {return -EINVAL;}
  // create barrier, allocate memory
  struct barrier_struct *b;
  b = (struct barrier_struct *)kmalloc(sizeof(struct barrier_struct),GFP_KERNEL);
  if (NULL == b) {return -ENOMEM;}
  // init fields
  (*b).initial_count = num;
  (*b).waiting_count = 0;
  //(*b).queue = NULL;
  // add barrier to the list
  _add_barrier_node(b);
  // checks errors....
  (*b).bID = next_id;
  next_id++; // better id management later...
  return (*b).bID;
}

/**
 * Destroys a barrier
 */
asmlinkage int sys_barrierdestroy(int barrierID)
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
asmlinkage int sys_barrierwait(int barrierID)
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
  struct barrier_node *tmp;
  struct list_head *pos;
  list_for_each( pos, &((*barrier_list).list) ){
    tmp = list_entry(pos, struct barrier_node, list);
    if ((*(*tmp).barrier).bID == barrierID)
      {
	b = (*tmp).barrier;
	return 0;
      }
  }
  // we should set a particular ERROR here!!!!!!!
  return -1;
}


/**
 * Add a barrier node to the barrier list, don't check for any problems
 * cool tutorial: http://isis.poly.edu/kulesh/stuff/src/klist/
 */
int _add_barrier_node(struct barrier_struct* b)
{
  struct barrier_node *tmp;
  // makes sure the list is initialized
  if (barrier_list == NULL)
    INIT_LIST_HEAD( &((*barrier_list).list) );
  tmp= (struct barrier_node *)kmalloc(sizeof(struct barrier_node),GFP_KERNEL);
  // should check for kmalloc error here!!!!!!!!!
  // copy b to tmp... deep or shallow copy? shallow
  (*tmp).barrier = b;
  // add tmp node to the global list
  list_add( &((*tmp).list), &((*barrier_list).list) );
  // if we're here, everything should have worked
  return 0;
}

unsigned int _next_id(void) {
	/*
		start with current value of next_id
		check if it's in use
		if so, increment and try again until you find one that isn't
		inefficient as hell, but honestly shouldn't ever be an issue
	*/
}
