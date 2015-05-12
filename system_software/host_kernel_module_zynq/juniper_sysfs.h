#ifndef __JUNIPER_SYSFS_H__
#define __JUNIPER_SYSFS_H__

#include "juniper_pci.h"

struct juniper_fpga_device
{
	struct juniper_device* phy_dev; // A reference to the actual parent hardware device
	unsigned int current_hold_mask;
};

struct juniper_accel_device
{
	struct juniper_fpga_device* fpga; // A reference to the FPGA
	int idx;
	int held; // Since we can't read back from the damn GPIO...
};

// Top level begin/end
int juniper_sysfs_register(void);
void juniper_sysfs_unregister(void);

int juniper_sysfs_new_device(struct juniper_device* dev);
void juniper_sysfs_lost_device(struct juniper_device* dev);

#endif