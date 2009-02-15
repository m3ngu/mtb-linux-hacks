/* fail.h: user-accessible interface to the system-call failure injector */

#ifndef _FAIL_H
#define _FAIL_H
struct syscall_failure {
	long syscall_nr; /* what system call to fail */
	long error; /* what error code to return */
};

int fail(int ith, int ncall, struct syscall_failure *calls);
#endif

