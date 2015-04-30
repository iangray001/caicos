#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memset...
#include <juniper_fpga_interface.h>

void handle_interrupt()
{
	// Read out the syscall args
	int rv = 0;
	juniper_fpga_syscall_args args;
	memset(&args, 0, sizeof(juniper_fpga_syscall_args));

	rv = juniper_fpga_partition_get_syscall_args(0, 0, &args);

	if(rv != JUNIPER_FPGA_OK)
		fprintf(stderr, "Failed to open syscall file with code %d.\n", rv);

	printf("Syscall: cmd: %d arg1: %d arg2: %d arg3: %d arg4: %d arg5: %d\n",
		   args.cmd,
		   args.arg1,
		   args.arg2,
		   args.arg3,
		   args.arg4,
		   args.arg5);

	juniper_fpga_partition_set_syscall_return(0, 0, 0xDEADBEEF);
}

int main()
{
	int rv = 0;

	printf("===============================\n");
	printf("  JUNIPER FPGA Interface Test\n");
	printf("===============================\n");

	printf("Starting accelerator...\n");

	rv = juniper_fpga_partition_start(0, 0);
	if(rv != JUNIPER_FPGA_OK)
	{
		fprintf(stderr, "Failed to start accelerator with code %d\n", rv);
		return 1;
	}

	printf("Spinning on done/interrupted...\n");

	while(1)
	{
		if(juniper_fpga_partition_idle(0, 0) == 1)
		{
			printf("Partition finished\n");
			break;
		}

		if(juniper_fpga_partition_interrupted(0, 0) == 1)
		{
			printf("Partition interrupted...\n");

			handle_interrupt();
		}
	}

	printf("Done!\n");

	return 0;
}