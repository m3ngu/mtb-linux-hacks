/* barrier_test.c: various test/demonstration code for kernel barriers
	2/25/09
*/


#include <stdio.h>
#include <stdlib.h>

_syscall1(int, barriercreate, 	int, num);
_syscall1(int, barrierdestroy, 	int, id);
_syscall1(int, barrierwait, 	int, id);

int main() {
	int barrier1 = barriercreate(1);
	fputs("Waiting for single-process barrier...", stderr);
	if ( barrierwait(barrier1) ) perror("inexplicable failure")
	else fputs("success!\n", stderr);
	return EXIT_SUCCESS;
}
