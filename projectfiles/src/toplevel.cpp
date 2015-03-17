#include <jamaica.h>
#include <stdlib.h>
#include "toplevel.h"
#include <juniperoperations.h>

#define VERSION 20

jamaica_thread __juniper_thread;
volatile int *__juniper_ram_master;
int __juniper_args[ARGS_MAX];



int hls(volatile int *master, int *slavea, int *slaveb, int *slavec, int *slaved) {
#pragma HLS INTERFACE s_axilite port=slavea bundle=BUS_A register
#pragma HLS INTERFACE s_axilite port=slaveb bundle=BUS_A register
#pragma HLS INTERFACE s_axilite port=slavec bundle=BUS_A register
#pragma HLS INTERFACE s_axilite port=slaved bundle=BUS_A register
#pragma HLS INTERFACE s_axilite port=return bundle=BUS_A register
#pragma HLS INTERFACE m_axi port=master

	__juniper_ram_master = master;
	create_jamaica_thread(&__juniper_thread);

	switch(*slavea) {
	case OP_VERSION:
		return VERSION;

	case OP_PEEK:
		//Read memory location slaveb. Address is an int address (i.e. divide raw address by 4)
		return master[*slaveb];

	case OP_POKE:
		//Write slavec to memory[slaveb]
		master[*slaveb] = *slavec;
		return 0;

	case OP_WRITE_ARG:
		//Set argument[slaveb] to slavec
		if(*slaveb >= 0 && *slaveb < ARGS_MAX) {
			__juniper_args[*slaveb] = *slavec;
		}
		return 0;

	case OP_CALL:
		//Call method ID slaveb
		int rv = __juniper_call(*slaveb);
		break;
	}

	//*slaveb = master[0x80000000/4];
	//*slavec = JAMAICA_BLOCK_GET_DATA32((jamaica_ref) (0x80001000), 0); //Wrong
	//*slaved = JAMAICA_BLOCK_GET_DATA32((jamaica_ref) (0x80001000/4), 0); //Correct

	//master[0x80000000/4] = 1234; //Correct
	//master[0x80000000] = 5678; //Wrong

	return 0;
}

