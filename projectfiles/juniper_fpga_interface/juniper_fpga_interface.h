#ifndef __JUNIPER_FPGA_INTERFACE__
#define __JUNIPER_FPGA_INTERFACE__

#define JUNIPER_FPGA_OK 0
#define JUNIPER_FPGA_FAIL_NOFILE -1
#define JUNIPER_FPGA_FAIL_DRVERR -2

int juniper_fpga_num_devices();
int juniper_fpga_num_partitions(int devNo);

// If positive (or zero), the idle status
// If negative, error.
int juniper_fpga_partition_idle(int devNo, int partNo);
int juniper_fpga_partition_interrupted(int devNo, int partNo);
int juniper_fpga_partition_start(int devNo, int partNo);

int juniper_fpga_partition_get_mem_base(int devNo, int partNo, unsigned int* base);
int juniper_fpga_partition_set_mem_base(int devNo, int partNo, unsigned int base);
int juniper_fpga_partition_get_arg(int devNo, int partNo, int argNo, unsigned int* arg);
int juniper_fpga_partition_set_arg(int devNo, int partNo, int argNo, unsigned int arg);
int juniper_fpga_partition_get_retval(int devNo, int partNo, unsigned int* rv);

#endif
