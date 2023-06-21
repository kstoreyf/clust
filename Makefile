#kdlib = kdtree-0.5.6/libkdtree.a

CC = gcc
LDFLAGS = $(kdlib) -lm
ODIR = obj
IDIR = include
HOMEDIR = .
CFLAGS = -std=c89 -pedantic -Wall -g -Wformat -I$(IDIR) -nostartfiles

_OBJ = run_statistics_mock.o run_statistics_aemulus.o
OBJ = $(patsubst %, $(ODIR)/%,$(_OBJ))

_DEPS_HOME = utils.h upf_code/upf.h marks.h
DEPS = $(patsubst %, $(HOMEDIR)/%,$(_DEPS_HOME))

_DEPS = kdtree/kdtree.h kdtree/kdtree_periodic.h kdtree/nrutil.h
DEPS += $(patsubst %, $(IDIR)/%,$(_DEPS))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: all run_statistics_mock run_statistics_aemulus
all: run_statistics_mock run_statistics_aemulus

run_statistics_mock:
	gcc -lm -lpthread -DUSE_LIST_NODE_ALLOCATOR -Wall -std=c89 -o run_statistics_mock run_statistics_mock.c utils.c upf_code/upf.c marks.c kdtree/kdtree.c kdtree/kdtree_periodic.c

run_statistics_aemulus:
	gcc -lm -lpthread -DUSE_LIST_NODE_ALLOCATOR -Wall -std=c89 -o run_statistics_aemulus run_statistics_aemulus.c utils.c upf_code/upf.c marks.c kdtree/kdtree.c kdtree/kdtree_periodic.c

.PHONY: clean
clean:
	rm -f run_statistics_mock run_statistics_aemulus
