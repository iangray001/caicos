include ../Makefile.inc

CFLAGS=-fPIC -DSTORAGE_PATH="\"$(STORAGE_PATH)\"" -DSTORAGE_SIZE="$(STORAGE_SIZE)"
LDFLAGS=-shared
OBJS=malloc_preload.o

all: malloc_preload.so

malloc_preload.so: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	@echo [CC] $@
	@$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -rf malloc_preload.so $(OBJS) *~
