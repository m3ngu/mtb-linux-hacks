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
#include <asm/atomic.h>
#include <linux/errno.h>
#include <linux/sched.h>

/**
 * Barrier structure
 */
struct barrier_struct {
	unsigned int bID;			// barrier ID
	spinlock_t spin_lock;		// spin lock
	atomic_t waiting_count;		// items in queue
	int initial_count;			// original N, or barrier size
	wait_queue_head_t queue;	// queue
	int destroyed;				// set to 1 if we're in the destruction function
};

struct barrier_node {
	struct barrier_struct *barrier;
	struct list_head list;
};

static LIST_HEAD(barrier_list);
//static unsigned int next_id = 1;
static atomic_t nex_id = ATOMIC_INIT(0);

/*
struct barrier_struct* _get_barrier(int barrierID);
int _add_barrier_node(struct barrier_struct* b);
unsigned int _next_id(void);
*/
void display(void);
struct barrier_node* _get_barrier_node(int barrierID);

asmlinkage int sys_barriercreate(int num)
{
	// check input
	if(num < 1) {return -EINVAL;}
	
	// create barrier node, allocate memory
	struct barrier_node *bnPtr = (struct barrier_node *)kmalloc(sizeof(struct barrier_node), GFP_KERNEL);
    if (NULL == bnPtr) {return -ENOMEM;}
    
    // create barrier, allocate memory
    struct barrier_struct *b = (struct barrier_struct *)kmalloc(sizeof(struct barrier_struct), GFP_KERNEL);
    if (NULL == b) {
    	kfree(bnPtr);
    	return -ENOMEM;
    }
    
    b->initial_count = num;
    atomic_set( &b->waiting_count , num); // shouldn't this be zero?
    
    spin_lock_init( &b->spin_lock );
    
    // init wait queue head
    init_waitqueue_head( &b->queue );

    b->bID = next_id++; // better id management later...;
    // including some CONCURRENCY PROTECTION
    bnPtr->barrier = b;
    
    INIT_LIST_HEAD( &bnPtr->list );
    list_add( &bnPtr->list , &barrier_list);
    
    display();
    
    return b->bID;
}

asmlinkage int sys_barrierdestroy(int barrierID)
{
	// speial case barrierID = -1
	// destroy everythign :D
	
	int destroyall_return = 0;
	
	// TODO: Fix Segmentation fault
	if (barrierID == -1 /*&& !list_empty( &barrier_list )*/) {
	
		struct list_head *iter;
		struct barrier_node *objPtr;
		
		redo:
			__list_for_each(iter, &barrier_list) {
				objPtr = list_entry(iter, struct barrier_node, list);
				
				destroyall_return += sys_barrierdestroy( objPtr->barrier->bID );
				goto redo;
			}
        
        next_id = 1;
        
        return destroyall_return;
        
	}
  
  	// check ID validity
	if (barrierID < 0) {return -EINVAL;}
    
    int return_value = 0;
    struct barrier_node *objPtr = _get_barrier_node(barrierID);
    
    if (objPtr != NULL) {
    
    	// lock barrier
    	printk(KERN_INFO "in destroy routine for barrier %d\n", barrierID);
		spin_lock( &objPtr->barrier->spin_lock );
		
		// set destroyed mode
		objPtr->barrier->destroyed = 1;
		
		// if people in queue, wake them up, and count
		
		int wc = atomic_read(&objPtr->barrier->waiting_count);

		printk(KERN_INFO "Found %d waiters\n", wc);
		if (wc == objPtr->barrier->initial_count)
		{
			// wake up everyone
			wake_up_all( &objPtr->barrier->queue );
			// remember - number of awoken processes
			return_value = wc;
		}
        printk(KERN_INFO "freeing barrier\n");
        kfree(objPtr->barrier);
        printk(KERN_INFO "deleting from list\n");
        list_del(&objPtr->list);
        printk(KERN_INFO "freeing barrier list entry\n");
        kfree(objPtr);
        printk(KERN_INFO "trying to read current barrier list\n");
        display();
    }
	printk(KERN_INFO "Apparently done with destruct routine (WTF?)\n");
	return return_value;
}

asmlinkage int sys_barrierwait(int barrierID)
{
	int return_value = 0;
	
	// check ID validity
	if (barrierID < 0) {return -EINVAL;}
    
    struct barrier_node *objPtr = _get_barrier_node(barrierID);
    
    if (NULL == objPtr) { /* not found */
    	return -EINVAL;
    } else {
		
		unsigned long flags; // has some value? 0?
		
		// lock barrier
		spin_lock_irqsave( &objPtr->barrier->spin_lock , flags);
	
		// update the counter of people waiting
		if (atomic_dec_and_test( &objPtr->barrier->waiting_count )) // all processes are here (waiting_count=0)
		{
			// wake up everyone
			wake_up_all( &objPtr->barrier->queue);
			
			// add 1 to waiting count (waiting_count == initial_count) means everyone has left
			atomic_inc( &objPtr->barrier->waiting_count );
			
			// release lock
			spin_unlock_irqrestore( &objPtr->barrier->spin_lock , flags);
			
			// go
			printk(KERN_INFO "We woke everybody up\n");
			return 0;
		}
		else // get in queue and wait
		{
			//get in queue
			printk(KERN_INFO "We get in queue\n");
		
			DEFINE_WAIT(wait);
		
			printk(KERN_INFO "Prepare_to_wait\n");
			prepare_to_wait( &objPtr->barrier->queue , &wait, TASK_INTERRUPTIBLE);

			// release lock
			spin_unlock_irqrestore( &objPtr->barrier->spin_lock , flags);
			
			// go to sleep
			if (1) // do we need a  condition? head of the queue?
				schedule();

			printk(KERN_INFO "We just woke up\n");

			//if barrier is marked destroyed, set return to -1
			if (objPtr->barrier->destroyed == 1)
			{
				printk(KERN_INFO "Barrier in destroyed mode?\n");
				return_value = -1214;
			}

			//lock barrier
			spin_lock_irqsave( &objPtr->barrier->spin_lock , flags);
			
			// get out of the list
			finish_wait( &objPtr->barrier->queue , &wait);
			
			//increment waiting_count
			// but really, no--why?
			atomic_inc( &objPtr->barrier->waiting_count );
			
			//if waiting count is now 0, destroy/clean up the barrier
			/*
			if (atomic_sub_and_test())
			{
				// cleanup
			printk(KERN_INFO "we should cleanup barrier here and now\n");
			}
			*/
			
			//unlock barrier
			spin_unlock_irqrestore( &objPtr->barrier->spin_lock , flags);
		}
	}
	// return the return value
	return return_value;

}

void display(void)
{
    struct list_head *iter;
    struct barrier_node *objPtr;

    printk(KERN_INFO "Current barrier list:\n");
    __list_for_each(iter, &barrier_list) {
    	printk(KERN_DEBUG "Current list pointer: %p\n", iter);
        objPtr = list_entry(iter, struct barrier_node, list);
    	printk(KERN_DEBUG "Current item pointer: %p\n", objPtr);
    	printk(KERN_DEBUG "    id:%d|cap:%d\n"
        	, objPtr->barrier->bID
        	, objPtr->barrier->initial_count);
    }
    printk(KERN_INFO "End of list\n");
}

/**
 * helper function, gets a barrier node that contains the barrier with a given ID
 * returns NULL if it was not found
 */
struct barrier_node* _get_barrier_node(int barrierID)
{
	struct list_head *iter;
    struct barrier_node *objPtr;

    __list_for_each(iter, &barrier_list) {
        objPtr = list_entry(iter, struct barrier_node, list);
        if(objPtr->barrier->bID == barrierID) {
        	return objPtr;
        }
    }
	
	printk(KERN_INFO "Barrier %d not found!\n",barrierID);
    return NULL; // Not found
}

/**
 * Get the next ID by incrementing next_id (using an atomic function)
 */
int _get_next_id()
{
  return atomic_inc_return( &next_id );
}


