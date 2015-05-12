#ifndef __JUNIPER_PCI_H__
#define __JUNIPER_PCI_H__

#include <linux/pci.h>

struct juniper_device;

// Basically wrappers around pci_(un)register_driver...
int juniper_pci_register(void (*new_device)(struct juniper_device* dev), void (*lost_device)(struct juniper_device* dev), void (*interrupted)(struct juniper_device* dev));
void juniper_pci_unregister(void);

unsigned int juniper_pci_devidx(struct juniper_device* dev);
struct device* juniper_pci_getdev(struct juniper_device* dev);

// Raw I/O
unsigned int juniper_pci_read(struct juniper_device* dev, unsigned int address);
void juniper_pci_write(struct juniper_device* dev, unsigned int address, unsigned int value);

// Burst raw I/O
void juniper_pci_read_burst(struct juniper_device* dev, unsigned int base_address, unsigned int* data, unsigned int count);
void juniper_pci_write_burst(struct juniper_device* dev, unsigned int base_address, unsigned int* data, unsigned int count);

#endif