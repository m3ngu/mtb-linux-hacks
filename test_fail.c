/* test_fail.c: test the new system call */

#include "/usr/src/linux-2.6.11.12-hwk2/include/fail.h"
#include "/usr/src/linux-2.6.11.12-hwk2/include/asm/unistd.h"
#include <stdio.h>
#include <errno.h>

_syscall3(int, fail, int, ith,
	 int, ncall, 
	 struct syscall_failure *, 
	 calls);

int main() {
	struct syscall_failure s;
	int res = 0;
	s.syscall_nr = 3;
	s.error = 42;
	res = fail(1,1,&s);
	printf("Got result %d\n", res);
	res = fail(1,1,&s);
	printf("Second, got result %d\n", res);
}
