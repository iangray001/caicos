#include <juniper_fpga_interface.h>
#include <juniperoperations.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <jamaica.h>
#include <jni.h>
#include <jbi.h>

extern jamaica_initClassEntry initClassesTable[];

void caicos_handle_pcie_interrupt(jamaica_thread *ct, int devNo, int partNo) {
	int rv = 0;
	juniper_fpga_syscall_args args;
	memset(&args, 0, sizeof(juniper_fpga_syscall_args));
	rv = juniper_fpga_partition_get_syscall_args(devNo, partNo, &args);
	
	if(rv != JUNIPER_FPGA_OK)
		fprintf(stderr, "Failed to open syscall file with code %d.\n", rv);
		
	switch(args.cmd) {
		case 250:
			rv = jamaicaInterpreter_allocJavaObject(ct, (jamaica_ref) initClassesTable[args.arg1].variable);
			break;
		case 249:
			rv = jamaicaInterpreter_allocSimpleArray(ct, (jamaica_int32) args.arg1, (jamaica_ref) initClassesTable[args.arg2].variable);
			break;
		case 248:
			rv = jamaicaInterpreter_initialize_class_helper(ct, (jamaica_ref) initClassesTable[args.arg1].variable, (jamaica_int32) args.arg2);
			break;
$ADDITIONAL_SYSCALLS
		default:
			rv = -1;
			break;
	}
	
	juniper_fpga_partition_set_syscall_return(devNo, partNo, rv);
}
