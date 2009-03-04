/* barrier.c
	Implementation of barrier synchronization primitive for Linux 2.6.11.12
	2/25/2009
	Ben Warfield, Mengu Sukan, Thierry Bertin-Mahieux
*/


#include <linux/list.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/linkage.h> // asmlinkage
#include <asm/semaphore.h>
#include <linux/errno.h>
#include <linux/sched.h>

/**
 * Barrier structure
 */
struct barrier_struct {
	unsigned int bID;		// barrier ID
	spinlock_t spin_lock;		// spin lock
	atomic_t refcount;
	int waiting_count;		// items in queue
	int initial_count;		// original N, or barrier size
	wait_queue_head_t queue;	// queue
	int barrier_iteration;
	int destroyed;			// set to 1 if we're in the destruction function
};

struct barrier_node {
	struct barrier_struct *barrier;
	struct list_head list;
};

static LIST_HEAD(barrier_list);
static DECLARE_MUTEX(search_lock);
static atomic_t next_id = ATOMIC_INIT(0);

/* TODO: block comment */
int _get_next_id(void);
void _leave_barrier(struct barrier_node *o, unsigned long f);
int destroyall(void);
void display(void);
struct barrier_node* _get_barrier_node(int barrierID);

asmlinkage int sys_barriercreate(int num)
{
	printk(KERN_INFO "entering sys_barriercreate\n");
	// check input
	if(num < 1) {return -EINVAL;}
	printk(KERN_INFO "creating barrier of size %d\n", num);
	// create barrier node, allocate memory
	struct barrier_node *bnPtr = (struct barrier_node *)kmalloc(sizeof(struct barrier_node), GFP_KERNEL);

	if (NULL == bnPtr) {return -ENOMEM;}
	
	// create barrier, allocate memory
	struct barrier_struct *b = (struct barrier_struct *)kmalloc(sizeof(struct barrier_struct), GFP_KERNEL);
	if (NULL == b) {
		kfree(bnPtr);
		return -ENOMEM;
	}
	
	printk(KERN_INFO "memory allocated\n");
	b->initial_count = num;
	atomic_set( &b->refcount , 0); // shouldn't this be zero?
	b->waiting_count = 0;
	b->destroyed = 0;
	b->barrier_iteration = 0;
	
	spin_lock_init( &b->spin_lock );
	
	// init wait queue head
	init_waitqueue_head( &b->queue );

	b->bID = _get_next_id();
	bnPtr->barrier = b;
	
	INIT_LIST_HEAD( &bnPtr->list );
	list_add( &bnPtr->list , &barrier_list);
	printk(KERN_INFO "initialization complete: id is %d\n", b->bID);

	
	display();
	
	return b->bID;
}

int destroyall(void) {
	
	int destroyall_return = 0;
	
	struct list_head *iter;
	struct barrier_node *objPtr;
	
	redo:
	__list_for_each(iter, &barrier_list) {
		objPtr = list_entry(iter, struct barrier_node, list);
		
		destroyall_return += sys_barrierdestroy( objPtr->barrier->bID );
		goto redo;
	}
	
	return destroyall_return;
}

asmlinkage int sys_barrierdestroy(int barrierID)
{
	// speial case barrierID = -1
	// destroy everythign :D
	if (barrierID == -1) {return destroyall();}
	
  	// check ID validity
	if (barrierID < 0) {return -EINVAL;}
	
	int return_value = 0;
	struct barrier_node *objPtr = _get_barrier_node(barrierID);
	unsigned long flags;
	
	if (objPtr != NULL) {
		printk(KERN_INFO "deleting from list\n");
		down(&search_lock);
		list_del(&objPtr->list);
		up(&search_lock);
		// lock barrier
		printk(KERN_INFO "in destroy routine for barrier %d\n", barrierID);
		spin_lock_irqsave( &objPtr->barrier->spin_lock, flags );
		
		// set destroyed mode
		objPtr->barrier->destroyed = 1;
		
		// if people in queue, wake them up, and count
		
		int wc = objPtr->barrier->waiting_count;

		printk(KERN_INFO "Found %d waiters\n", wc);
		if (wc != 0)
		{
			// wake up everyone
			wake_up_all( &objPtr->barrier->queue );
			// remember - number of awoken processes
			return_value = wc;
		}
		_leave_barrier(objPtr, flags);
		printk(KERN_INFO "trying to read current barrier list\n");
		display();
	}
	printk(KERN_INFO "Exiting destruct routine\n");
	return return_value;
}

asmlinkage int sys_barrierwait(int barrierID)
{
	int return_value = 0;
	int pid = current->tgid;
	
	// check ID validity
	if (barrierID < 0) {return -EINVAL;}
	
	struct barrier_node *objPtr = _get_barrier_node(barrierID);
	
	if (NULL == objPtr) { /* not found */
		return -EINVAL;
	} else {
		int my_iteration;
		unsigned long flags; // overwritten by macro
		
		// lock barrier
		struct barrier_struct *b = objPtr->barrier;
		spin_lock_irqsave( &b->spin_lock , flags);
		if (b->destroyed) {
			/* save some time: return -1 */
			/* but we'll need to call _leave_barrier anyway */
		}
		my_iteration = b->barrier_iteration;
		// update the counter of people waiting
		if (++b->waiting_count == b->initial_count ) // barrier full
		//if (atomic_dec_and_test( &objPtr->barrier->waiting_count )) // all processes are here (waiting_count=0)
		{
			// wake up everyone
			b->barrier_iteration++;
			b->waiting_count = 0;
			wake_up_all( &objPtr->barrier->queue);
			
			// add 1 to waiting count (waiting_count == initial_count) means everyone has left

			// atomic_inc( &objPtr->barrier->waiting_count );
			// XXX does this need to check overflow?
			// release lock
			spin_unlock_irqrestore( &b->spin_lock , flags);
			
			// go
			printk(KERN_INFO "Process %d woke everybody up\n", pid);
			return 0;
		}
		else // get in queue and wait
		{
			//get in queue
			// release lock (do this before prepare_to_wait?)
			spin_unlock_irqrestore( &b->spin_lock , flags);

			printk(KERN_INFO "Process %d getting in queue\n", pid);
		
			DEFINE_WAIT(wait);
		
			printk(KERN_INFO "Process %d Prepare_to_wait\n", pid);
			prepare_to_wait( &b->queue , &wait, TASK_INTERRUPTIBLE );

			
			// go to sleep
			while (b->barrier_iteration == my_iteration && 0 == b->destroyed) // still the barrier we were waiting for
				schedule();

			printk(KERN_INFO "Process %d just woke up\n", pid);
			// get out of the list
			finish_wait( &objPtr->barrier->queue , &wait);

			//lock barrier
			spin_lock_irqsave( &b->spin_lock , flags);
			//if barrier is marked destroyed, set return to -1
			if (b->barrier_iteration == my_iteration) {
				return_value = -1;
			}
			_leave_barrier(objPtr, flags);
		}
	}
	// return the return value
	return return_value;

}

void display(void)
{
	struct list_head *iter;
	struct barrier_node *objPtr;
	down(&search_lock);

	printk(KERN_INFO "Current barrier list:\n");
	__list_for_each(iter, &barrier_list) {
		printk(KERN_DEBUG "Current list pointer: %p\n", iter);
		objPtr = list_entry(iter, struct barrier_node, list);
		printk(KERN_DEBUG "Current item pointer: %p\n", objPtr);
		printk(KERN_DEBUG "id:%d|cap:%d\n"
			, objPtr->barrier->bID
			, objPtr->barrier->initial_count);
	}
	up(&search_lock);
	printk(KERN_INFO "End of list\n");
}

/**
 * helper function, gets a barrier node that contains the barrier with a given ID
 * returns NULL if it was not found
 */
struct barrier_node* _get_barrier_node(int barrierID)
{
	struct list_head *iter;
	struct barrier_node *objPtr = NULL;
	down(&search_lock);
	
	__list_for_each(iter, &barrier_list) {
		objPtr = list_entry(iter, struct barrier_node, list);
		if(objPtr->barrier->bID == barrierID) {
			break;
		}
	}
	if (NULL != objPtr) {
		atomic_inc(&objPtr->barrier->refcount);
	} else {
		printk(KERN_INFO "Barrier %d not found!\n", barrierID);
	}
	up(&search_lock);

	return objPtr; // Not found
}

/**
 * Get the next ID by incrementing next_id (using an atomic function)
 */
int _get_next_id()
{
  return atomic_inc_return( &next_id );
}

/*
	1. assume that we have the spin-lock
	2. decrement refcount
	3. if 0
		a. test for destroyed flag
		b. free memory if need be
	4. release spinlock
	
*/
void _leave_barrier(struct barrier_node *objPtr, unsigned long flags) {
	int pid = current->tgid;
	printk(KERN_INFO "process %d is leaving barrier %d\n",
		pid, objPtr->barrier->bID
	);
	if (NULL == objPtr) {
		printk(KERN_ERR "NULL list-node passed to _leave_barrier!\n");
		return;
	} else if ( NULL == objPtr->barrier ) {
		printk(KERN_ERR "NULL barrier passed to _leave_barrier!\n");
		return;
	}
	struct barrier_struct *b = objPtr->barrier;

	if ( atomic_dec_and_test( &b->refcount ) ) {
		printk(KERN_INFO "Process %d is last to leave barrier %d\n",
		pid, b->bID); // XXX undeclared
		if (b->destroyed == 1)
		{
			spin_unlock_irqrestore( &b->spin_lock , flags);
			printk(KERN_INFO "Barrier %d in destroyed mode\n", b->bID);
			printk(KERN_INFO "freeing barrier\n");
			kfree(b);
			printk(KERN_INFO "freeing barrier list entry\n");
			kfree(objPtr);
			return;
		}
	} 	
	//unlock barrier
	spin_unlock_irqrestore( &b->spin_lock , flags);
}
