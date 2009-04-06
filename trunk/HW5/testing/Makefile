# makefile -- to compile our testing code
#
# Team 2: T. Bertin-Mahieux, M. Sukan, B. Warfield
#   W4118: Operating Systems HW5


CFLAGS= -std=c99 -Wall -pedantic -g
TREE_TAG=-hwk5
BIN=swap-central
EASY_COMPILE=$(CC) $(CFLAGS) -o $@ $^ 
SYSCALL_COMPILE=$(CC) $(CFLAGS) -I /usr/src/linux-2.6.11.12$(TREE_TAG)/include -o $@ $^

all: $(BIN)

swap-central: swap_central.c
	$(EASY_COMPILE)


test: $(BIN)
	echo "no tests defined yet"

clean:
	rm -f $(BIN)
