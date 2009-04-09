
/* mru_test.c: temp test, check that the system call does something
	4/04/09
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "asm/unistd.h"

#include "linux/hw5_definitions.h"

// our system call
_syscall1(int, set_cachepolicy, int, policy)


int main() {

  printf("we call set_cachepolicy to CACHE_MRU\n");
  
  int res = set_cachepolicy(CACHE_MRU);
  if (res < 0)
    {
      perror("can't set cache policy to CACHE_MRU? ");
      return -1;
    }
  return 0;
}
