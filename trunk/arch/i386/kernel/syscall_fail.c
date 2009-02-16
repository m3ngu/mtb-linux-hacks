/* fail.c: implementation of fail syscall */


#include <linux/fail.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/errno.h>
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
asmlinkage long sys_fail(int ith, int ncall, struct syscall_failure *calls) {
	struct task_struct *tsk = current;
	int i;
	int invalid_arg = 0;
	if ( 0 > ith || 1 > ncall || NULL == calls) 	return -EINVAL;	
	if ( NULL != tsk->fail_vector) 	return -EINVAL;
	tsk->fail_vector = kmalloc(
		ncall * sizeof(struct syscall_failure),
		GFP_KERNEL
	);
	if ( NULL == tsk->fail_vector ) return -ENOMEM;
	/* printk(KERN_ERR "Syscall 'fail' arrived at end of current code\n"); */
	for ( i = 0; i < ncall; i++ ) {
		if ( 0 > syscall_failure[i].syscall_nr  
			|| NR_syscalls <= syscall_failure[i].syscall_nr 
			|| 0 > syscall_failure[i].error ) { 
				/* problem: free memory, and return EINVAL */ 
				invalid_arg = 1;
				break;
		}
		copy_from_user(tsk->fail_vector + i, calls + i, 
			sizeof(struct syscall_failure)
		);
	}
	if (invalid_arg) {
		free_fail(tsk);
		return -EINVAL;
	}
	tsk->fail_skip_count = ith;
	tsk->fail_vec_length = ncall;
	return 0;
}

asmlinkage long syscall_fail(long syscall_nr) {
	struct task_struct *tsk = current;

	if (0) { /* we actually fail */
		free_fail(tsk);
		return -1;
	}
	return 0;
}

void free_fail(struct task_struct *tsk)
{
	tsk->fail_skip_count = 0;
	tsk->fail_vec_length = 0;
  if (NULL != tsk->fail_vector) kfree(tsk->fail_vector);
}
