#include <jamaica.h>
#include <stdlib.h>
#include "toplevel.h"
#include <juniperoperations.h>
#include <fpgaporting.h>

#define VERSION 25

jamaica_thread __juniper_thread;
int __juniper_args[ARGS_MAX];

//Memory interfaces
volatile int *__juniper_ram_master;
volatile char *__juniper_ram_master_char;
volatile short *__juniper_ram_master_short;

int hls(int *opid, int *arg1, int *arg2, int *arg3) {

	/*
	 * Bundle the different memory interfaces together into the same AXI Master interface
	 * This uses slave offset mode, which allows each interface to be separately offset.
	 * This is not what we want, but HLS only supports AXI master bundling if offsetting
	 * is also used.
	 */
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_char bundle=MAXI offset=slave
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_short bundle=MAXI offset=slave

	/*
	 * Place all control logic (and the offsets for the memory interfaces) on an AXI slave
	 * interface.
	 */
#pragma HLS INTERFACE s_axilite port=opid bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=arg1 bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=arg2 bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=arg3 bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=return bundle=AXILiteS register

	//Set up dummy __juniper_thread struct
	create_jamaica_thread();

	switch(*opid) {
	case OP_VERSION: return VERSION;

	/*
	 * Read memory location [slaveb]
	 * Because the memory interfaces are types, the address is interpreted as an
	 * index for the appropriate type, so the host should divide the raw addr by
	 * 4 to read ints, or 2 for shorts.
	 *
	 * For fabricating Jamaica references on the host the following is correct:
	 * val = JAMAICA_BLOCK_GET_DATA32((jamaica_ref) (0x80001000/4), 0);
	 * because jamaica_refs are integer-indexed pointers.
	 */
	case OP_PEEK_8: return __juniper_ram_master_char[*arg1];
	case OP_PEEK_16: return __juniper_ram_master_short[*arg1];
	case OP_PEEK_32: return __juniper_ram_master[*arg1];

	//Write arg2 to memory[arg1]
	case OP_POKE_8:
		__juniper_ram_master_char[*arg1] = *arg2;
		return 0;
	case OP_POKE_16:
		__juniper_ram_master_short[*arg1] = *arg2;
		return 0;
	case OP_POKE_32:
		__juniper_ram_master[*arg1] = *arg2;
		return 0;
#ifdef JUNIPER_SUPPORT_FLOATS
	case OP_POKE_F: {
		int temp = *arg2;
		float f = *(float*)&temp;
		f = f + 0.1f;
		__juniper_ram_master[*arg1] = *(int *)&f;
	}
#endif

	//Set argument[arg1] to arg2
	case OP_WRITE_ARG:
		if(*arg1 >= 0 && *arg1 < ARGS_MAX) {
			__juniper_args[*arg1] = *arg2;
		}
		return 0;

	//Call method ID arg1
	case OP_CALL:
		return __juniper_call(*arg1);

	//Test functions
	case OP_TEST_ARRAY_LEN:
		return JAMAICA_GET_ARRAY_LENGTH(*arg1);

	case OP_TEST_ARRAY_SUM: {
			int len = JAMAICA_GET_ARRAY_LENGTH(*arg1);
			int total = 0;
			for(int i = 0; i < len; i++) {
				total += jamaicaGC_GetArray32(*arg1, i);
			}
			return total;
		}
	}

	return 0;
}

