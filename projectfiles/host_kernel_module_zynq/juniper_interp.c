#include "juniper_interp.h"

#define PCI_CORE_BASE 0x20000000
#define PCI_CORE_GPIO 0x23000000
#define PCI_CORE_GPIO_TRI (PCI_CORE_GPIO + 4)
#define PCI_CORE_ID_SHIFT 20

#define PCI_CORE_START_BIT 0
#define PCI_CORE_IDLE_BIT 2

#define PCI_ICAP_BASE     0x22000000
#define PCI_ICAP_WRFIFO  (PCI_ICAP_BASE + 0x100)
#define PCI_ICAP_CONTROL (PCI_ICAP_BASE + 0x10C)
#define PCI_ICAP_STATUS  (PCI_ICAP_BASE + 0x110)
#define PCI_ICAP_WRFREE  (PCI_ICAP_BASE + 0x114)

#define PCI_ICAP_CONTROL_WRITE 0x1
#define PCI_ICAP_STATUS_DONE   0x1


void juniper_interp_init(struct juniper_fpga_device* dev)
{
	// Set the GPIO as output
	juniper_pci_write(dev->phy_dev, PCI_CORE_GPIO_TRI, 0x0);
	juniper_pci_write(dev->phy_dev, PCI_CORE_GPIO, 0x0); // Remove hold on everything
}

int juniper_interp_get_cores(struct juniper_fpga_device* dev)
{
	return 4; // lol...
}

int juniper_interp_accel_idle(struct juniper_accel_device* dev)
{
	unsigned int status = 0;
	// Query the device
	if(dev->held)
		return 0;

	status = juniper_pci_read(dev->fpga->phy_dev, PCI_CORE_BASE | (dev->idx << PCI_CORE_ID_SHIFT));
	return status & (1 << PCI_CORE_IDLE_BIT);
}

void juniper_interp_accel_start(struct juniper_accel_device* dev)
{
	unsigned int status = 0;

	if(dev->held)
		return;

	status = juniper_pci_read(dev->fpga->phy_dev, PCI_CORE_BASE | (dev->idx << PCI_CORE_ID_SHIFT));
	status |= (1 << PCI_CORE_START_BIT);
	juniper_pci_write(dev->fpga->phy_dev, PCI_CORE_BASE | (dev->idx << PCI_CORE_ID_SHIFT), status);
}

void juniper_interp_accel_hold(struct juniper_accel_device* dev, int hold)
{
	unsigned int hold_mask = dev->fpga->current_hold_mask;

	if(hold)
		hold_mask |= (1 << dev->idx);
	else
		hold_mask &= ~(1 << dev->idx);

	//printk(KERN_INFO "JFM: HOLD MASK: 0x%x\n", hold_mask);

	dev->fpga->current_hold_mask = hold_mask;
	juniper_pci_write(dev->fpga->phy_dev, PCI_CORE_GPIO, hold_mask);
}

// THESE ARE HACKS. FIX THEM LATER.
void juniper_interp_accel_read(struct juniper_accel_device* dev, off_t offset, char* buf, size_t count)
{
	// Right, this is a hack.
	// Fix this nicer later...
	unsigned int* int_buf = (unsigned int*)buf;
	unsigned int_count = count / 4;
	unsigned i = 0;

	//printk(KERN_INFO "JFM: Reading from 0x%x for %d bytes\n", offset, count);

	if(offset % 4 != 0)
		return;
	if(count % 4 != 0)
		return;

	juniper_pci_read_burst(dev->fpga->phy_dev, offset, int_buf, int_count);

	// Because of hacks we're using, we need to endian-swap it.
	for(i = 0; i < int_count; i++)
	{
		unsigned int tmp = 0;
		tmp  = (int_buf[i] >> 24)  & 0xFF;
		tmp |= (int_buf[i] >> 8)   & 0xFF00;
		tmp |= (int_buf[i] << 8)   & 0xFF0000;
		tmp |= (int_buf[i] << 24)  & 0xFF000000;
		int_buf[i] = tmp;
	}
}

void juniper_interp_accel_write(struct juniper_accel_device* dev, off_t offset, char* buf, size_t count)
{
	// Right, this is a hack.
	// Fix this nicer later...
	unsigned int* int_buf = (unsigned int*)buf;
	unsigned int_count = count / 4;
	unsigned i = 0;

	if(offset % 4 != 0)
		return;
	if(count % 4 != 0)
		return;

	//juniper_pci_write_burst(dev->fpga->phy_dev, offset, int_buf, int_count);
	// FIXME: Manually do the burst
	for(i = 0; i < int_count; i++)
	{
		unsigned int tmp = 0;
		tmp  = (int_buf[i] >> 24)  & 0xFF;
		tmp |= (int_buf[i] >> 8)   & 0xFF00;
		tmp |= (int_buf[i] << 8)   & 0xFF0000;
		tmp |= (int_buf[i] << 24)  & 0xFF000000;
		juniper_pci_write(dev->fpga->phy_dev, offset, tmp);
		offset += 4;
	}
}

ssize_t juniper_interp_config_write(struct juniper_fpga_device* dev, off_t offset, char* buf, size_t count)
{
	unsigned int* int_buf = (unsigned int*)buf;
	unsigned int_count = count / 4;
	unsigned i = 0;
	unsigned int avail = 0;

	// How many bytes are available?
	avail = juniper_pci_read(dev->phy_dev, PCI_ICAP_WRFREE);

	//printk(KERN_INFO "JFM: There are %d entries available in the write FIFO\n", avail);
	//printk(KERN_INFO "JFM: It wants to write %d bytes\n", count);

	if(int_count > avail)
		int_count = avail;

	for(i = 0; i < int_count; i++)
	{
		unsigned int tmp = 0;
		tmp  = (int_buf[i] >> 24)  & 0xFF;
		tmp |= (int_buf[i] >> 8)   & 0xFF00;
		tmp |= (int_buf[i] << 8)   & 0xFF0000;
		tmp |= (int_buf[i] << 24)  & 0xFF000000;
		juniper_pci_write(dev->phy_dev, PCI_ICAP_WRFIFO, tmp);
	}

	// Trigger the transaction
	juniper_pci_write(dev->phy_dev, PCI_ICAP_CONTROL, PCI_ICAP_CONTROL_WRITE);
	while(!(juniper_pci_read(dev->phy_dev, PCI_ICAP_STATUS) & PCI_ICAP_STATUS_DONE));
		//printk(KERN_INFO "Status: 0x%x 0x%x\n", juniper_pci_read(dev->phy_dev, PCI_ICAP_CONTROL), juniper_pci_read(dev->phy_dev, PCI_ICAP_STATUS));

	return int_count * 4;
}