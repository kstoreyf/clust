kdlib = kdtree-0.5.6/libkdtree.a

CC = gcc
CFLAGS = -std=c89 -pedantic -Wall -g -I..
LDFLAGS = $(kdlib) -lm

.PHONY: all
all: test

test: test.c $(kdlib)

.PHONY: clean
clean:
	rm -f test
