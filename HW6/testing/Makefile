# makefile -- to compile our testing code
#
# Team 2: T. Bertin-Mahieux, M. Sukan, B. Warfield
#   W4118: Operating Systems HW6


CFLAGS= -std=c99 -Wall -pedantic -g
TREE_TAG=-hwk6
BIN=tag
EASY_COMPILE=$(CC) $(CFLAGS) -o $@ $^ 
SYSCALL_COMPILE=$(CC) $(CFLAGS) -I /usr/src/linux-2.6.11.12$(TREE_TAG)/include -o $@ $^

all: $(BIN)

tag: tag.c
	$(SYSCALL_COMPILE)

test: $(BIN)
	echo "no tests defined yet"

clean:
	rm -f $(BIN)
