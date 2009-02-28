# makefile -- to compile our testing code
#
# Team 2: T. Bertin-Mahieux, M. Sukan, B. Warfield
#   W4118: Operating Systems HW2



TREE_TAG=.hmwk3

test-barrier: barrier_test.c
	gcc -static -I /usr/src/linux-2.6.11.12$(TREE_TAG) -o $@ $^

test: test-barrier
	./$^

