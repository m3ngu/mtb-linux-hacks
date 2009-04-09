
/* mru_test.c: temp test, check that the system call does something
	4/04/09
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include <linux/hw5_definitions>

// our system call
_syscall1(int, set_cachepolicy, int, policy)


int main() {

  printf("we call set_cachepolicy to CACHE_NORMAL\n");
  
  int res = set_cachepolicy(CHANCE_NORMAL);
  if (res < 0)
    perror("can't set cache policy to change normal? ");
  return 0;
}
