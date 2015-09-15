#define _GNU_SOURCE
#include <stdio.h>
#include <stddef.h>
#include <dlfcn.h>
#include <malloc.h>

#include <sys/mman.h>
#include <fcntl.h>

#undef sbrk

//#define DBG(...) fprintf(stderr, __VA_ARGS__)
#define DBG(...)

// We can't pull in malloc.h without screwing stuff up.
// This is the linkage to morecore() to override it later.
// Note that this is glibc specific
extern void* (*__morecore)(ptrdiff_t sz);
//extern void* stderr;

int inited = 0;
void* (*inner_malloc)(size_t size) = NULL;
void* (*inner_calloc)(size_t num, size_t size) = NULL;
void (*_inner_free)(void* ptr) = NULL;
void* (*inner_mmap)(void* addr, size_t length, int prot, int flags, int fd, off_t offset) = NULL;

void init_preloader();

#define MALLOC_STORAGE_SIZE (1024 * 256 * 1024)

//char malloc_storage2[MALLOC_STORAGE_SIZE] __attribute__((aligned(4096)));
unsigned int used_malloc_storage = 0;
unsigned int used_malloc_storage_top = 0;

char* malloc_storage = NULL;
char malloc_supertemp_storage[1024];

#define STORAGE malloc_storage
//#define STORAGE_PATH "/sys/bus/pci/devices/0000:02:00.0/resource0"
#define STORAGE_PATH "/dev/mtdblock0"

void* override_morecore(ptrdiff_t sz)
{
	static unsigned int total_morecore = 0;
	int i = 0;

	DBG("malloc_preload:override_morecore: start\n");

	if((used_malloc_storage + sz) > MALLOC_STORAGE_SIZE - used_malloc_storage_top)
	{
		DBG("malloc_preload:override_morecore: out of storage\n");
		return (void*)0;
	}
	total_morecore += sz;
	DBG("malloc_preload:override_morecore: Morecore %d total: %d 0x%x\n", sz, total_morecore, &STORAGE[used_malloc_storage]);

	for(i = 0; i < sz; i++)
		STORAGE[used_malloc_storage + i] = 0;

	used_malloc_storage += sz;

	return &STORAGE[used_malloc_storage - sz];
}

void __attribute__((constructor)) setup_preloader()
{
	__morecore = &override_morecore;
	init_preloader();
	int fd = open(STORAGE_PATH, O_RDWR | O_SYNC);
	if(fd == -1)
		DBG("malloc_preload:setup_preloader: Failed to open " STORAGE_PATH "\n");
	malloc_storage = inner_mmap(0, MALLOC_STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(malloc_storage == (void*)-1)
		DBG("malloc_preload:setup_preloader: Failed to mmap...\n");

	DBG("malloc_preload:setup_preloader: Overridden morecore\n");
}

void init_preloader()
{
	int i = 0;
	inited = 1;

	if(inner_mmap == NULL)
	{
		inner_mmap = dlsym(RTLD_NEXT, "mmap");
		if(inner_mmap == NULL)
			DBG("malloc_preload:init_preloader: Failed to dlsym next-mmap\n");
	}

	if(_inner_free == NULL)
	{
		_inner_free = dlsym(RTLD_NEXT, "free");
		if(_inner_free == NULL)
			DBG("malloc_preload:init_preloader: Failed to dlsym next-free\n");
	}

	if(inner_malloc == NULL)
	{
		inner_malloc = dlsym(RTLD_NEXT, "malloc");
		if(inner_malloc == NULL)
			DBG("malloc_preload:init_preloader: Failed to dlsym next-malloc\n");
	}

	if(inner_calloc == NULL)
	{
		inner_calloc = dlsym(RTLD_NEXT, "calloc");
		if(inner_calloc == NULL)
			DBG("malloc_preload:init_preloader: Failed to dlsym next-calloc\n");
	}
}

void* malloc(size_t size)
{
	static int malloc_max = 0;

	if(!inited)
		init_preloader();

	malloc_max += size;
	DBG("malloc_preload:malloc: = %d max = %d base=0x%x supertemp=0x%x\n", size, malloc_max, malloc_storage, malloc_supertemp_storage);
	//DBG("malloc_preload:malloc: = 0x%x, calloc = 0x%x, free = 0x%x, mmap = 0x%x\n", inner_malloc, inner_calloc, _inner_free, inner_mmap);

	void* retVal = NULL;

	// Do we want the address?
	if(size == 0)
	{
		DBG("malloc_preload:malloc: zero\n");
		return STORAGE;
	}

	retVal = inner_malloc(size);
	DBG("malloc_preload:malloc: retval = 0x%x\n", retVal);
	return retVal;
}

void* calloc(size_t num, size_t size)
{
	int i = 0;
	if(!inited)
		init_preloader();

	DBG("malloc_preload:calloc: %d %d\n", num, size);

	if(inited && malloc_storage == NULL)
	{
		// This is (annoyingly) needed.
		// dlsym requires calloc.
		// Our calloc override requires mmap before it is initialised.
		// In order to get a handle on the (actual) mmap, we need dlsym.
		// To break the circular dependency, we need this basic temp allocator.
		// This temp allocator allocates some space for dlsym so we can get mmap.
		// It's a hack, but it works.
		for(i = 0; i < 1024; i++)
			malloc_supertemp_storage[i] = 0;
		return malloc_supertemp_storage;
	}

	void* foo = inner_calloc(num, size);
	return foo;
}

void free(void* ptr)
{
	if(!inited)
		init_preloader();

	if(ptr == malloc_supertemp_storage)
	{
		return;
	}

	if(ptr == STORAGE)
		return;
	else
		_inner_free(ptr);
}

void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	if(!inited)
		init_preloader();

	void* rv = inner_mmap(addr, length, prot, flags, fd, offset);
	return malloc(length);

	/*unsigned int topAddr = (unsigned int)STORAGE;
	topAddr += MALLOC_STORAGE_SIZE;
	topAddr -= used_malloc_storage_top;
	DBG("MMAP topAddr: 0x%x\n", topAddr);
	topAddr -= length;

	// How much extra to page align it?
	unsigned int pageMask = 4096 - 1;
	unsigned int extra = topAddr & pageMask;
	topAddr -= extra;
	DBG("Extra bytes: 0x%x, New top: 0x%x, Old used: 0x%x\n", extra, topAddr, used_malloc_storage_top);
	used_malloc_storage_top += length;
	used_malloc_storage_top += extra;
	DBG("NEW USED: 0x%x\n", used_malloc_storage_top);

	DBG("malloc_preload:mmap: addr: 0x%x length %d prot 0x%x flags 0x%x fd %d offset 0x%x rv 0x%x, top 0x%x\n", addr, length, prot, flags, fd, offset, topAddr, topAddr + length);

	//mmap returns zero-filled pages
	memset((void*) topAddr, 0, length);

	return (void*) topAddr;*/
}
