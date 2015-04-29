#define _GNU_SOURCE
#include <stdio.h>
#include <stddef.h>
#include <dlfcn.h>

#include <sys/mman.h>
#include <fcntl.h>

// We can't pull in malloc.h without screwing stuff up.
// This is the linkage to morecore() to override it later.
// Note that this is glibc specific, unless it wasn't obvious :P
extern void* (*__morecore)(ptrdiff_t sz);

void* (*inner_malloc)(size_t size) = NULL;
//void (*_inner_free)(void* ptr) = NULL;

// 4MB
#define MALLOC_STORAGE_SIZE (1024 * 4 * 1024)
//char malloc_storage[MALLOC_STORAGE_SIZE];
int used_malloc_storage = 0;

char* malloc_storage = NULL;

void* override_morecore(ptrdiff_t sz)
{
    fprintf(stderr, "Calling morecore. Requesting %ld bytes\n", sz);

    if((used_malloc_storage + sz) > MALLOC_STORAGE_SIZE)
        return (void*)-1;
    
    used_malloc_storage += sz;

    fprintf(stderr, "Ptr: 0x%x\n", &malloc_storage[used_malloc_storage - sz]);

    return &malloc_storage[used_malloc_storage - sz];
}

void* malloc(size_t size)
{
    if(inner_malloc == NULL)
    {
        // Find it!
        inner_malloc = dlsym(RTLD_NEXT, "malloc");
        if(inner_malloc == NULL)
            fprintf(stderr, "Failed to dlsym next-malloc");

        // Now redirect morecore()
	// Try and mmap the PCIe
	int fd = open("/dev/uio0", O_RDWR | O_SYNC);
	if(!fd)
	  fprintf(stderr, "Failed to open /dev/uio0\n");
	malloc_storage = mmap(0, MALLOC_STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(!malloc_storage)
	  fprintf(stderr, "Failed to mmap...\n");
	
        __morecore = override_morecore;
    }

    fprintf(stderr, "Allocating %d bytes\n", size);

    return inner_malloc(size);
}

/*void free(void* ptr)
{
//    fprintf(stderr, "Attempting to free from 0x%x\n", ptr);
    if(_inner_free == NULL)
    {
        _inner_free = dlsym(RTLD_NEXT, "free");
        if(_inner_free == NULL)
            fprintf(stderr, "Failed to dlsym next-free");
    }

    //  fprintf(stderr, "Calling free from 0x%x\n", ptr);
    _inner_free(ptr);
    }*/

