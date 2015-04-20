CC=gcc
LD=gcc

CFLAGS=-m32 -fPIC
TESTCFLAGS=-m32
LDFLAGS=-m32 -shared -fPIC -ldl
TESTLDFLAGS=-m32

OBJS=malloc_preload.o

all: malloc_preload.so test

test: test.o
	$(LD) $(TESTLDFLAGS) -o $@ $^

test.o: test.c
	$(CC) $(TESTCFLAGS) -c -o $@ $<

malloc_preload.so: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -rf malloc_preload.so test test.o $(OBJS) *~
