/* change_userweight.c: change the weight of a UID passed on the command line
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "asm/unistd.h"

_syscall1(int, getuserweight, int, uid)
_syscall2(int, setuserweight, int, uid, int, weight)

int main (int argc, char *argv[]) {
	if (3 != argc) exit(2);
	int uid = atoi(argv[1]);
	int weight = atoi(argv[2]);
	int ret = setuserweight(uid,weight);
	if (ret) perror("Failed to set user weight");
}
