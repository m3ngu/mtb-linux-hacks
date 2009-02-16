/* linux/fail.h: back-end function declarations for syscall_fail.h */

#ifndef _LINUX_FAIL_H
#define _LINUX_FAIL_H
#include <linux/linkage.h>
#include <linux/sched.h>
#include <fail.h>

asmlinkage long syscall_fail(long syscall_nr);

void free_fail(struct task_struct *tsk);

#endif
