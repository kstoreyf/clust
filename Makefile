#kdlib = kdtree-0.5.6/libkdtree.a


CC = gcc
LDFLAGS = $(kdlib) -lm
ODIR = obj
IDIR = include
#LDIR = ../libC_main
#CFLAGS = -std=c89 -pedantic -Wall -g -I.. -Wformat -I$(IDIR)
#CFLAGS = -std=c89 -pedantic -Wall -g -Wformat -I$(IDIR) -L$(LDIR)
CFLAGS = -std=c89 -pedantic -Wall -g -Wformat -I$(IDIR) -nostartfiles

#LIBS = -lC_main -lm

_OBJ = test.o upf.o
OBJ = $(patsubst %, $(ODIR)/%,$(_OBJ))

_DEPS = kdtree.h kdtree_periodic.h nrutil.h
DEPS = $(patsubst %, $(IDIR)/%,$(_DEPS))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: all
all: test upf

#test: test.c include/nrutil.h $(kdlib)
#test: test.c $(OBJ)
#	$(CC) -o $@ $^ $(CFLAGS) $(kdlib)

#upf: upf.c $(kdlib)
#upf: upf.c $(OBJ)
#	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

test: test.c $(kdlib)
test:
	#gcc -o test test.c kdtree.c
	gcc -lm -lpthread -DUSE_LIST_NODE_ALLOCATOR -Wall -std=c89 -o test test.c kdtree.c kdtree_periodic.c

upf:
	gcc -lm -lpthread -DUSE_LIST_NODE_ALLOCATOR -Wall -std=c89 -o upf upf.c kdtree.c kdtree_periodic.c

.PHONY: clean
clean:
	rm -f test upf
