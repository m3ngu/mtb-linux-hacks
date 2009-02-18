/* test_fail.c: test the new system call */

#include "fail.h"
#include "asm/unistd.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

_syscall3(int, fail, int, ith,
	 int, ncall, 
	 struct syscall_failure *, 
	 calls);

int main() {
	
	struct syscall_failure s;
	int res = 0; // Result
	
	puts("----------------------------------------------");
	puts("Part-1 Testing bad argument handling in fail()");
	puts("----------------------------------------------");
	
	s.syscall_nr = __NR_fail;
	s.error = ENOTSOCK; 
	
	res = fail(-1,1, &s);
	
	printf("[1a] Invalid ith (-1) => fail() returned %i\n", res);
	if (res) perror("[1a]");
	
	res = fail(1,-1, &s);
	
	printf("[1b] Invalid ncall (-1) => fail() returned %i\n", res);
	if (res) perror("[1b]");
	
	res = fail(1,0, &s);
	
	printf("[1c] Invalid ncall (0) => fail() returned %i\n", res);
	if (res) perror("[1c]");
	
	res = fail(0,1, (struct syscall_failure *) 1024);
	
	printf("[1d] Invalid *calls (bad pointer => fail() returned %i\n", res);
	if (res) perror("[1d]");
	
	puts("------------------------------------");
	puts("Part-2 Testing wrong usage of fail()");
	puts("------------------------------------");
	
	res = fail(1,1,&s);
	
	printf("[2a] A valid call to fail(1,1,__NR_fail) => returned %i\n", res);
	if (res) perror("[2a]");
	
	res = fail(1,1,&s);
	
	printf("Same call to fail() repeated immediately => returned %i\n", res);
	if (res) perror("[2a]");
	
	// This should fail because we told it to
	res = fail(1,1,&s);
	
	printf("Same call to fail() repeated 3rd time => returned %i\n", res);
	if (res) perror("[2a]");
	
	struct syscall_failure sarr[2];
	sarr[0] = s;
	sarr[1] = s;
	
	res = fail(1,2,sarr);
	
	printf("[2b] Repetitive syscall_nr in *calls => fail() returned %i\n", res);
	if (res) perror("[2b]");
		
	puts("-------------------------------");
	puts("Part-3 Calling fail() in a loop");
	puts("-------------------------------");
	
	puts("[3a] Calling fail() with ith = 0");
	
	s.syscall_nr = __NR_getpid;
	s.error = ENOTSOCK;
	
	res = fail(0,1,&s);
	if (res) perror("[3a]");
	
	int i;
	pid_t pid;
	
	for (i = 0; i < 3; i++) {
		pid = getpid();
		printf("Syscall returned %i in iteration %i\n", pid, i);
	}
	
	puts("[3b] Calling fail() with ith = 3");
	
	res = fail(3,1,&s);
	if (res) perror("[3a]");
	
	for (i = 0; i < 6; i++) {
		pid = getpid();
		printf("Syscall returned %i in iteration %i\n", pid, i);
	}
	
	//TODO Other tests

}
