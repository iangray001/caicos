#include <stdlib.h>
#include <string.h>
#include <ap_int.h>

//Memory interfaces
volatile int *__juniper_ram_master;
volatile char *__juniper_ram_master_char;
volatile short *__juniper_ram_master_short;
#ifdef JUNIPER_SUPPORT_FLOATS
volatile float *__juniper_ram_master_float;
#endif

typedef struct
{
    unsigned char cmd;
    unsigned int arg1;
    unsigned int arg2;
    unsigned int arg3;
    unsigned int arg4;
    unsigned int arg5;
    
    unsigned char ret_cmd;
    unsigned int  retVal;
} PCIE_Syscall;

volatile ap_uint<1>   jamaica_syscall;
//volatile int jamaica_syscall;
volatile PCIE_Syscall syscall_args;

void fix_hls_compiler();

unsigned int do_syscall(unsigned char cmd, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5)
{
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_char bundle=MAXI offset=slave
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_short bundle=MAXI offset=slave
#ifdef JUNIPER_SUPPORT_FLOATS
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_float bundle=MAXI offset=slave
#endif
#pragma HLS INLINE

#pragma HLS INTERFACE s_axilite port=syscall_args bundle=AXILiteS register
#pragma HLS INTERFACE ap_none port=jamaica_syscall register
	
	syscall_args.cmd = cmd;
	syscall_args.arg1 = arg1;
	syscall_args.arg2 = arg2;
	syscall_args.arg3 = arg3;
	syscall_args.arg4 = arg4;
	syscall_args.arg5 = arg5;

	// Raise int high
	jamaica_syscall = 1;

	// Wait for response
	while(syscall_args.ret_cmd != 0xAA);

	jamaica_syscall = 0;

        while(syscall_args.ret_cmd != 0x55);

	return syscall_args.retVal;
}

int hls(int* opid, int* arg1, int* arg2, int* arg3)
{
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_char bundle=MAXI offset=slave
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_short bundle=MAXI offset=slave
#ifdef JUNIPER_SUPPORT_FLOATS
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_float bundle=MAXI offset=slave
#endif

#pragma HLS INTERFACE s_axilite port=opid bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=arg1 bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=arg2 bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=arg3 bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=syscall_args bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=return bundle=AXILiteS register
#pragma HLS INTERFACE ap_none port=jamaica_syscall register

//	fix_hls_compiler();
	int foo = 0;
	//jamaica_syscall = 0;

	do_syscall(0x10, 1, 2, 3, 4, 5);

	foo = __juniper_ram_master[0];
	foo += 4;
	__juniper_ram_master[1] = 6;

	do_syscall(0x11, 1, 2, 3, 4, 5);

    return 0;
}

void fix_hls_compiler()
{
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_char bundle=MAXI offset=slave
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_short bundle=MAXI offset=slave
#ifdef JUNIPER_SUPPORT_FLOATS
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_float bundle=MAXI offset=slave
#endif
#pragma INLINE

    syscall_args.cmd = syscall_args.arg1;
    syscall_args.arg1 = syscall_args.arg2;
    syscall_args.arg2 = syscall_args.arg3;
    syscall_args.arg3 = syscall_args.arg4;
    syscall_args.arg4 = syscall_args.arg5;
    syscall_args.arg5 = syscall_args.cmd;
}
