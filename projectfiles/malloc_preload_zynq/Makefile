CC=gcc
LD=gcc

CFLAGS=-fPIC
LDFLAGS=-shared -fPIC -ldl

OBJS=malloc_preload.o

all: malloc_preload.so

malloc_preload.so: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -rf malloc_preload.so $(OBJS) *~
