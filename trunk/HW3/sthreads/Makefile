#
# Makefile for SThreads
#

CC=gcc
CFLAGS=-g -O2 -Wall -std=c99 -pedantic
LDFLAGS=-g

.c.s:
	$(CC) $(CFLAGS) $(CPPFLAGS) -S $<

test-sthread: test.o libsthread.a Makefile
	$(CC) $(LDFLAGS) -o $@ test.o -L. -lsthread

libsthread.a: sthread.o sync.o
	$(AR) cr $@ $^


test: test-sthread
	./$^

clean:
	$(RM) libsthread.a test-sthread *.o *~
