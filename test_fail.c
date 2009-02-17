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
	s.syscall_nr = __NR_fail;
	s.error = ENOTSOCK; 
	res = fail(1,1, (struct syscall_failure *) 1024);
	if (res) perror("Error result from call 1");
	else puts("Unexpected success on call 1");
	res = fail(0,1,&s); 
	if (res) perror("Unexpected failure from call 2");
	else puts("Success on call 2");
	res = fail(1,1,&s);
	if (res) perror("Error result from call 3");
	else puts("Unexpected success on call 3");
	s.error = ENOMEDIUM;
	res = fail(1,1,&s);
	if (res) perror("Error result from call 4");
	else puts("Success on call 4");
	res = fail(1,1,&s);
	if (res) perror("Error result from call 5");
	else puts("Unexpected success on call 5");
	res = fail(1,1,&s);
	if (res) perror("Error result from call 6");
	else puts("Unexpected success on call 6");

}
