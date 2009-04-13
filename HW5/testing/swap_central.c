/* swap_central.c - use horrible amounts of memory, very inefficiently.
	4/6/2009
	Ben Warfield
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
	Uselessly large circular doubly-linked list structure.  We will 
	create lots of them.
*/

struct circular_illogic {
	struct circular_illogic *prev;
	struct circular_illogic *next;
	char space_waster[2048];
};

/*
	Usage: swap-central [ list_size [ scans ] ]
		Will create a circular list of the given size (or 10000)
		then scan through the entire list the given number of times (or 5)
*/

int main(int argc, char *argv[]) {
	int total_list_members = argc > 1 ? atoi(argv[1]) : 10000;
	int iterations = argc > 2 ? atoi(argv[2]) : 5;
	/* infinite loop on -1?  But how? */
	struct circular_illogic head;
	head.prev = &head;
	head.next = &head;
	strcpy(head.space_waster, "this uses a trivial amount of space");
	printf("Producing loop of %d list members to traverse\n", total_list_members);
	
	for (int i = 0; i < total_list_members; i++) {
		struct circular_illogic *new = malloc(sizeof(struct circular_illogic));
		if (NULL == new) {
			perror("Allocation failed");
			exit(2);
		}
		// Produces a random integer between 0 and 1023 by masking
		// a 32-bit integers all but last 9 digits
		int charcount = rand() & 1023;
		for (int j = 0; j < charcount; j++) {
			new->space_waster[j] = 'a';
		}
		new->space_waster[charcount] = '\0';
		new->next = &head;
		new->prev = head.prev;
		head.prev->next = new;
		head.prev = new;
	}
	
	struct circular_illogic *cur = &head;
	int super_total_number = total_list_members * iterations;
	for (int i = 0; i < super_total_number ; i++) {
		cur = cur->next;
		int unused = strlen(cur->space_waster);
		if ( ! ( i % 50000) ) {
			printf("In step %d out of %d\n", i , super_total_number);
		}
		if ( &head == cur ) {
			printf("In step %d: passing Go! (Can I have $200?)\n", i);
		}
	
	}
	
	
	
}
