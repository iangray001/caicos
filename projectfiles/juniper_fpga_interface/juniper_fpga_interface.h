#ifndef __JUNIPER_FPGA_INTERFACE__
#define __JUNIPER_FPGA_INTERFACE__

#include <stdint.h>

#define JUNIPER_FPGA_OK 0
#define JUNIPER_FPGA_FAIL_NOFILE -1
#define JUNIPER_FPGA_FAIL_DRVERR -2

#define JUNIPER_FPGA_SYSCALL_IN_ARGS_SIZE 21
#define JUNIPER_FPGA_SYSCALL_OUT_ARGS_SIZE 5

#define JUNIPER_FPGA_SYSCALL_OUT_KNOCK1 0xAA
#define JUNIPER_FPGA_SYSCALL_OUT_KNOCK2 0x55

typedef struct 
{
	uint8_t cmd;
	uint32_t arg1;
	uint32_t arg2;
	uint32_t arg3;
	uint32_t arg4;
	uint32_t arg5;
} juniper_fpga_syscall_args;

int juniper_fpga_num_devices();
int juniper_fpga_num_partitions(int devNo);

// If positive (or zero), the idle status
// If negative, error.
int juniper_fpga_partition_idle(int devNo, int partNo);
int juniper_fpga_partition_interrupted(int devNo, int partNo);
int juniper_fpga_partition_start(int devNo, int partNo);

int juniper_fpga_partition_get_mem_base(int devNo, int partNo, uint32_t* base);
int juniper_fpga_partition_set_mem_base(int devNo, int partNo, uint32_t base);
int juniper_fpga_partition_get_arg(int devNo, int partNo, int argNo, uint32_t* arg);
int juniper_fpga_partition_set_arg(int devNo, int partNo, int argNo, uint32_t arg);
int juniper_fpga_partition_get_retval(int devNo, int partNo, uint32_t* rv);

int juniper_fpga_partition_get_syscall_args(int devNo, int partNo, juniper_fpga_syscall_args* args);
int juniper_fpga_partition_set_syscall_return(int devNo, int partNo, unsigned int val);

#endif
