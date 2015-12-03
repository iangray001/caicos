#ifndef __JUNIPER_PCI_H__
#define __JUNIPER_PCI_H__

#include <linux/ioport.h>

// Opaque handle presented by juniper_pci. This is to abstract away from the inner device object used by
// juniper_pci.c. All other modules should use the handle provided to them.
struct juniper_device;

// Basically wrappers around pci_(un)register_driver...
int juniper_pci_register(void (*new_device)(struct juniper_device* dev), void (*lost_device)(struct juniper_device* dev), void (*interrupted)(struct juniper_device* dev));
void juniper_pci_unregister(void);

// Get the device number. A unique index for each JUNIPER card.
unsigned int juniper_pci_devidx(struct juniper_device* dev);

// Get a device structure pertaining to the physical hardware device. Useful when creating sysfs attributes.
struct device* juniper_pci_getdev(struct juniper_device* dev);

resource_size_t juniper_pci_memory_size(struct juniper_device* dev);
unsigned int juniper_pci_is_reconfigurable(struct juniper_device* dev);

// Raw I/O
unsigned int juniper_pci_read_memory(struct juniper_device* dev, unsigned int address);
void juniper_pci_write_memory(struct juniper_device* dev, unsigned int address, unsigned int value);
unsigned int juniper_pci_read_periph(struct juniper_device* dev, unsigned int address);
void juniper_pci_write_periph(struct juniper_device* dev, unsigned int address, unsigned int value);

// Burst raw I/O
void juniper_pci_read_memory_burst(struct juniper_device* dev, unsigned int base_address, unsigned int* data, unsigned int count);
void juniper_pci_write_memory_burst(struct juniper_device* dev, unsigned int base_address, unsigned int* data, unsigned int count);

#endif