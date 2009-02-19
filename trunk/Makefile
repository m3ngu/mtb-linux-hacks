# makefile -- to compile our testing code
#
# Team 2: T. Bertin-Mahieux, M. Sukan, B. Warfield
#   W4118: Operating Systems HW2



HEADERS=/usr/src/linux-2.6.11.12-hwk2/include

test-fail: test_fail.c
	gcc -static -I $(HEADERS) -o test-fail test_fail.c

test: test-fail
	./test-fail

