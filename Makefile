#kdlib = kdtree-0.5.6/libkdtree.a

CC = gcc
LDFLAGS = $(kdlib) -lm
ODIR = obj
IDIR = include
HOMEDIR = .
CFLAGS = -std=c89 -pedantic -Wall -g -Wformat -I$(IDIR) -nostartfiles

_OBJ = run_statistics_mock.o
OBJ = $(patsubst %, $(ODIR)/%,$(_OBJ))

_DEPS_HOME = utils.h upf.h marks.h
DEPS = $(patsubst %, $(HOMEDIR)/%,$(_DEPS_HOME))

_DEPS = kdtree.h kdtree_periodic.h nrutil.h
DEPS += $(patsubst %, $(IDIR)/%,$(_DEPS))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: all run_statistics_mock
all: run_statistics_mock

run_statistics_mock:
	gcc -lm -lpthread -DUSE_LIST_NODE_ALLOCATOR -Wall -std=c89 -o run_statistics_mock run_statistics_mock.c utils.c upf.c marks.c kdtree.c kdtree_periodic.c

.PHONY: clean
clean:
	rm -f run_statistics_mock
