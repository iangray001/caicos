#include "juniper_sysfs.h"
#include "juniper_interp.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/slab.h>

#define JUNIPER_CLASS_NAME "juniper"
#define JUNIPER_ACCEL_CLASS_NAME "juniper_accel"
#define JUNIPER_DEVICE_NAME "juniper"

// Max size of a bitfile
#define FPGA_CONFIG_MAX_SIZE 128 * 1024 * 1024
#define ACCEL_MEM_SIZE 16 * 1024 * 1024

int juniper_sysfs_lost_device_iter(struct device* dev, void* data);
int juniper_sysfs_lost_device_accel_iter(struct device* dev, void* data);

ssize_t accel_idle_show(struct device* dev, struct device_attribute* attr, char* buf);
ssize_t accel_start_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);
ssize_t accel_hold_show(struct device* dev, struct device_attribute* attr, char* buf);
ssize_t accel_hold_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);

ssize_t accel_mem_read(struct file* f, struct kobject* kobj, struct bin_attribute* bin_attr, char* buf, loff_t offset, size_t count);
ssize_t accel_mem_write(struct file* f, struct kobject* kobj, struct bin_attribute* bin_attr, char* buf, loff_t offset, size_t count);

ssize_t fpga_icap_write(struct file* f, struct kobject* kobj, struct bin_attribute* bin_attr, char* buf, loff_t offset, size_t count);

static struct class* juniper_class = NULL;
static struct class* juniper_accel_class = NULL;

// Declare the attributes we need
// We want compat for Linux 2.6, so this binary attribute must be done manually :(
struct bin_attribute bin_attr_reconfig = {
	.attr = {.name = "reconfig", .mode = S_IWUSR|S_IWGRP },
	.read = NULL,
	.write = fpga_icap_write,
	.size = FPGA_CONFIG_MAX_SIZE
};

// Attributes for the accelerators
DEVICE_ATTR(accel_idle, S_IRUSR|S_IRGRP, accel_idle_show, NULL);                       // Is the accelerator idle?
DEVICE_ATTR(accel_start, S_IWUSR|S_IWGRP, NULL, accel_start_store);                    // Start the accelerator running
DEVICE_ATTR(accel_hold, S_IRUSR|S_IRGRP | S_IWUSR|S_IWGRP, accel_hold_show, accel_hold_store); // Accelerator isolator. USE BEFORE RECONFIG!

struct bin_attribute bin_attr_accel_mem = {
	.attr = {.name = "mem", .mode = S_IWUGO | S_IRUGO },
	.read = accel_mem_read,
	.write = accel_mem_write,
	.size = ACCEL_MEM_SIZE
};

// Aggregate the accel_* device attrs
struct attribute* accel_device_attrs[] = {
	&dev_attr_accel_idle.attr,
	&dev_attr_accel_start.attr,
	&dev_attr_accel_hold.attr,
	NULL
};

struct attribute_group accel_device_attr_group = {
	.attrs = accel_device_attrs
};

int juniper_sysfs_register()
{
	// Need to start creating the required device nodes
	juniper_class = class_create(THIS_MODULE, JUNIPER_CLASS_NAME);
	if(juniper_class == 0)
	{
		printk(KERN_ERR "JFM: Failed to create sysfs class\n");
		return -ENODEV;
	}

	juniper_accel_class = class_create(THIS_MODULE, JUNIPER_ACCEL_CLASS_NAME);
	if(juniper_accel_class == 0)
	{
		printk(KERN_ERR "JFM: Failed to create sysfs accelerator class\n");
		return -ENODEV;
	}

	return 0;
}

void juniper_sysfs_unregister()
{
	class_unregister(juniper_accel_class);
	class_destroy(juniper_accel_class);
	class_unregister(juniper_class);
	class_destroy(juniper_class);
}

int juniper_sysfs_new_device(struct juniper_device* dev)
{
	// Create the device node...
	unsigned int dev_idx = juniper_pci_devidx(dev);
	unsigned int rc;
	int num_accel = 0;
	int i = 0;
	struct device* new_dev = NULL;
	
	// Make a test device...
	struct juniper_fpga_device* fpga_data = kzalloc(sizeof(struct juniper_fpga_device), GFP_KERNEL);
	fpga_data->phy_dev = dev;

	new_dev = device_create(juniper_class, juniper_pci_getdev(dev), 0, fpga_data, JUNIPER_DEVICE_NAME "%d", dev_idx);
	if(!new_dev)
	{
		printk(KERN_ERR "JFM: Failed to create subdevice\n");
		return -ENODEV;
	}

	// Create the binary attribute
	rc = device_create_bin_file(new_dev, &bin_attr_reconfig);
	if(rc)
	{
		printk(KERN_ERR "JFM: Failed to create reconfig bin file\n");
		return -ENODEV;
	}

	// Create the accelerator nodes
	num_accel = juniper_interp_get_cores(fpga_data);
	for(i = 0; i < num_accel; i++)
	{
		struct juniper_accel_device* accel_data = kzalloc(sizeof(struct juniper_accel_device), GFP_KERNEL);
		struct device* accel_dev = NULL;
		accel_data->fpga = fpga_data;
		accel_data->held = 0;
		accel_data->idx = i;

		// Now create the sub-sub device
		accel_dev = device_create(juniper_accel_class, new_dev, 0, accel_data, JUNIPER_DEVICE_NAME "%d%c", dev_idx, 'a'+i);
		if(!accel_dev)
		{
			printk(KERN_ERR "JFM: Failed to create subsubdevice\n");
			return -ENODEV;
		}

		// Create the group nodes
		rc = sysfs_create_group(&accel_dev->kobj, &accel_device_attr_group);
		if(rc)
		{
			printk(KERN_ERR "JFM: Failed to create sysfs group for subsubdevice\n");
			return -ENODEV;
		}

		rc = device_create_bin_file(accel_dev, &bin_attr_accel_mem);
		if(rc)
		{
			printk(KERN_ERR "JFM: Failed to create mem bin file\n");
			return -ENODEV;
		}
	}

	return 0;
}

int juniper_sysfs_lost_device_accel_iter(struct device* dev, void* data)
{
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);
	kfree(accel_data);
	dev_set_drvdata(dev, NULL);

	device_remove_bin_file(dev, &bin_attr_accel_mem);
	device_del(dev);
	put_device(dev);

	return 0;
}

int juniper_sysfs_lost_device_iter(struct device* dev, void* data)
{
	struct juniper_fpga_device* fpga_data = NULL;

	device_for_each_child(dev, NULL, juniper_sysfs_lost_device_accel_iter);

	fpga_data = dev_get_drvdata(dev);
	kfree(fpga_data);
	dev_set_drvdata(dev, NULL);

	device_remove_bin_file(dev, &bin_attr_reconfig);
	device_del(dev);
	put_device(dev);

	return 0;
}

void juniper_sysfs_lost_device(struct juniper_device* dev)
{
	struct device* jdev = juniper_pci_getdev(dev);

	// Free the device children.
	device_for_each_child(jdev, NULL, juniper_sysfs_lost_device_iter);
}


ssize_t accel_idle_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	// Get the status of the accelerator
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);
	int running = 0;

	running = juniper_interp_accel_idle(accel_data);
	buf[0] = running ? '1' : '0';
	buf[1] = '\0';

	return 2;
}

ssize_t accel_start_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);

	if(count < 1)
		return 0;

	if(buf[0] == '1')
		juniper_interp_accel_start(accel_data);
	return count;
}

ssize_t accel_hold_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);
	buf[0] = accel_data->held ? '1' : '0';
	buf[1] = '\0';
	return 2;
}

ssize_t accel_hold_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);

	if(count < 1)
		return 0;

	//printk(KERN_INFO "Store count: %d\n", count);

	if(buf[0] == '1')
	{
		juniper_interp_accel_hold(accel_data, 1);
		accel_data->held = 1;
	}
	else
	{
		juniper_interp_accel_hold(accel_data, 0);
		accel_data->held = 0;
	}

	return count; // Fully consume
}

ssize_t accel_mem_read(struct file* f, struct kobject* kobj, struct bin_attribute* bin_attr, char* buf, loff_t offset, size_t count)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);

	juniper_interp_accel_read(accel_data, offset, buf, count);
	return count;
}

ssize_t accel_mem_write(struct file* f, struct kobject* kobj, struct bin_attribute* bin_attr, char* buf, loff_t offset, size_t count)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);

	juniper_interp_accel_write(accel_data, offset, buf, count);
	return count;
}

ssize_t fpga_icap_write(struct file* f, struct kobject* kobj, struct bin_attribute* bin_attr, char* buf, loff_t offset, size_t count)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct juniper_fpga_device* accel_data = dev_get_drvdata(dev);

	return juniper_interp_config_write(accel_data, offset, buf, count);
}