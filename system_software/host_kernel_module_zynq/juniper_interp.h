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


#endif