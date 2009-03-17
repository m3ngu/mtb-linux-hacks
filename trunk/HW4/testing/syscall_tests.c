/* syscall_tests.c: test the basic functioning of our two new system calls
	3/16/09
	Ben Warfield
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "asm/unistd.h"

_syscall1(int, getuserweight, int, uid)
_syscall2(int, setuserweight, int, uid, int, weight)

int main() {
	int weight_found;
	puts("[1a]	get your own user weight");
	weight_found = getuserweight(-1);
	if ( 0 < weight_found ) {
		printf("[1a]	got weight %d\n", weight_found);
	} else 
		perror("[1a]	failed");
	
	puts("[1b]	get your own weight, the hard way");
	int uid = getuid();
	weight_found = getuserweight(uid);
	if ( 0 < weight_found ) {
		printf("[1b]	got weight %d\n", weight_found);
	} else 
		perror("[1b]	failed");
	
	uid = 104; /* sshd */
	puts("[1c]	get weight for another user (sshd)");
	weight_found = getuserweight(uid);
	if ( 0 < weight_found ) {
		printf("[1c]	got weight %d\n", weight_found);
	} else 
		perror("[1c]	failed");
	
	uid = 0;
	puts("[1d]	get weight for another user (root)");
	weight_found = getuserweight(uid);
	if ( 0 < weight_found ) {
		printf("[1d]	got weight %d\n", weight_found);
	} else 
		perror("[1d]	failed");

	uid = -3;
	puts("[1e]	get weight for impossible user (-3)");
	weight_found = getuserweight(uid);
	if ( 0 < weight_found ) {
		printf("[1e]	got weight %d\n", weight_found);
	} else 
		perror("[1e]	failed as expected");

	uid = 3000000;
	puts("[1e]	get weight for impossible user (3000000)");
	weight_found = getuserweight(uid);
	if ( 0 < weight_found ) {
		printf("[1e]	got weight %d\n", weight_found);
	} else 
		perror("[1e]	failed as expected");	
		
	uid = 50000;
	puts("[1f]	get weight for possible but invalid user (50000)");
	weight_found = getuserweight(uid);
	if ( 0 < weight_found ) {
		printf("[1f]	got weight %d\n", weight_found);
	} else 
		perror("[1f]	as expected");	
		

		
	uid = getuid();
	printf("Current UID: %d\n", uid);
	weight_found = setuserweight(uid,20);
	if (0 == weight_found) puts("[2a]	unexpected success!");
	else perror("[2a]	try to set your weight (possibly not as root)");

	puts("[2b]	get your own user weight");
	weight_found = getuserweight(-1);
	if ( 0 < weight_found ) {
		printf("[2b]	got weight %d\n", weight_found);
	} else 
		perror("[2b]	failed");	
	
	return EXIT_SUCCESS;
}
