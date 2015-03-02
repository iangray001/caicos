#include <jamaica.h>
#include <stdlib.h>

#include "fpgaporting.h"
#include "generatedcode.h"


jamaica_thread t;
volatile jamaica_data32 *__juniper_ram_master;

#define RAM_BASE_INT (0x80000000/4)

void hls(volatile jamaica_data32 *master, int *slavea, int *slaveb, int *slavec, int *slaved) {
//void hls(int *master, int *slavea, int *slaveb, int *slavec, int *slaved) {
#pragma HLS RESOURCE variable=master core=AXI4M
#pragma HLS RESOURCE core=AXI4LiteS metadata="-bus_bundle slv0" variable=slavea
#pragma HLS RESOURCE core=AXI4LiteS metadata="-bus_bundle slv0" variable=slaveb
#pragma HLS RESOURCE core=AXI4LiteS metadata="-bus_bundle slv0" variable=slavec
#pragma HLS RESOURCE core=AXI4LiteS metadata="-bus_bundle slv0" variable=slaved
#pragma HLS RESOURCE core=AXI4LiteS metadata="-bus_bundle slv0" variable=return
#pragma HLS INTERFACE ap_none port=slavea register
#pragma HLS INTERFACE ap_none port=slaveb register
#pragma HLS INTERFACE ap_none port=slavec register
#pragma HLS INTERFACE ap_none port=slaved register
#pragma HLS INTERFACE ap_bus port=master

	__juniper_ram_master = master;
	create_jamaica_thread(&t);

	*slaved = JAMAICA_BLOCK_GET_DATA32((jamaica_ref) (0x80001000/4),3).i;

	*slavec = 71;
	//jam_comp_simpleTests_FFT_2_fft1(&t, 
	//	(jamaica_ref) (0x80001000/4), 
	//	(jamaica_ref) (0x80002000/4), 
	//	(jamaica_ref) (0x80003000/4), 
	//	(jamaica_ref) (0x80004000/4), 
	//	64, 
	//	(jamaica_ref) (0x80005000/4));

	jam_comp_unitTests_Types_0_types1(&t, 
		10,
		100.0, 
		1000.0,
		(jamaica_ref) (0x80001000/4),
		(jamaica_ref) (0x80002000/4)
		);

	*slavea = 11;
}

