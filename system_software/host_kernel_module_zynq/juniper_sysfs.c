#include "juniper_sysfs.h"
#include "juniper_interp.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

#define JUNIPER_CLASS_NAME "juniper"
#define JUNIPER_ACCEL_CLASS_NAME "juniper_accel"
#define JUNIPER_DEVICE_NAME "juniper"

// Max size of a bitfile
#define FPGA_CONFIG_MAX_SIZE 128 * 1024 * 1024
#define ACCEL_MEM_SIZE 512 * 1024 * 1024
//#define ACCEL_MEM_SIZE 0

#define WR_PERM (S_IWUSR | S_IWGRP)
#define RD_PERM (S_IRUSR | S_IRGRP)

int juniper_sysfs_lost_device_iter(struct device* dev, void* data);
int juniper_sysfs_lost_device_accel_iter(struct device* dev, void* data);

ssize_t accel_idle_show(struct device* dev, struct device_attribute* attr, char* buf);
ssize_t accel_start_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);
ssize_t accel_hold_show(struct device* dev, struct device_attribute* attr, char* buf);
ssize_t accel_hold_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);

ssize_t accel_argN_show(struct device* dev, struct device_attribute* attr, char* buf);
ssize_t accel_argN_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);

ssize_t accel_retVal_show(struct device* dev, struct device_attribute* attr, char* buf);

ssize_t accel_ram_base_show(struct device* dev, struct device_attribute* attr, char* buf);
ssize_t accel_ram_base_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);

ssize_t accel_mem_read(struct file* f, struct kobject* kobj, struct bin_attribute* bin_attr, char* buf, loff_t offset, size_t count);
ssize_t accel_mem_write(struct file* f, struct kobject* kobj, struct bin_attribute* bin_attr, char* buf, loff_t offset, size_t count);

ssize_t fpga_icap_write(struct file* f, struct kobject* kobj, struct bin_attribute* bin_attr, char* buf, loff_t offset, size_t count);

ssize_t accel_interrupted_show(struct device* dev, struct device_attribute* attr, char* buf);

ssize_t accel_syscall_args_show(struct device* dev, struct device_attribute* attr, char* buf);
ssize_t accel_syscall_args_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);

static struct class* juniper_class = NULL;
static struct class* juniper_accel_class = NULL;

// Declare the attributes we need
// We want compat for Linux 2.6, so this binary attribute must be done manually :(
struct bin_attribute bin_attr_reconfig = {
	.attr = {.name = "reconfig", .mode = S_IWUGO },
	.read = NULL,
	.write = fpga_icap_write,
	.size = FPGA_CONFIG_MAX_SIZE
};

// Attributes for the accelerators
DEVICE_ATTR(accel_idle, RD_PERM, accel_idle_show, NULL);                       // Is the accelerator idle?
DEVICE_ATTR(accel_start, WR_PERM, NULL, accel_start_store);                    // Start the accelerator running
DEVICE_ATTR(accel_hold, RD_PERM | WR_PERM, accel_hold_show, accel_hold_store); // Accelerator isolator. USE BEFORE RECONFIG!
DEVICE_ATTR(accel_arg0, RD_PERM | WR_PERM, accel_argN_show, accel_argN_store); // Arguments
DEVICE_ATTR(accel_arg1, RD_PERM | WR_PERM, accel_argN_show, accel_argN_store);
DEVICE_ATTR(accel_arg2, RD_PERM | WR_PERM, accel_argN_show, accel_argN_store);
DEVICE_ATTR(accel_arg3, RD_PERM | WR_PERM, accel_argN_show, accel_argN_store);
DEVICE_ATTR(accel_retval, RD_PERM, accel_retVal_show, NULL);
DEVICE_ATTR(accel_ram_base, RD_PERM | WR_PERM, accel_ram_base_show, accel_ram_base_store);
DEVICE_ATTR(accel_interrupted, RD_PERM, accel_interrupted_show, NULL);
DEVICE_ATTR(accel_syscall_args, RD_PERM | WR_PERM, accel_syscall_args_show, accel_syscall_args_store);

struct bin_attribute bin_attr_accel_mem = {
	.attr = {.name = "mem", .mode = WR_PERM | RD_PERM },
	.read = accel_mem_read,
	.write = accel_mem_write,
	.size = ACCEL_MEM_SIZE
};

// Aggregate the accel_* device attrs
struct attribute* accel_device_attrs[] = {
	&dev_attr_accel_idle.attr,
	&dev_attr_accel_start.attr,
	&dev_attr_accel_arg0.attr,
	&dev_attr_accel_arg1.attr,
	&dev_attr_accel_arg2.attr,
	&dev_attr_accel_arg3.attr,
	&dev_attr_accel_retval.attr,
	&dev_attr_accel_ram_base.attr,
	&dev_attr_accel_interrupted.attr,
	&dev_attr_accel_syscall_args.attr,
	NULL
};

struct attribute* accel_device_reconfig_attrs[] = {
	&dev_attr_accel_hold.attr,
	NULL
};

struct attribute_group accel_device_attr_group = {
	.attrs = accel_device_attrs
};

struct attribute_group accel_device_reconfig_attr_group = {
	.attrs = accel_device_reconfig_attrs
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
	if(juniper_pci_is_reconfigurable(dev))
	{
		rc = device_create_bin_file(new_dev, &bin_attr_reconfig);
		if(rc)
		{
			printk(KERN_ERR "JFM: Failed to create reconfig bin file\n");
			return -ENODEV;
		}
	}

	// Do device specific initialisation
	juniper_interp_init(fpga_data);

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

		if(juniper_pci_is_reconfigurable(dev))
		{
			rc = sysfs_create_group(&accel_dev->kobj, &accel_device_reconfig_attr_group);
			if(rc)
			{
				printk(KERN_ERR "JFM: Failed to create sysfs group for subsubdevice\n");
				return -ENODEV;
			}
		}

		// Internally, the kernel frees based upon the attribute name, so we can tweak the parameters
		// of the attribute here.
		// This is, of course, a hack. But it works.
		bin_attr_accel_mem.size = juniper_pci_memory_size(dev);

		rc = device_create_bin_file(accel_dev, &bin_attr_accel_mem);
		if(rc)
		{
			printk(KERN_ERR "JFM: Failed to create mem bin file\n");
			return -ENODEV;
		}
		printk(KERN_INFO "JFM: Created subdevice dataaddr %p\n", accel_data);
	}
	printk(KERN_INFO "JFM: Created device dataaddr %p\n", fpga_data);

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

	if(juniper_pci_is_reconfigurable(fpga_data->phy_dev))
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
	int idle = 0;

	idle = juniper_interp_accel_idle(accel_data);
	buf[0] = idle ? '1' : '0';
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

	//	printk(KERN_INFO "Store count: %d\n", count);

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
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);
	struct juniper_fpga_device* fpga_data = accel_data->fpga;
	
	return juniper_interp_config_write(fpga_data, offset, buf, count);
}

ssize_t accel_argN_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	// Which attribute to read from?
	int attrId = -1;
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);
	uint32_t arg = 0;

	if(attr == &dev_attr_accel_arg0)
		attrId = 0;
	else if(attr == &dev_attr_accel_arg1)
		attrId = 1;
	else if(attr == &dev_attr_accel_arg2)
		attrId = 2;
	else if(attr == &dev_attr_accel_arg3)
		attrId = 3;

	if(attrId == -1)
	{
		printk(KERN_ERR "JFM: Failed to ascertain which argument attr corresponds to, bailing.");
		return -1;
	}

	// So, I'm kinda breaking the rules here.
	// This attribute is a binary attribute. It'll do for now.
	// Maybe consider making it ASCII later :P
	// We're assuming little-endian here.
	// TODO: Detect endian-ness from macros...
	arg = juniper_interp_get_arg(accel_data, attrId);
	buf[0] = arg & 0xFF;
	buf[1] = (arg >> 8) & 0xFF;
	buf[2] = (arg >> 16) & 0xFF;
	buf[3] = (arg >> 24) & 0xFF;

	return 4;
}

ssize_t accel_argN_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
	int attrId = -1;
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);
	uint32_t arg = 0;
	const unsigned char* unsBuf = (const unsigned char*)buf;

	// Sanity. Make sure we're writing the correct amount of data...
	if(count != 4)
		return -1;

	if(attr == &dev_attr_accel_arg0)
		attrId = 0;
	else if(attr == &dev_attr_accel_arg1)
		attrId = 1;
	else if(attr == &dev_attr_accel_arg2)
		attrId = 2;
	else if(attr == &dev_attr_accel_arg3)
		attrId = 3;

	if(attrId == -1)
	{
		printk(KERN_ERR "JFM: Failed to ascertain which argument attr corresponds to, bailing.");
		return -1;
	}

	// Pack it...
	arg |= unsBuf[0];
	arg |= (unsBuf[1] << 8);
	arg |= (unsBuf[2] << 16);
	arg |= (unsBuf[3] << 24);
	juniper_interp_set_arg(accel_data, attrId, arg);

	return 4;
}

ssize_t accel_ram_base_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);
	uint32_t arg = 0;

	// So, I'm kinda breaking the rules here.
	// This attribute is a binary attribute. It'll do for now.
	// Maybe consider making it ASCII later :P
	// We're assuming little-endian here.
	// TODO: Detect endian-ness from macros...
	arg = juniper_interp_get_ram_base(accel_data);
	buf[0] = arg & 0xFF;
	buf[1] = (arg >> 8) & 0xFF;
	buf[2] = (arg >> 16) & 0xFF;
	buf[3] = (arg >> 24) & 0xFF;

	return 4;
}

ssize_t accel_ram_base_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);

	const unsigned char* unsBuf = (const unsigned char*)buf;

	// Pack it...
	uint32_t arg = 0;
	arg |= unsBuf[0];
	arg |= (unsBuf[1] << 8);
	arg |= (unsBuf[2] << 16);
	arg |= (unsBuf[3] << 24);
	juniper_interp_set_ram_base(accel_data, arg);

	return 4;
}

ssize_t accel_retVal_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);
	uint32_t arg = 0;

	// So, I'm kinda breaking the rules here.
	// This attribute is a binary attribute. It'll do for now.
	// Maybe consider making it ASCII later :P
	// We're assuming little-endian here.
	// TODO: Detect endian-ness from macros...
	arg = juniper_interp_get_retVal(accel_data);
	buf[0] = arg & 0xFF;
	buf[1] = (arg >> 8) & 0xFF;
	buf[2] = (arg >> 16) & 0xFF;
	buf[3] = (arg >> 24) & 0xFF;

	return 4;
}

ssize_t accel_interrupted_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);
	buf[0] = accel_data->interrupted ? '1' : '0';
	buf[1] = '\0';

	//printk(KERN_INFO "READING INTERRUPTED FOR %d VAL %d ADDR %p!\n", accel_data->idx, accel_data->interrupted, accel_data);

	if(accel_data->interrupted)
		accel_data->interrupted = 0;
	return 2;
}

ssize_t accel_syscall_args_show(struct device* dev, struct device_attribute* attr, char* buf)
{
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);

	// Read out all of the syscall args...
	uint8_t  cmd  = juniper_interp_get_syscall_arg(accel_data, 0);
	uint32_t arg1 = juniper_interp_get_syscall_arg(accel_data, 1);
	uint32_t arg2 = juniper_interp_get_syscall_arg(accel_data, 2);
	uint32_t arg3 = juniper_interp_get_syscall_arg(accel_data, 3);
	uint32_t arg4 = juniper_interp_get_syscall_arg(accel_data, 4);
	uint32_t arg5 = juniper_interp_get_syscall_arg(accel_data, 5);

	// Pack that into the result...
	// Loop unrolling, innit.
	buf[0] = (cmd >> 0) & 0xFF;

	buf[1] = (arg1 >> 0) & 0xFF;
	buf[2] = (arg1 >> 8) & 0xFF;
	buf[3] = (arg1 >> 16)  & 0xFF;
	buf[4] = (arg1 >> 24)  & 0xFF;

	buf[5] = (arg2 >> 0) & 0xFF;
	buf[6] = (arg2 >> 8) & 0xFF;
	buf[7] = (arg2 >> 16)  & 0xFF;
	buf[8] = (arg2 >> 24)  & 0xFF;

	buf[9] = (arg3 >> 0) & 0xFF;
	buf[10] = (arg3 >> 8) & 0xFF;
	buf[11] = (arg3 >> 16)  & 0xFF;
	buf[12] = (arg3 >> 24)  & 0xFF;

	buf[13] = (arg4 >> 0) & 0xFF;
	buf[14] = (arg4 >> 8) & 0xFF;
	buf[15] = (arg4 >> 16)  & 0xFF;
	buf[16] = (arg4 >> 24)  & 0xFF;

	buf[17] = (arg5 >> 0) & 0xFF;
	buf[18] = (arg5 >> 8) & 0xFF;
	buf[19] = (arg5 >> 16)  & 0xFF;
	buf[20] = (arg5 >> 24)  & 0xFF;

	buf[21] = '\0';

	return 22;
}

ssize_t accel_syscall_args_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count)
{
	struct juniper_accel_device* accel_data = dev_get_drvdata(dev);
	uint8_t cmd = 0;
	uint32_t arg1 = 0;

	if(count < 5)
		return 0;

	cmd |= buf[0];

	arg1 |= buf[1] << 0;
	arg1 |= buf[2] << 8;
	arg1 |= buf[3] << 16;
	arg1 |= buf[4] << 24;

	juniper_interp_set_syscall_arg(accel_data, 1, arg1);
	juniper_interp_set_syscall_arg(accel_data, 0, cmd); // Todo: Do the command shuffling here?

	return 5;
}

static int juniper_sysfs_interrupted_find_accel(struct device* dev, void* userData)
{
	struct juniper_accel_device* accel = dev_get_drvdata(dev);

	//printk(KERN_INFO "JFM: Find device, curIdx = %d userdata = %ld drvdata: %p\n", accel->idx, (long)userData, accel);

	if(accel->idx == (long)userData)
		return 1;
	else
		return 0;
}

static int juniper_sysfs_interrupted_find_device(struct device* dev, void* userData)
{
  const char* device_name = dev_name(dev);

  // it's either juniper0 or uio0.
  if(device_name[0] == 'j')
    return 1;
  return 0;
}

void juniper_sysfs_interrupted(struct juniper_device* dev)
{
	// We were interrupted. Figure out what device raised it.
	struct device* jdev = NULL;
	struct device* childDev = NULL;
	struct device* childAccel = NULL;
	struct juniper_fpga_device* fpga_data = NULL;
	struct juniper_accel_device* accel_data = NULL;

	long hp_intr = -1;
	int dodgy_platformdevice_hack = 0;

	jdev = juniper_pci_getdev(dev);
	
	// Get the device
	childDev = device_find_child(jdev, &dodgy_platformdevice_hack, juniper_sysfs_interrupted_find_device);
	if(childDev == NULL)
	{
		printk(KERN_ERR "JFM: Could not find child device %ld in interrupt!\n", hp_intr);
		return;
	}

	fpga_data = dev_get_drvdata(childDev);
	hp_intr = juniper_interp_get_interrupt_id(fpga_data);

	// Break out if it didn't originate from the accelerators.
	if(hp_intr == -1)
	{
		printk(KERN_INFO "JFM: Got unknown interrupt...\n");
		return;
	}

	// Then the accelerator
	childAccel = device_find_child(childDev, (void*)hp_intr, juniper_sysfs_interrupted_find_accel);
	if(childAccel == NULL)
	{
	  printk(KERN_ERR "childAccel IS NULL in JUNIPER module\n");
	}

	accel_data = dev_get_drvdata(childAccel);
	//printk(KERN_INFO "JFM: Got interrupt for accelerator number %ld TEST: %d ADDR %p\n", hp_intr, accel_data->idx, accel_data);
	
	accel_data->interrupted = 1;
	put_device(childDev);
	juniper_interp_ack_interrupt(fpga_data, hp_intr);
}
