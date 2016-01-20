#define _GNU_SOURCE
#include <stdio.h>
#include <stddef.h>
#include <dlfcn.h>
#include <malloc.h>
#include <string.h>

#include <sys/mman.h>
#include <fcntl.h>

#undef sbrk

//#define DBG(...) fprintf(stderr, __VA_ARGS__)
#define DBG(...)

int inited = 0;
void *fpga_memory = 0;
int mmapfd = -1;

void init_preloader();
void abort(void);

void* (*inner_mmap)(void* addr, size_t length, int prot, int flags, int fd, off_t offset) = NULL;

void __attribute__((constructor)) setup_preloader() {
	init_preloader();
}

void get_storage();



void init_preloader() {
	int i = 0;
	inited = 1;

	DBG("[JUNIPER] Initialising preloader\n");

	if(inner_mmap == NULL) {
		inner_mmap = dlsym(RTLD_NEXT, "mmap");
		if(inner_mmap == NULL) {
			DBG("[JUNIPER] Failed to dlsym next-mmap\n");
			abort();
		}
	}

	get_storage();
}

void* mmap(void* addr, size_t length, int prot, int flags, int fd_ignored, off_t offset) {
	if(!inited) init_preloader();

	DBG("[JUNIPER] Call mmap(%p, 0x%x, %d, %d, %d, 0x%x)\n", addr, length, prot, flags, fd_ignored, offset);

	//By attempting to mmap address -1, the application can request the FPGA's base phyiscal address.
	if((int) addr == ~0) {
		if(fpga_memory == 0) {
			DBG("[MMAP] Requested Jamaica heap base address but Jamaica hasn't allocated yet. Aborting.\n");
			abort();
		}
		return fpga_memory;
	}

	//Jamaica relies on its heap being zeroed so we memset it (as per the specification)
	DBG("[JUNIPER] Clearing region...\n");
	memset(fpga_memory, 0, length);
	DBG("[JUNIPER] Cleared region.\n");

	return fpga_memory;
}


void get_storage() {
	mmapfd = open(STORAGE_PATH, O_RDWR | O_SYNC);
	if(mmapfd == -1) {
		DBG("[JUNIPER] Failed to open " STORAGE_PATH "\n");
		abort();
	}

	DBG("[JUNIPER] Opened %s\n", STORAGE_PATH);

	fpga_memory = inner_mmap(0, STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mmapfd, 0);
	if(fpga_memory == (void*)-1) {
		DBG("[JUNIPER] Failed to mmap...\n");
		abort();
	}
}

