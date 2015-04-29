CCPATH=/home/jamie/juniper/zc706_linux_buildroot/output/host/usr/bin/
CC=$(CCPATH)arm-buildroot-linux-gnueabi-gcc
LD=$(CCPATH)arm-buildroot-linux-gnueabi-gcc

CFLAGS=-fPIC
TESTCFLAGS=
LDFLAGS=-shared -fPIC -ldl
TESTLDFLAGS=

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
