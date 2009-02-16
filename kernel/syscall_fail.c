/* fail.c: implementation of fail syscall */


#include <fail.h>


_syscall3(int, fail, int, ith,
	 int, ncall, 
	 struct syscall_failure *, 
	 calls);
	 
int sys_fail(int ith, int ncall, struct syscall_failure *calls) {

}


void free_fail(struct task_struct *tsk)
{
  if (NULL != tsk->fail_vector) kfree(tsk->fail_vector);
}
