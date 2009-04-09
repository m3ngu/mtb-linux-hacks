

/* mru_test.c: temp test, check that the system call does something
	4/04/09
*/
//#define _REENTRANT
//#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include "asm/unistd.h"

_syscall1(int, set_cachepolicy, int, policy)



int main() {

  printf("we call set_cachepolicy\n");
  
  set_cachepolicy(1);

  return 0;
}
