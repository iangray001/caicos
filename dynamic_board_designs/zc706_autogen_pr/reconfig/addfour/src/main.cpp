#include <stdlib.h>
#include <string.h>

void hls(int* mem)
{
#pragma HLS INTERFACE m_axi port=mem
#pragma HLS INTERFACE s_axilite port=return
	int tmp[2];

	int foo = mem[0];
	foo += 4;
	mem[1] = foo;
}
