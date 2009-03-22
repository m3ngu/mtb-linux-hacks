# makefile -- to compile our testing code
#
# Team 2: T. Bertin-Mahieux, M. Sukan, B. Warfield
#   W4118: Operating Systems HW4


CFLAGS= -std=c99 -Wall -pedantic -g
TREE_TAG=.hmwk4

all: test-syscalls change_scheduler inf-loop unchange_scheduler

test-syscalls: syscall_tests.c
	$(CC) $(CFLAGS) -I /usr/src/linux-2.6.11.12$(TREE_TAG)/include -o $@ $^ 

change_scheduler: change_scheduler.c
	$(CC) $(CFLAGS) -I /usr/src/linux-2.6.11.12$(TREE_TAG)/include -o $@ $^ 

inf-loop: infinite_loop.c
	$(CC) $(CFLAGS) -o $@ $^ 

unchange_scheduler: unchange_scheduler.c
	$(CC) $(CFLAGS) -o $@ $^

test: test-syscalls
	./$^

clean:
	rm test-syscalls change_scheduler inf-loop
