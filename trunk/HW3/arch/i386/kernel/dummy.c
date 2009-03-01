




/* barrier.c
	Implementation of barrier synchronization primitive for Linux 2.6.11.12
	2/25/2009
	Ben Warfield, Mengu Sukan, Thierry Bertin-Mahieux
*/


#include <linux/list.h>
#include <linux/unistd.h> // do we need this one
#include <linux/linkage.h> // asmlinkage
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/barrier.h>
#include <linux/spinlock.h>



/* necessary globals: */
static unsigned int next_id = 1;
/* optionally: */
static unsigned int last_destroyed = 0;


/**
 * Right implementation using linux/list.h
 */
// list node
struct barrier_node {
  struct list_head list; //linux kernel list implementation//
  struct barrier_struct *barrier;
};
// actual list
//static struct barrier_node *barrier_list = NULL;
static struct list_head *barrier_list = NULL;

/**
 * DEBUG!!!!!
 */
static struct barrier_struct *_testb = NULL;


/**
 * Creates a barrier
 */
asmlinkage int sys_barriercreate(int num)
{
  int err = 0;
  struct barrier_struct *b = NULL;
  /*
   * DEBUG!
   */
  create_test_barrier(num);
  return 0;
  // check input
  if(num < 1) {return -EINVAL;}
  // create barrier, allocate memory
  b = (struct barrier_struct *)kmalloc(sizeof(struct barrier_struct),GFP_KERNEL);
  if (NULL == b) {return -ENOMEM;}
  // init fields and spin_lock
  b->initial_count = num;
  b->waiting_count = 0;
  b->destroyed = 0;
  spin_lock_init( &(b->spin_lock) );
  // init wait queue head
  b->queue = (wait_queue_head_t *)kmalloc(sizeof(wait_queue_head_t),GFP_KERNEL);
  if (b->queue == NULL) {return -ENOMEM;}
  init_waitqueue_head( b->queue );
  // CHECKS ERROR
  // add barrier to the list
  //err = _add_barrier_node(b);
  if (err < 0)
    {
      printk(KERN_INFO "could not add barrier to list, return -1\n");
      return -1;
    }
  // set ID, returns it
  b->bID = next_id;
  next_id++; // better id management later...
  printk(KERN_INFO "Created a barrier with ID=%i\n",b->bID);
  printk(KERN_INFO "next id is now %i\n",next_id);
  return b->bID;
}

/**
 * Destroys a barrier
 * Returns -num of awoken processes (it's considered an error)
 */
asmlinkage int sys_barrierdestroy(int barrierID)
{
  struct barrier_struct *b;
  struct barrier_node *bn;
  int return_value = 0;
  // check ID validity
  if (barrierID < 0) {return -EINVAL;}
  // find the barrier node and barrier
  bn = _get_barrier_node( barrierID );
  if (bn == NULL)
    return return_value;
  b = bn->barrier;
  // lock barrier
  spin_lock( &(b->spin_lock) );
  // set destroyed mode
  b->destroyed = 1;
  // if people in queue, wake them up, and count
  if (b->waiting_count > 0)
    {        
      // wake up everyone
      wake_up_all(b->queue);
      // remember - number of awoken processes
      return_value = - b->waiting_count;
    }
  // destroy the struct, can we sleep here?
  kfree(b);
  // remove and delete node
  list_del( &(bn->list) );
  kfree(bn);
  /*
   * What happens if the list is now empty???? pointer to NULL?
   */
  return return_value;
}

/**
 * Wait on the barrier, or release everyone if you're the Nth one
 * cool tutorial: http://lwn.net/Articles/22913/
 */
asmlinkage int sys_barrierwait(int barrierID)
{
  return barrierwait_test();
/*  int return_value = 0;
  struct barrier_struct *b;
  // check ID validity
  if (barrierID < 0) {return -EINVAL;}
  // find the barrier
  //b = _get_barrier( barrierID );
  //if (b == NULL)
  //  return -1;
*/  /* 
     check if it exist but was destroyed, error !!!!!!!!
  */
/*  // lock barrier
  spin_lock( &(b->spin_lock) );
  // update the counter of people waiting
  b->waiting_count++;
  if (b->waiting_count == b->initial_count) // all processes are here
    {
      // wake up everyone
      wake_up_all(b->queue);
      // reset waiting_count
      b->waiting_count = 0;
      // release lock
      spin_unlock( &(b->spin_lock) );
      // go
    }
  else // get in queue and wait
    {
      //get in queue
      DEFINE_WAIT(wait);
      while (1) // do we need a  condition? head of the queue?
	{ 
	  prepare_to_wait( b->queue , &wait, TASK_INTERRUPTIBLE);
	  // release lock
	  spin_unlock( &(b->spin_lock) );
	  // go to sleep
	  if (1) // do we need a  condition? head of the queue?
	    schedule();
	  finish_wait( b->queue , &wait);
	}
      //if barrier is marked destroyed, set return to -1
      if (b->destroyed == 1)
	return_value = -1;
      //lock barrier
      spin_lock( &(b->spin_lock) );
      //decrement waiting_count
      b->waiting_count--;
      //if waiting count is now 0, destroy/clean up the barrier
      if (b->waiting_count == 0)
	  {
*/	    /*
	      cleanup
	    */
/*	  }
      //unlock barrier
      spin_unlock( &(b->spin_lock) );
    }

  // return the return value
  return return_value;*/
	return -1;
}

/**
 * helper function, gets a barrier node that contains the barrier with a given ID
 * returns NULL if it was not found
 */
struct barrier_node* _get_barrier_node(int barrierID)
{
  /*
    go through the list, checks ID, return pointer
    if not found, error
  */
  struct barrier_node *tmp;
  struct list_head *pos;
  list_for_each( pos, barrier_list ){
    tmp = list_entry(pos, struct barrier_node, list);
    if (tmp->barrier->bID == barrierID)
      {
	return tmp;
      }
  }
  return NULL;
}


/**
 * helper function, gets a barrier with a given ID
 * returns NULL if it was not found
 */
struct barrier_struct* _get_barrier(int barrierID)
{
/*  struct barrier_node *bn;
  bn = _get_barrier_node(barrierID);
  if (bn == NULL)
    return NULL;
  return bn->barrier;*/
	return NULL;
}


/**
 * Add a barrier node to the barrier list, don't check for any problems
 * cool tutorial: http://isis.poly.edu/kulesh/stuff/src/klist/
 */
int _add_barrier_node(struct barrier_struct* b)
{
  struct barrier_node *tmp = NULL;
  // check if barrier is not NULL
  if (b == NULL)
    return -EINVAL;
  // makes sure the list is initialized
  printk(KERN_INFO "If first barrier, we INIT\n");
  if (barrier_list == NULL)
    {
      barrier_list = (struct list_head *)kmalloc(sizeof(struct list_head),GFP_KERNEL);
      if (NULL == barrier_list) {return -ENOMEM;}
      INIT_LIST_HEAD( barrier_list );
      printk(KERN_INFO "INIT done, success\n");
    }
  printk(KERN_INFO "it wasn't the first barrier\n");
  tmp= (struct barrier_node *)kmalloc(sizeof(struct barrier_node),GFP_KERNEL);
  if (NULL == tmp) {return -ENOMEM;}
  // copy b to tmp... deep or shallow copy? shallow
  tmp->barrier = b;
  // add tmp node to the global list
  printk(KERN_INFO "we add the new barrir to the list...");
  list_add( &(tmp->list), barrier_list );
  printk(KERN_INFO "done\n");
  // DOES LIST_ADD RETURNS ERROR?
  // if we're here, everything should have worked
  return 0;
}

/**
 * Generates IDs for newly created barriers
 */
unsigned int _next_id(void) {
	/*
		start with current value of next_id
		check if it's in use
		if so, increment and try again until you find one that isn't
		inefficient as hell, but honestly shouldn't ever be an issue
	*/
}


/**
 * DEBUG!!!!
 */
int create_test_barrier(int id) {
  if (_testb != NULL)
    {
      kfree(_testb);
      _testb = NULL;
    }
  _testb = (struct barrier_struct *)kmalloc(sizeof(struct barrier_struct),GFP_KERNEL);
  if (_testb == NULL) {return -ENOMEM;}
  _testb->bID = id;
  _testb->initial_count = 3;
  _testb->waiting_count = 0;
  _testb->destroyed = 0;
  spin_lock_init( &(_testb->spin_lock) );
  _testb->queue = (wait_queue_head_t *)kmalloc(sizeof(wait_queue_head_t),GFP_KERNEL);
  if (_testb->queue == NULL) {return -ENOMEM;}
  init_waitqueue_head( _testb->queue );
  printk(KERN_INFO "created barrier with id=%i\n",_testb->bID);
}

int barrierwait_test() {
  int return_value = 0;
  printk(KERN_INFO "we're inside barrierwait_test\n");
  if (_testb == NULL)
    {
      printk(KERN_INFO "barrierwait: barrier NULL??\n");
      return -123;
    }
  // lock barrier
  spin_lock( &(_testb->spin_lock) );
  // update the counter of people waiting
  _testb->waiting_count++;
  if (_testb->waiting_count == _testb->initial_count) // all processes are here
    {
      // wake up everyone
      wake_up_all(_testb->queue);
      // remove 1 to waiting count
      _testb->waiting_count--;
      // release lock
      spin_unlock( &(_testb->spin_lock) );
      // go
      printk(KERN_INFO "we waked everybody up\n");
      return 0;
    }
  else // get in queue and wait
    {
      //get in queue
      printk(KERN_INFO "we get in queue\n");
      DEFINE_WAIT(wait);
      //while (1) // do we need a  condition? head of the queue?
      //{ 
	  printk(KERN_INFO "prepare_to_wait\n");
	  prepare_to_wait( _testb->queue , &wait, TASK_INTERRUPTIBLE);
	  // release lock
	  spin_unlock( &(_testb->spin_lock) );
	  // go to sleep
	  if (1) // do we need a  condition? head of the queue?
	    schedule();
	  printk(KERN_INFO "we just woke up\n");

	  //}
      //if barrier is marked destroyed, set return to -1
      if (_testb->destroyed == 1)
	{
	  printk(KERN_INFO "barrier in destroyed mode?\n");
	  return_value = -1214;
	}
      //lock barrier
      spin_lock( &(_testb->spin_lock) );
      // get out of the list
      finish_wait( _testb->queue , &wait);
      //decrement waiting_count
      _testb->waiting_count--;
      //if waiting count is now 0, destroy/clean up the barrier
      if (_testb->waiting_count == 0)
	  {
	    /*
	      cleanup
	    */
	    printk(KERN_INFO "we should cleanup barrier here and now\n");
	  }
      //unlock barrier
      spin_unlock( &(_testb->spin_lock) );
    }

  // return the return value
  return return_value;

}
