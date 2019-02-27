#kdlib = kdtree-0.5.6/libkdtree.a

CC = gcc
CFLAGS = -std=c89 -pedantic -Wall -g -I..
LDFLAGS = $(kdlib) -lm

.PHONY: all
all: test upf

#test: test.c $(kdlib)
test:
	#gcc -o test test.c kdtree.c 
	gcc -lm -lpthread -DUSE_LIST_NODE_ALLOCATOR -Wall -std=c89 -o test test.c kdtree.c kdtree_periodic.c

upf:
	  gcc -lm -lpthread -DUSE_LIST_NODE_ALLOCATOR -Wall -std=c89 -o upf upf.c kdtree.c kdtree_periodic.c

.PHONY: clean
clean:
	rm -f test upf
