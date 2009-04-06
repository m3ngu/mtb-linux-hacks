/* swap_central.c - use horrible amounts of memory, very inefficiently.
	4/6/2009
	Ben Warfield
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct circular_illogic {
	struct circular_illogic *prev;
	struct circular_illogic *next;
	char space_waster[2048];
};

int main(int argc, char *argv[]) {
	int total_list_members = argc > 1 ? atoi(argv[1]) : 10000;
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
	for (int i = 0; /* muahahaha!!! */ ; i++) {
		cur = cur->next;
		int unused = strlen(cur->space_waster);
		if ( ! ( i % 10000) ) {
			printf("In step %d: string length is %d\n", i , unused);
		}
		if ( &head == cur ) {
			printf("In step %d: passing Go! (Can I have $200?)\n", i);
		}
	
	}
	
	
	
}
