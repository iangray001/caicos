#include <juniper_fpga_interface.h>
#include <juniperoperations.h>
#include <stdlib.h>
#include <stdio.h>
#include "jamaica.h"
#include "jni.h"
#include "jbi.h"
#include "Main__.h"
#include <sys/mman.h>
#include "caicos_debug.h"
#include <sys/time.h>

struct timeval timing_start;
struct timeval timing_end;

long *samples = NULL;
int num_samples = 0;
int current_sample = 0;

void caicos_timing_start(int size) {
	if(num_samples == 0) {
		void *mem = malloc(sizeof(long) * size);
		if(mem == NULL) {
			printf("[CAICOS] Failed to allocate memory for timing samples.");
		}
		samples = (long *) mem;
		num_samples = size;
		current_sample = 0;
	}

	gettimeofday(&timing_start, NULL);
}

void caicos_timing_end() {
	gettimeofday(&timing_end, NULL);
	unsigned long usec = (timing_end.tv_usec + (timing_end.tv_sec * 1000000)) - (timing_start.tv_usec + (timing_start.tv_sec * 1000000));

	if(current_sample < num_samples) {
		samples[current_sample] = usec;
		current_sample++;

		if(current_sample >= num_samples) {
			int i;
			printf("Timing results:\n");
			for(i = 0; i < num_samples; i++) {
				printf("%lu\n", samples[i]);
			}
		}
	}
}


int caicos_docall(int op, int argc, ...) {
        int rv, i;
        va_list argv;
        juniper_fpga_partition_set_arg(0, 0, 0, op);
        va_start(argv, argc);
        for(i = 0; i < argc; i++) {
                int a = va_arg(argv, int);
                juniper_fpga_partition_set_arg(0, 0, i+1, a);
        }
        va_end(argv);
        juniper_fpga_partition_start(0, 0);
        while(!juniper_fpga_partition_idle(0, 0));
        juniper_fpga_partition_get_retval(0, 0, &rv);
        return rv;
}

void caicos_mdump(volatile void *mem, int count) {
        int i;
        for(i = 0; i < count; i++) {
                printf("0x%.8X ", ((int *)mem)[i]);
                if(i && !(i%8)) printf("\n");
        }
        printf("\n");
}

void caicos_check(int a, int b) {
        static int call = 0;
        if(a != b) {
                printf("ERROR (%d): %d is not equal to %d!\n", call, a, b);
        }
        call++;
}

TEST_FUNCTION(caicos_test8, char, OP_PEEK_8, OP_POKE_8, 1)
TEST_FUNCTION(caicos_test16, short, OP_PEEK_16, OP_POKE_16, 2)
TEST_FUNCTION(caicos_test32, int, OP_PEEK_32, OP_POKE_32, 4)


