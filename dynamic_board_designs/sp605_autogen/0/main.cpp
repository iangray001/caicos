#include <stdlib.h>
#include <string.h>
#include <ap_int.h>

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

volatile int *__juniper_ram_master;
volatile char *__juniper_ram_master_char;
volatile short *__juniper_ram_master_short;
#ifdef JUNIPER_SUPPORT_FLOATS
volatile float *__juniper_ram_master_float;
#endif

volatile ap_uint<1>   jamaica_syscall;
volatile PCIE_Syscall syscall_args;

void fix_hls_compiler()
{
#pragma HLS INLINE
    syscall_args.cmd = syscall_args.arg1;
    syscall_args.arg1 = syscall_args.arg2;
    syscall_args.arg2 = syscall_args.arg3;
    syscall_args.arg3 = syscall_args.arg4;
    syscall_args.arg4 = syscall_args.arg5;
    syscall_args.arg5 = syscall_args.cmd;
}

int hls(int* opid, int* arg1, int* arg2, int* arg3)
{
#pragma HLS INTERFACE m_axi port=__juniper_ram_master offset=slave
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_char offset=slave
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_short offset=slave
#ifdef JUNIPER_SUPPORT_FLOATS
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_float offset=slave
#endif

#pragma HLS INTERFACE s_axilite port=opid bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=arg1 bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=arg2 bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=arg3 bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=syscall_args bundle=AXILiteS register
#pragma HLS INTERFACE s_axilite port=return bundle=AXILiteS register

#pragma HLS INTERFACE ap_none port=jamaica_syscall register

    // Annoyingly, it has to do *something*
    //fix_hls_compiler();
    jamaica_syscall = 0;

    syscall_args.cmd = 0;
    syscall_args.arg1 = 0;
    syscall_args.arg2 = 0;
    syscall_args.arg3 = 0;
    syscall_args.arg4 = 0;
    syscall_args.arg5 = 0;

    volatile unsigned char foo = syscall_args.ret_cmd;
    volatile unsigned int bar = syscall_args.retVal;
    __juniper_ram_master[0] = 0;
    __juniper_ram_master[1] = 0;

    return 0;
}
