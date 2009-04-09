
/* mru_test.c: temp test, check that the system call does something
	4/04/09
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
//#include "asm/unistd.h"
//#include <hw5_definitions.h>

// our system call
_syscall1(int, set_cachepolicy, int, policy)


int main() {

  printf("we call set_cachepolicy to CACHE_NORMAL\n");
  
  int res = set_cachepolicy(CHANGE_NORMAL);
  if (res < 0)
    return -1;
    //perror("can't set cache policy to change normal? ");
  return 0;
}
