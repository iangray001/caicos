#include <stdlib.h>
#include <string.h>

void hls(int* mem)
{
#pragma HLS INTERFACE m_axi port=mem
#pragma HLS INTERFACE s_axilite port=return
    *mem = 0;
}
