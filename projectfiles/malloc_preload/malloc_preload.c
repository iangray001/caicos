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
// Note that this is glibc specific, unless it wasn't obvious :P
extern void* (*__morecore)(ptrdiff_t sz);
//extern void* stderr;

int inited = 0;
void* (*inner_malloc)(size_t size) = NULL;
void* (*inner_calloc)(size_t num, size_t size) = NULL;
void (*_inner_free)(void* ptr) = NULL;
void* (*inner_mmap)(void* addr, size_t length, int prot, int flags, int fd, off_t offset) = NULL;

void init_preloader();

// 1MB
#define MALLOC_STORAGE_SIZE (1024 * 512 * 1024)

char malloc_storage2[MALLOC_STORAGE_SIZE] __attribute__((aligned(4096)));
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

    DBG("MORECORE!\n");
    
    if((used_malloc_storage + sz) > MALLOC_STORAGE_SIZE - used_malloc_storage_top)
      {
	       DBG("OUT\n");
	
        return (void*)0;
      }
    total_morecore += sz;
    DBG("Morecore %d total: %d 0x%x\n", sz, total_morecore, &STORAGE[used_malloc_storage]);

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
    DBG("Failed to open " STORAGE_PATH "\n");
  malloc_storage = inner_mmap(0, MALLOC_STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(malloc_storage == (void*)-1)
    DBG("Failed to mmap...\n");
  
  DBG("Overridden morecore!\n");
}

void init_preloader()
{
  int i = 0;
  inited = 1;

  DBG("1\n");

  DBG("1.1\n");

  if(inner_mmap == NULL)
    {
      inner_mmap = dlsym(RTLD_NEXT, "mmap");
      if(inner_mmap == NULL)
	DBG("Failed to dlsym next-mmap\n");
    }

  DBG("2\n");

  if(_inner_free == NULL)
    {
      _inner_free = dlsym(RTLD_NEXT, "free");
      if(_inner_free == NULL)
	DBG("Failed to dlsym next-free\n");
    }

  DBG("2.1\n");

  if(inner_malloc == NULL)
    {
      // Find it!
      inner_malloc = dlsym(RTLD_NEXT, "malloc");
      if(inner_malloc == NULL)
	DBG("Failed to dlsym next-malloc\n");
      
      // Now redirect morecore()
      // Try and mmap the PCIe
      // Er...moved above!
    }
  
  DBG("3\n");
  
  if(inner_calloc == NULL)
    {
      inner_calloc = dlsym(RTLD_NEXT, "calloc");
      if(inner_calloc == NULL)
	DBG("Failed to dlsym next-calloc\n");
    }

  DBG("4\n");
  
  DBG("TEST\n");
  DBG("DONE\n");
}

void* malloc(size_t size)
{
  static int malloc_max = 0;
  
  if(!inited)
    init_preloader();

  malloc_max += size;
  DBG("@@ malloc = %d max = %d base=0x%x supertemp=0x%x\n", size, malloc_max, malloc_storage, malloc_supertemp_storage);
  //  DBG("@@@ malloc = 0x%x, calloc = 0x%x, free = 0x%x, mmap = 0x%x\n", inner_malloc, inner_calloc, _inner_free, inner_mmap);

  void* retVal = NULL;

  // Do we want the address?
  if(size == 0)
    {
      DBG("@@ malloc zero\n");
      return STORAGE;
    }
  
  retVal = inner_malloc(size);
  DBG("@@@ malloc retval = 0x%x\n", retVal);
  return retVal;
  }

void* calloc(size_t num, size_t size)
{
  int i = 0;
  if(!inited)
    init_preloader();

  if(inited && malloc_storage == NULL)
    {
      DBG("ST\n");
      // This is (annoyingly) needed.
      // dlsym requires calloc.
      // Out calloc override requires mmap before it is initialised.
      // In order to get a handle on the (actual) mmap, we need dlsym.
      // To break the circular dependency, we need this (pretty ghetto) temp allocator.
      // This temp allocator allocates some space for dlsym so we can get mmap.
      // It's a hack, but, eh, it works.
      for(i = 0; i < 1024; i++)
      	malloc_supertemp_storage[i] = 0;
      return malloc_supertemp_storage;
    }

  void* foo = inner_calloc(num, size);
  DBG("@@ calloc %d %d\n", num, size);
  return foo;
}

void free(void* ptr)
{
  if(!inited)
    init_preloader();

  if(ptr == malloc_supertemp_storage)
    {
      DBG("HURR\n");
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
  
  //void* rv = inner_mmap(addr, length, prot, flags, fd, offset);

  // Return from the top? :D
  unsigned int topAddr = (unsigned int)STORAGE;
  topAddr += MALLOC_STORAGE_SIZE;
  topAddr -= used_malloc_storage_top;
  topAddr -= length;

  // How much extra to page align it?
  unsigned int pageMask = 4096 - 1;
  unsigned int extra = topAddr & pageMask;
  topAddr -= extra;

  used_malloc_storage_top += length;
  used_malloc_storage_top += extra;

  DBG("I MMAPED! addr: 0x%x length %d prot 0x%x flags 0x%x fd %d offset 0x%x rv 0x%x, top 0x%x\n", addr, length, prot, flags, fd, offset, topAddr, topAddr + length);

  return (void*)topAddr;
}
