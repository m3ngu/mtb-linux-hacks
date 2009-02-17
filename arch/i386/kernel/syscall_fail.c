/* fail.c: implementation of fail syscall */


#include <linux/fail.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <asm-i386/uaccess.h>
/* XXX undeclared: copy_from_user.  Where the heck is it? */

/*
_syscall3(int, fail, int, ith,
	 int, ncall, 
	 struct syscall_failure *, 
	 calls);
	 */
/*
	int fail_skip_count;
	int fail_vec_length;
	struct syscall_failure *fail_vector;
*/

/**
 * implements the sysem call, put a list of syscall in the kernel memory
 * (attached to a task) + when do we fail
 * many sanity checks, can return -EINVAL and -ENOMEM
 */
asmlinkage long sys_fail(int ith, int ncall, struct syscall_failure *calls) {
	struct task_struct *tsk = current;
	int i, copy_ret;
	int internal_error = 0;
	if ( 0 > ith || 1 > ncall || NULL == calls) 	return -EINVAL;	
	if ( NULL != tsk->fail_vector) 	return -EINVAL;

	tsk->fail_vector = kmalloc(
		ncall * sizeof(struct syscall_failure),
		GFP_KERNEL
	);
	if ( NULL == tsk->fail_vector ) return -ENOMEM;
	
	copy_ret = copy_from_user(
		tsk->fail_vector, calls, ncall * sizeof(struct syscall_failure)
	);
	if (copy_ret) { /* copy failed: return EFAULT */
		internal_error = EFAULT;
	}

	for ( i = 0; !internal_error && i < ncall; i++ ) {
		if ( 0 > tsk->fail_vector[i].syscall_nr  
			|| NR_syscalls <= tsk->fail_vector[i].syscall_nr 
			|| 0 > tsk->fail_vector[i].error ) { 
				/* problem: free memory, and return EINVAL */ 
				internal_error = EINVAL;
		}
	}
	
	if (internal_error) {
		free_fail(tsk);
		return -internal_error;
	}
	
	tsk->fail_skip_count = ith;
	tsk->fail_vec_length = ncall;
	return 0;
}

/**
 * Called right before a system call, checks if we must generate an error
 * Returns 0 or the appropriate error (negated)
 */
asmlinkage long syscall_fail(long syscall_nr) {
	struct task_struct *tsk = current;
	int i;
	int err = -1;
	// iterate over syscalls in kernel memory
	for (i = 0; i < tsk->fail_vec_length; i++)
	  {
	    if (tsk->fail_vector[i].syscall_nr == syscall_nr)
	      {
		/* printk(KERN_ERR "Found syscall in intercept list..."); */
		tsk->fail_skip_count = 	tsk->fail_skip_count - 1;
		err = tsk->fail_vector[i].error;
		/* printk(KERN_ERR "error found is %d, skip count now %d\n", 
			err, tsk->fail_skip_count); */
		break;
	      }
	  }
	// do we send an error?
	if ( tsk->fail_skip_count < 0)
	  {
		/* printk(KERN_ERR "Cleaning up and returning %d\n", err); */
	    free_fail(tsk);
	    return -err;
	  }
	// we do not fake fail, continue normally
	return 0;
}

/**
 * clean the task_struct of the 'fail pointers'
 */
void free_fail(struct task_struct *tsk)
{
	tsk->fail_skip_count = 0;
	tsk->fail_vec_length = 0;
	if (NULL != tsk->fail_vector) {
		kfree(tsk->fail_vector);
		tsk->fail_vector = NULL;
	}
}
