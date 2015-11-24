#include "juniper_pci.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/uio_driver.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <linux/ioport.h>
#include <asm/io.h>

// The implementation of the opaque pointer that is thrown around in the rest
// of the library.
struct juniper_device
{
	struct platform_device* pdev;
	void* mem_storage;
	void* periph_storage;
	int irq;
	struct uio_info* uio_reg;
};

static int juniper_probe(struct platform_device* pdev);
static int juniper_remove(struct platform_device* pdev);
static irqreturn_t juniper_irqhandler(int irq, void* data);

static const struct of_device_id juniper_zynq_id[] = {
	{ .compatible = "york,juniper-zynq-1.0" },
	{}
};

MODULE_DEVICE_TABLE(of, juniper_zynq_id);

static struct platform_driver juniper_zynq_driver = {
	.probe = juniper_probe,
	.remove = juniper_remove,
	.driver = {
		.name = "juniper_zynq",
		.of_match_table = juniper_zynq_id,
	}
};

// Callbacks to the rest of the driver
void (*new_device_callback)(struct juniper_device* dev) = NULL;
void (*lost_device_callback)(struct juniper_device* dev) = NULL;
void (*interrupted_callback)(struct juniper_device* dev) = NULL;

int juniper_pci_register(void (*new_device)(struct juniper_device* dev), void (*lost_device)(struct juniper_device* dev), void (*interrupted)(struct juniper_device* dev))
{
	new_device_callback = new_device;
	lost_device_callback = lost_device;
	interrupted_callback = interrupted;
	
	return platform_driver_register(&juniper_zynq_driver);
}

void juniper_pci_unregister()
{
	platform_driver_unregister(&juniper_zynq_driver);
}

static int juniper_probe(struct platform_device* pdev)
{
	struct resource* mem_resource;      // The DDR for the accelerator
	struct resource* periph_resource;   // The action device I/O region
	void* mem_storage = NULL;
	void* periph_storage = NULL;
	int irq;
	int rv;
	struct juniper_device* jdev = NULL;
	struct uio_info* uio_reg;

	// First, get the resources from the DTB and map them
	mem_resource = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(mem_resource == NULL)
	{
		rv = -ENOSYS;
		printk(KERN_ERR "JFM: Could not get mem resource.\n");
		goto err_mem_res;
	}

	periph_resource = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if(periph_resource == NULL)
	{
		rv = -ENOSYS;
		printk(KERN_ERR "JFM: Could not get peripheral resource.\n");
		goto err_periph_res;
	}

	mem_storage = devm_ioremap_resource(&pdev->dev, mem_resource);
	if(IS_ERR(mem_storage))
	{
		rv = PTR_ERR(mem_storage);
		printk(KERN_ERR "JFM: Could not ioremap memory resource.\n");
		goto err_mem_storage;
	}

	periph_storage = devm_ioremap_resource(&pdev->dev, periph_resource);
	if(IS_ERR(periph_storage))
	{
		rv = PTR_ERR(periph_storage);
		printk(KERN_ERR "JFM: Could not ioremap periph resource.\n");
		goto err_periph_storage;
	}

	// Register an interrupt handler
	irq = platform_get_irq(pdev, 0);
	if(irq < 0)
	{
		rv = irq;
		printk(KERN_ERR "JFM: Could not get IRQ number.\n");
		goto err_irq;
	}

	jdev = kzalloc(sizeof(struct juniper_device), GFP_KERNEL);
	if(jdev == NULL)
	{
		rv = -ENOMEM;
		printk(KERN_ERR "JFM: Cannot alloc storage for struct juniper_device.\n");
		goto err_jd_kzalloc;
	}

	jdev->pdev = pdev;
	jdev->mem_storage = mem_storage;
	jdev->periph_storage = periph_storage;
	jdev->irq = irq;

	rv = devm_request_irq(&pdev->dev, irq, juniper_irqhandler, 0, "jzynq", jdev);
	if(rv != 0)
	{
		// rv is already set correctly
		printk(KERN_ERR "JFM: Could not register interrupt.\n");
		goto err_irq_map;
	}

	// Finally, create the UIO region for the memory.
	uio_reg = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
	if(!uio_reg)
	{
		rv = -ENOMEM;
		printk(KERN_ERR "JFM: Could not alloc storage for uio_reg.\n");
		goto err_uio_kzalloc;
	}

	uio_reg->name = "juniper_uio";
	uio_reg->version = "1.0";
	uio_reg->mem[0].name = "juniper_uio_mem";
	uio_reg->mem[0].memtype = UIO_MEM_VIRTUAL;
	uio_reg->mem[0].addr = (phys_addr_t)mem_storage;
	uio_reg->mem[0].size = mem_resource->end - mem_resource->start + 1;

	jdev->uio_reg = uio_reg;

	rv = uio_register_device(&pdev->dev, uio_reg);
	if(rv)
	{	
		printk(KERN_ERR "JFM: Could not map UIO region.\n");
		goto err_uio_reg;
	}

	platform_set_drvdata(pdev, jdev);

	// Finally, call the sysfs init routines
	new_device_callback(jdev);

	return 0;

err_uio_reg:
	kfree(uio_reg);
err_uio_kzalloc:
	// devm_request_irq should clean itself up on error exit.
err_irq_map:
	kfree(jdev);
err_jd_kzalloc:
err_irq:
	// periph_storage is dev managed, shouldn't need to dealloc here
err_periph_storage:
	// mem_storage is dev managed - I'm not sure if it auto cleans up here...
err_mem_storage:
err_periph_res:
err_mem_res:
	return rv;
}

static int juniper_remove(struct platform_device* pdev)
{
	// Most of this is dev-managed, so we don't need to do much
	struct juniper_device* jdev;

	jdev = platform_get_drvdata(pdev);

	lost_device_callback(jdev);
	uio_unregister_device(jdev->uio_reg);

	kfree(jdev->uio_reg);
	kfree(jdev);

	return 0;
}

// Code to implement the functions defined in juniper_pci.h
struct device* juniper_pci_getdev(struct juniper_device* dev)
{
	return &dev->pdev->dev;
}

unsigned int juniper_pci_devidx(struct juniper_device* dev)
{
	// The Zynq currently only implements a single FPGA device
	return 0;
}

resource_size_t juniper_pci_memory_size(struct juniper_device* dev)
{
	struct resource* res;
	res = platform_get_resource(dev->pdev, IORESOURCE_MEM, 0);
	if(!res)
	{
		// If device init succeeded, this should never, ever happen.
		printk(KERN_ERR "JFM: Could not get platform mem resource 0, aborting.\n");
		return -1;
	}

	return res->end - res->start + 1;
}

unsigned int juniper_pci_is_reconfigurable(struct juniper_device* dev)
{
	// The Zynq is always connected to a reconfig ICAP.
	return 1;
}

unsigned int juniper_pci_read_memory(struct juniper_device* dev, unsigned int address)
{
	char* plStorageBase = (char*)dev->mem_storage;
	plStorageBase += address;

	return ioread32(plStorageBase);
}

void juniper_pci_write_memory(struct juniper_device* dev, unsigned int address, unsigned int value)
{
	char* plStorageBase = (char*)dev->mem_storage;
	plStorageBase += address;

	iowrite32(value, plStorageBase);
}

unsigned int juniper_pci_read_periph(struct juniper_device* dev, unsigned int address)
{
	char* plStorageBase = (char*)dev->periph_storage;
	plStorageBase += address;

	return ioread32(plStorageBase);
}

void juniper_pci_write_periph(struct juniper_device* dev, unsigned int address, unsigned int value)
{
	char* plStorageBase = (char*)dev->periph_storage;
	plStorageBase += address;

	iowrite32(value, plStorageBase);
}

// Consider making these actual DMA later...
void juniper_pci_read_memory_burst(struct juniper_device* dev, unsigned int base_address, unsigned int* data, unsigned int count)
{
	char* plStorageBase = (char*)dev->mem_storage;
	plStorageBase += base_address;

	while(count--)
	{
		*data = ioread32(plStorageBase);
		data++;
		plStorageBase += 4;
	}
}

void juniper_pci_write_memory_burst(struct juniper_device* dev, unsigned int base_address, unsigned int* data, unsigned int count)
{
	char* plStorageBase = (char*)dev->mem_storage;
	plStorageBase += base_address;

	memcpy_toio(plStorageBase, data, count);
}

static irqreturn_t juniper_irqhandler(int irq, void* data)
{
	struct juniper_device* dev = data;
	interrupted_callback(dev);

	printk(KERN_INFO "JFM: INTERRUPT!\n");
	return IRQ_HANDLED;
}
