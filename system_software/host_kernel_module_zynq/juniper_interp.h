#ifndef __JUNIPER_INTERP_H__
#define __JUNIPER_INTERP_H__

#include "juniper_pci.h"
#include "juniper_sysfs.h"

void juniper_interp_init(struct juniper_fpga_device* dev);
int juniper_interp_get_cores(struct juniper_fpga_device* dev);
int juniper_interp_accel_idle(struct juniper_accel_device* dev);
void juniper_interp_accel_start(struct juniper_accel_device* dev);
void juniper_interp_accel_hold(struct juniper_accel_device* dev, int hold);
void juniper_interp_accel_read(struct juniper_accel_device* dev, off_t offset, char* buf, size_t count);
void juniper_interp_accel_write(struct juniper_accel_device* dev, off_t offset, char* buf, size_t count);
ssize_t juniper_interp_config_write(struct juniper_fpga_device* dev, off_t offset, char* buf, size_t count);
uint32_t juniper_interp_get_arg(struct juniper_accel_device* dev, int argN);
void juniper_interp_set_arg(struct juniper_accel_device* dev, int argN, uint32_t val);
uint32_t juniper_interp_get_syscall_arg(struct juniper_accel_device* dev, int argN);
void juniper_interp_set_syscall_arg(struct juniper_accel_device* dev, int argN, uint32_t val);
uint32_t juniper_interp_get_ram_base(struct juniper_accel_device* dev);
void juniper_interp_set_ram_base(struct juniper_accel_device* dev, uint32_t val);
uint32_t juniper_interp_get_retVal(struct juniper_accel_device* dev);

uint32_t juniper_interp_get_interrupt_id(struct juniper_fpga_device* dev);
void     juniper_interp_ack_interrupt(struct juniper_fpga_device* dev, uint32_t intrId);



#endif