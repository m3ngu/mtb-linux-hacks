/* fail.c: implementation of fail syscall */

_syscall3(int, fail, int, ith,
	 int, ncall, 
	 struct syscall_failure *, 
	 calls);
	 
int sys_fail(int ith, int ncall, struct syscall_failure *calls) {

}