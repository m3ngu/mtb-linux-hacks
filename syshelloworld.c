/* syshelloworld.c
	Use the simplest syscall ever, see if it works
*/

#include "/usr/src/linux-2.6.11.12-hwk2/include/asm/unistd.h"
#include <errno.h>
#include <stdio.h>
/* _syscall0(int,helloworld); */
_syscall1(int,helloworld,char *, arg);

int main() {
	/* helloworld(); */
	if (helloworld("Hey there!\n")) perror("Fail:");
}