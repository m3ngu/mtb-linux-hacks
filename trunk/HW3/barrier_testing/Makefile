# makefile -- to compile our testing code
#
# Team 2: T. Bertin-Mahieux, M. Sukan, B. Warfield
#   W4118: Operating Systems HW2


CFLAGS= -std=c99 -Wall -pedantic -g
TREE_TAG=-hwk3

test-barrier: barrier_test.c
	$(CC) $(CFLAGS) -static -I /usr/src/linux-2.6.11.12$(TREE_TAG)/include -o $@ $^ ../sthreads/sthread.o

test: test-barrier
	./$^

