# makefile -- to compile our testing code
#
# Team 2: T. Bertin-Mahieux, M. Sukan, B. Warfield
#   W4118: Operating Systems HW4


CFLAGS= -std=c99 -Wall -pedantic -g
TREE_TAG=-hwk4
BIN=test-syscalls change-scheduler inf-loop change-weight

all: $(BIN)

test-syscalls: syscall_tests.c
	$(CC) $(CFLAGS) -I /usr/src/linux-2.6.11.12$(TREE_TAG)/include -o $@ $^ 

change-weight: change_userweight.c
	$(CC) $(CFLAGS) -I /usr/src/linux-2.6.11.12$(TREE_TAG)/include -o $@ $^ 

change-scheduler: change_scheduler.c
	$(CC) $(CFLAGS) -o $@ $^ 

inf-loop: infinite_loop.c
	$(CC) $(CFLAGS) -o $@ $^ 

test: test-syscalls
	./test-syscalls && ./setup.sh && ./test_with_signals.pl

clean:
	rm -f $(BIN)
