/* barrier.c
	Implementation of the system call for HW5: MRU memory management 
	Linux 2.6.11.12
	04/04/2009
	Ben Warfield, Mengu Sukan, Thierry Bertin-Mahieux
*/



#include <linux/wait.h>
#include <linux/linkage.h> // asmlinkage
#include <asm/semaphore.h>
#include <linux/errno.h>
#include <linux/sched.h>




/**
 * change the memory management policy
 */
asmlinkage int sys_cachepolicy(int policy)
{
  printk("sys_cachepolicy in linux/mru.c, not implemented yet\n");
  return -1;
}

