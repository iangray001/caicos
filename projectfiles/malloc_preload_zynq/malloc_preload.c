#define _GNU_SOURCE
#include <stdio.h>
#include <stddef.h>
#include <dlfcn.h>
#include <malloc.h>
#include <string.h>

#include <sys/mman.h>
#include <fcntl.h>

#undef sbrk
void abort(void);

//#define DBG(...) fprintf(stderr, __VA_ARGS__)
#define DBG(...)

int inited = 0;
void* (*inner_mmap)(void* addr, size_t length, int prot, int flags, int fd, off_t offset) = NULL;
void* last_mmap_result = 0;

void init_preloader();

#define MALLOC_STORAGE_SIZE (1024 * 256 * 1024)

#define STORAGE_PATH "/dev/uio0"

void __attribute__((constructor)) setup_preloader()
		{
	init_preloader();
		}

void init_preloader()
{
	int i = 0;
	inited = 1;

	if(inner_mmap == NULL)
	{
		inner_mmap = dlsym(RTLD_NEXT, "mmap");
		if(inner_mmap == NULL)
		{
			DBG("malloc_preload:init_preloader: Failed to dlsym next-mmap\n");
			abort();
		}
	}
}

void* mmap(void* addr, size_t length, int prot, int flags, int fd_ignored, off_t offset)
{
	if(!inited)
		init_preloader();

	DBG("MMAP: %p 0x%x, %d, %d, %d, 0x%x\n", addr, length, prot, flags, fd_ignored, offset);

	/*
	 * By attempting to mmap address -1, the source application can request the FPGA's base phyiscal address.
	 * This must be done after Jamaica has actually mmapped its heap.
	 */
	if((int) addr == ~0) {
		if(last_mmap_result == 0) {
			DBG("MMAP: Requested Jamaica heap base address but Jamaica hasn't allocated yet. Aborting.\n");
			abort();
		}
		return last_mmap_result;
	}

	/*
	 * Only the first mmap is remapped onto the FPGA. This may be expandable, but it is not necessary for
	 * Java-based systems in which the entire heap is mapped first.
	 */
	if(last_mmap_result != 0) {
		DBG("MMAP: mmap has already been called once - can't redirect again to the FPGA, aborting.\n");
		abort();
	}

	int fd = open(STORAGE_PATH, O_RDWR | O_SYNC);
	if(fd == -1)
	{
		DBG("malloc_preload:setup_preloader: Failed to open " STORAGE_PATH "\n");
		return (void*)-1;
	}

	void* storage = inner_mmap(0, MALLOC_STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(storage == (void*)-1)
	{
		DBG("malloc_preload:setup_preloader: Failed to mmap...\n");
		return (void*)-1;
	}

	last_mmap_result = storage;

	DBG("MMAP: Clearing region...\n");

	/*
	 * It appears that Jamaica relies on its heap being zeroed so we memset it as per the mmap specification.
	 */
	memset(storage, 0, length);
	DBG("MMAP: Cleared region.\n");

	return storage;
}
