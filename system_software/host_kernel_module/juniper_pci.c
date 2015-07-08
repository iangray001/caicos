#include "juniper_pci.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

#define JUNIPER_PCI_INTF_WIDTH 4 // 32-bit interface
#define MEM_ORDER 2

#define JUNIPER_MEM_BAR 0
#define JUNIPER_PERIPH_BAR 2

//#define PCIE_ADDR_BASE 0x24000000
#define PCIE_ADDR_BASE 0x04000000
#define PCIE_OFF_BAR0U 0x208
#define PCIE_OFF_BAR0L 0x20C

#define PCIE_MASTER_BASE 0x40000000

//#define CDMA_ADDR_BASE 0x25000000
#define CDMA_ADDR_BASE    0x05000000
#define CDMA_OFF_CDMACR   0x0            // Control register
#define CDMA_OFF_CDMASR   0x4            // Status register
#define CDMA_OFF_SRCADDR  0x18           // DMA source address
#define CDMA_OFF_DESTADDR 0x20           // DMA destination address
#define CDMA_OFF_BTT      0x28           // Bytes to transfer

#define CDMA_CDMACR_IOC_IRQ_BIT 12
#define CDMA_CDMASR_IDLE_BIT    1
#define CDMA_CDMASR_IOC_IRQ_BIT 12
#define CDMA_BTT_MASK           0x003FFFFF

static int juniper_probe(struct pci_dev *pdev, const struct pci_device_id* ent);
static void juniper_remove(struct pci_dev *pdev);

static irqreturn_t juniper_irqhandler(int irq, void* data);

// This is the opaque pointer used for callbacks etc.
// This just makes life easier.
struct juniper_device 
{
	struct pci_dev* device;
};

struct juniper_dev_privdata
{
	struct juniper_device* externDev; // Reference to the created device so we can keep track of it
	unsigned int* memory_bar;
	unsigned int* periph_bar;
	int dma_using_dac;
	unsigned int* dma_mem_backing;
	wait_queue_head_t wait_queue;
	int dma_done_flag;
	dma_addr_t dma_handle;
};

static struct pci_device_id juniper_ids[] = 
{
	{ PCI_VDEVICE(XILINX, 0x7011) },
	{}
};

MODULE_DEVICE_TABLE(pci, juniper_ids);

static struct pci_driver juniper_driver = 
{
	.name = "JUNIPER JFM Host Driver",
	.id_table = juniper_ids,
	.probe = juniper_probe,
	.remove = juniper_remove
};

void (*new_device_callback)(struct juniper_device* dev) = NULL;
void (*lost_device_callback)(struct juniper_device* dev) = NULL;
void (*interrupted_callback)(struct juniper_device* dev) = NULL;

int juniper_pci_register(void (*new_device)(struct juniper_device* dev), void (*lost_device)(struct juniper_device* dev), void (*interrupted)(struct juniper_device* dev))
{
	new_device_callback = new_device;
	lost_device_callback = lost_device;
	interrupted_callback = interrupted;
	return pci_register_driver(&juniper_driver);
}

void juniper_pci_unregister()
{
	pci_unregister_driver(&juniper_driver);
}

struct device* juniper_pci_getdev(struct juniper_device* dev)
{
	return &dev->device->dev;
}

unsigned int juniper_pci_devidx(struct juniper_device* dev)
{
	return PCI_SLOT(dev->device->devfn);
}

resource_size_t juniper_pci_memory_size(struct juniper_device* dev)
{
	return pci_resource_len(dev->device, 0);
}

// TODO: More graceful error handling.
static int juniper_probe(struct pci_dev *pdev, const struct pci_device_id* ent)
{
	int rc = 0;
	struct juniper_dev_privdata* pd = kzalloc(sizeof(struct juniper_dev_privdata), GFP_KERNEL);
	unsigned int tmp;

	if(pd == NULL)
	{
		printk(KERN_ERR "JFM: Couldn't allocate storage for PCI driver privdata. Aborting\n");
		return -ENODEV;
	}

	pd->externDev = kzalloc(sizeof(struct juniper_device), GFP_KERNEL);
	if(pd->externDev == NULL)
	{
		printk(KERN_ERR "JFM: Couldn't allocate storage for PCI driver external reference. Aborting\n");
		return -ENODEV;
	}

	pd->externDev->device = pdev;

	pci_set_drvdata(pdev, pd);

	rc = pci_enable_device(pdev);

	if(rc)
	{	
		printk(KERN_ERR "JFM: Couldn't enable PCI device. Aborting.\n");
		return -ENODEV;
	}

	if(!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM))
	{
		printk(KERN_ERR "JFM: PCI device has wrong resource flags on BAR 0. Aborting.\n");
		return -ENODEV;
	}

	rc = pci_request_regions(pdev, "juniper");
	if(rc)
	{
		printk(KERN_ERR "JFM: Couldn't take ownership of PCI regions. Aborting.\n");
		return -ENODEV;
	}

	// Map all of BAR 0
	pd->memory_bar = pci_iomap(pdev, JUNIPER_MEM_BAR, 0);

	if(!pd->memory_bar)
	{
	  printk(KERN_ERR "JFM: Couldn't take memory BAR (%d). Aborting.\n", JUNIPER_MEM_BAR);
		return -ENODEV;
	}

	pd->periph_bar = pci_iomap(pdev, JUNIPER_PERIPH_BAR, 0);
	if(!pd->periph_bar)
	{
	  printk(KERN_ERR "JFM: Couldn't take peripheral BAR (%d). Aborting\n", JUNIPER_PERIPH_BAR);
		return -ENODEV;
	}

	pci_set_master(pdev);

	if(pci_enable_msi(pdev))
	{
		printk(KERN_ERR "JFM: Couldn't enable MSI. Aborting\n");
		return -ENODEV;
	}

	init_waitqueue_head(&pd->wait_queue);
	rc = request_irq(pdev->irq, juniper_irqhandler, 0, "juniper_irq", pd);

	if(rc)
	{
		printk(KERN_ERR "JFM: Couldn't request IRQ. Aborting\n");
		return -ENODEV;
	}

	// Set up DMA
	if(!pci_set_dma_mask(pdev, DMA_BIT_MASK(64)))
		pd->dma_using_dac = 1;
	else if(!pci_set_dma_mask(pdev, DMA_BIT_MASK(32)))
		pd->dma_using_dac = 0;
	else
	{
		printk(KERN_ERR "JFM: Failed to set DMA mask\n");
		return -ENODEV;
	}

	// Alloc some DMA-able memory.
	pd->dma_mem_backing = (unsigned int*)__get_free_pages(GFP_KERNEL | __GFP_DMA | __GFP_ZERO, MEM_ORDER);

	if(!pd->dma_mem_backing)
	{
		printk(KERN_ERR "JFM: Failed to allocate DMA mem\n");
		return -ENODEV;
	}

	printk(KERN_INFO "JFM: Got memory at %p. DAC: %d\n", pd->dma_mem_backing, pd->dma_using_dac);

	// Map through to get a DMA handle
	pd->dma_handle = pci_map_single(pdev, pd->dma_mem_backing, PAGE_SIZE * (1 << MEM_ORDER), PCI_DMA_FROMDEVICE);
	if(pci_dma_mapping_error(pdev, pd->dma_handle))
	{
		printk(KERN_ERR "JFM: DMA mapping failed. Aborting\n");
		return -ENODEV;
	}

	// Notify the PCIE core of the DMA mapping.
	iowrite32(((pd->dma_handle >> 32) & 0xFFFFFFFF), &pd->periph_bar[(PCIE_ADDR_BASE + PCIE_OFF_BAR0U) / 4]);
	iowrite32(pd->dma_handle & 0xFFFFFFFF, &pd->periph_bar[(PCIE_ADDR_BASE + PCIE_OFF_BAR0L) / 4]);

	// Set up the DMA controller
	// Enable interrupt on completion
	tmp = ioread32(&pd->periph_bar[(CDMA_ADDR_BASE + CDMA_OFF_CDMACR) / 4]);
	tmp |= (1 << CDMA_CDMACR_IOC_IRQ_BIT);
	iowrite32(tmp, &pd->periph_bar[(CDMA_ADDR_BASE + CDMA_OFF_CDMACR) / 4]);

	// Set the destination to the PCIE base (where it'll always go...)
	tmp = 0x40000000;
	iowrite32(tmp, &pd->periph_bar[(CDMA_ADDR_BASE + CDMA_OFF_DESTADDR) / 4]);

	// Tell the upper level about it
	if(new_device_callback != NULL)
		new_device_callback(pd->externDev);

	return 0; // TODO: Isn't there a macro for this (EOK?)
}

static void juniper_remove(struct pci_dev *pdev)
{
	struct juniper_dev_privdata* pd;
	pd = pci_get_drvdata(pdev);

	if(lost_device_callback != NULL)
		lost_device_callback(pd->externDev);

	pci_unmap_single(pdev, pd->dma_handle, PAGE_SIZE * (1 << MEM_ORDER), PCI_DMA_FROMDEVICE);
	free_pages((unsigned long)pd->dma_mem_backing, MEM_ORDER);

	free_irq(pdev->irq, pd);
	pci_disable_msi(pdev);
	pci_clear_master(pdev);
	pci_iounmap(pdev, pd->periph_bar);
	pci_iounmap(pdev, pd->memory_bar);
	pci_release_regions(pdev);

	kfree(pd->externDev);
	kfree(pd);

	pci_disable_device(pdev);
}

static irqreturn_t juniper_irqhandler(int irq, void* data)
{
	struct juniper_dev_privdata* pd = data;
	//pd->dma_done_flag = 1;
	//wake_up_interruptible(&pd->wait_queue);
	interrupted_callback(pd->externDev);

	printk(KERN_INFO "JFM: INTERRUPT!\n");
	return IRQ_HANDLED;
}

// TODO: Consider de-duplicating the code in the below methods.
unsigned int juniper_pci_read_memory(struct juniper_device* dev, unsigned int address)
{
	struct juniper_dev_privdata* pd;
	pd = pci_get_drvdata(dev->device);

	return ioread32(&pd->memory_bar[address / JUNIPER_PCI_INTF_WIDTH]);
}

void juniper_pci_write_memory(struct juniper_device* dev, unsigned int address, unsigned int value)
{
	struct juniper_dev_privdata* pd;
	pd = pci_get_drvdata(dev->device);

	iowrite32(value, &pd->memory_bar[address / JUNIPER_PCI_INTF_WIDTH]);
}

unsigned int juniper_pci_read_periph(struct juniper_device* dev, unsigned int address)
{
	struct juniper_dev_privdata* pd;
	pd = pci_get_drvdata(dev->device);

	return ioread32(&pd->periph_bar[address / JUNIPER_PCI_INTF_WIDTH]);
}

void juniper_pci_write_periph(struct juniper_device* dev, unsigned int address, unsigned int value)
{
	struct juniper_dev_privdata* pd;
	pd = pci_get_drvdata(dev->device);

	iowrite32(value, &pd->periph_bar[address / JUNIPER_PCI_INTF_WIDTH]);
}

// Consider making these actual DMA later...
void juniper_pci_read_memory_burst(struct juniper_device* dev, unsigned int base_address, unsigned int* data, unsigned int count)
{
	int i = 0;
	while(count--)
	{
		data[i++] = juniper_pci_read_memory(dev, base_address);
		base_address += JUNIPER_PCI_INTF_WIDTH;
	}

  /*	struct juniper_dev_privdata* pd;
	pd = pci_get_drvdata(dev->device);
	base_address &= ~(0xF); // 32-bit align it. TODO: THIS IS WRONG!

	// Tell the DMA core about the source address
	iowrite32(base_address, &pd->registers[(CDMA_ADDR_BASE + CDMA_OFF_SRCADDR) / 4]);

	// Count is in 32-bit multiples, so multiply it back out
	count *= 4;

	// Tell it the count and start the transfer!
	//printk(KERN_INFO "Starting transaction!\n");
	iowrite32(count, &pd->registers[(CDMA_ADDR_BASE + CDMA_OFF_BTT) / 4]);

	// And wait for the results...
	wait_event_interruptible(pd->wait_queue, (pd->dma_done_flag != 0));

	//printk(KERN_INFO "I'M AWAKE!\n");
	if(pd->dma_done_flag == 0)
	{
		printk(KERN_ERR "Er...DMA error...\n");
		return;
	}

	pd->dma_done_flag = 0;

	// First, clear the DMA interrupt
	iowrite32((1 << CDMA_CDMASR_IOC_IRQ_BIT), &pd->registers[(CDMA_ADDR_BASE + CDMA_OFF_CDMASR) / 4]);

	pci_dma_sync_single_for_cpu(dev->device, pd->dma_handle, PAGE_SIZE * (1 << MEM_ORDER), PCI_DMA_FROMDEVICE);
	memcpy((unsigned char*)data, (unsigned char*)pd->dma_mem_backing, count);
	pci_dma_sync_single_for_device(dev->device, pd->dma_handle, PAGE_SIZE * (1 << MEM_ORDER), PCI_DMA_FROMDEVICE);*/
}

void juniper_pci_write_memory_burst(struct juniper_device* dev, unsigned int base_address, unsigned int* data, unsigned int count)
{
	int i = 0;
	while(count--)
	{
		juniper_pci_write_memory(dev, base_address, data[i]);
		base_address += JUNIPER_PCI_INTF_WIDTH;
	}
}
