// Module stuff
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include "juniper_pci.h"
#include "juniper_sysfs.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jamie Garside <jamie.garside@york.ac.uk");
MODULE_DESCRIPTION("JUNIPER JFM Communications Module");

static int __init juniper_init(void);
static void __exit juniper_exit(void);

static void juniper_discovered_device(struct juniper_device* dev);
static void juniper_lost_device(struct juniper_device* dev);
static void juniper_interrupted(struct juniper_device* dev);

int juniper_init()
{
	int rc = 0;

	rc = juniper_sysfs_register();
	if(rc > 0)
	{
		printk(KERN_ERR "JFM: Sysfs registration failed. Aborting\n");
		return -ENODEV;
	}

	rc = juniper_pci_register(juniper_discovered_device, juniper_lost_device, juniper_interrupted);
	if(rc > 0)
	{
		printk(KERN_ERR "JFM: PCI registration failed. Aborting\n");
		return -ENODEV;
	}

	return 0;
}

void juniper_exit()
{
	juniper_sysfs_unregister();
	juniper_pci_unregister();
}

// This mechanism exists so we can route this signal through to the filesystem layer etc.
void juniper_discovered_device(struct juniper_device* dev)
{
	juniper_sysfs_new_device(dev);
}

void juniper_lost_device(struct juniper_device* dev)
{
	juniper_sysfs_lost_device(dev);
}

void juniper_interrupted(struct juniper_device* dev)
{
	juniper_sysfs_interrupted(dev);
}

module_init(juniper_init);
module_exit(juniper_exit);