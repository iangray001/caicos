#include "juniper_interp.h"

/*
 * These offsets can be determined by running scripts/getoffsets.py
 */
#define PCI_CORE_BASE 0x00000000
#define PCI_CORE_ACCEL_BASE 0x00000000
#define PCI_CORE_ACCEL_RETVAL 0x00000010
#define PCI_CORE_ACCEL_MASTER0 0x00000018
#define PCI_CORE_ACCEL_MASTER1 0x00000020
#define PCI_CORE_ACCEL_MASTER2 0x00000028
#define PCI_CORE_ACCEL_ARG0 0x00000030
#define PCI_CORE_ACCEL_ARG1 0x00000038
#define PCI_CORE_ACCEL_ARG2 0x00000040
#define PCI_CORE_ACCEL_ARG3 0x00000048
#define PCI_CORE_ACCEL_SYSCALL_ARG0 0x00000060
#define PCI_CORE_ACCEL_SYSCALL_ARG1 0x00000050
#define PCI_CORE_ACCEL_SYSCALL_ARG2 0x00000070
#define PCI_CORE_ACCEL_SYSCALL_ARG3 0x00000080
#define PCI_CORE_ACCEL_SYSCALL_ARG4 0x00000090
#define PCI_CORE_ACCEL_SYSCALL_ARG5 0x000000a0
#define PCI_CORE_ACCEL_SYSCALL_OUT_ARG0 0x00000068
#define PCI_CORE_ACCEL_SYSCALL_OUT_ARG1 0x00000058
#define PCI_CORE_ACCEL_SYSCALL_OUT_ARG2 0x00000078
#define PCI_CORE_ACCEL_SYSCALL_OUT_ARG3 0x00000088
#define PCI_CORE_ACCEL_SYSCALL_OUT_ARG4 0x00000098
#define PCI_CORE_ACCEL_SYSCALL_OUT_ARG5 0x000000a8

#define PCI_CORE_GPIO 0x03000000
#define PCI_CORE_GPIO_TRI (PCI_CORE_GPIO + 4)
#define PCI_CORE_ID_SHIFT 20

#define PCI_CORE_START_BIT 0
#define PCI_CORE_DONE_BIT 1
#define PCI_CORE_IDLE_BIT 2

#define PCI_ICAP_BASE     0x02000000
#define PCI_ICAP_WRFIFO  (PCI_ICAP_BASE + 0x100)
#define PCI_ICAP_CONTROL (PCI_ICAP_BASE + 0x10C)
#define PCI_ICAP_STATUS  (PCI_ICAP_BASE + 0x110)
#define PCI_ICAP_WRFREE  (PCI_ICAP_BASE + 0x114)

#define PCI_ICAP_CONTROL_WRITE 0x1
#define PCI_ICAP_STATUS_DONE   0x1

#define PCI_INTC_BASE 0x06000000
#define PCI_INTC_ISR  (PCI_INTC_BASE + 0x0)
#define PCI_INTC_IPR  (PCI_INTC_BASE + 0x4)
#define PCI_INTC_IER  (PCI_INTC_BASE + 0x8)
#define PCI_INTC_IAR  (PCI_INTC_BASE + 0xC)
#define PCI_INTC_SIE  (PCI_INTC_BASE + 0x10)
#define PCI_INTC_SIE  (PCI_INTC_BASE + 0x10)
#define PCI_INTC_CIE  (PCI_INTC_BASE + 0x14)
#define PCI_INTC_IVR  (PCI_INTC_BASE + 0x18)
#define PCI_INTC_MER  (PCI_INTC_BASE + 0x1C)
#define PCI_INTC_IMR  (PCI_INTC_BASE + 0x20)
#define PCI_INTC_ILR  (PCI_INTC_BASE + 0x24)

// Number of interrupts to "skip"
// This currently corresponds to the DMA interrupt.
#define PCI_ICAP_RESERVED_INTERRUPTS 1

void juniper_interp_init(struct juniper_fpga_device* dev)
{
	int numIntr = 0;
	uint32_t intrMask = 0x0;
	int i = 0;

	printk(KERN_INFO "JFM: INTRP: INIT\n");

	// Set the GPIO as output
	// Only need to do this for devices that support PR
	if(juniper_pci_is_reconfigurable(dev->phy_dev))
	{
		juniper_pci_write_periph(dev->phy_dev, PCI_CORE_GPIO_TRI, 0x0);
		juniper_pci_write_periph(dev->phy_dev, PCI_CORE_GPIO, 0x0); // Remove hold on everything
	}

	// Set up the interrupt controller
	// The PCI layer should have initialised MSI by this point
	numIntr = juniper_interp_get_cores(dev);
	numIntr += PCI_ICAP_RESERVED_INTERRUPTS;

	// Build a mask with that many bits
	for(i = 0; i < numIntr; i++)
	{
		intrMask <<= 1;
		intrMask |= 0x1;
	}

	// Write that to the enable register
	juniper_pci_write_periph(dev->phy_dev, PCI_INTC_IER, intrMask);

	// Set ME and HIE bits in the master interrupt enable
	juniper_pci_write_periph(dev->phy_dev, PCI_INTC_MER, 0x3);
}

int juniper_interp_get_cores(struct juniper_fpga_device* dev)
{
	return 1; // lol...
}

int juniper_interp_accel_idle(struct juniper_accel_device* dev)
{
	unsigned int status = 0;
	// Query the device
	if(dev->held)
		return 0;

	status = juniper_pci_read_periph(dev->fpga->phy_dev, PCI_CORE_ACCEL_BASE | (dev->idx << PCI_CORE_ID_SHIFT));
	return status & (1 << PCI_CORE_IDLE_BIT);
}

void juniper_interp_accel_start(struct juniper_accel_device* dev)
{
	unsigned int status = 0;

	if(dev->held)
		return;

	status = juniper_pci_read_periph(dev->fpga->phy_dev, PCI_CORE_ACCEL_BASE | (dev->idx << PCI_CORE_ID_SHIFT));
	status |= (1 << PCI_CORE_START_BIT);
	juniper_pci_write_periph(dev->fpga->phy_dev, PCI_CORE_ACCEL_BASE | (dev->idx << PCI_CORE_ID_SHIFT), status);
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
	juniper_pci_write_periph(dev->fpga->phy_dev, PCI_CORE_GPIO, hold_mask);
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

	juniper_pci_read_memory_burst(dev->fpga->phy_dev, offset, int_buf, int_count);

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
		juniper_pci_write_memory(dev->fpga->phy_dev, offset, tmp);
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
	avail = juniper_pci_read_periph(dev->phy_dev, PCI_ICAP_WRFREE);

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
		juniper_pci_write_periph(dev->phy_dev, PCI_ICAP_WRFIFO, tmp);
	}

	// Trigger the transaction
	juniper_pci_write_periph(dev->phy_dev, PCI_ICAP_CONTROL, PCI_ICAP_CONTROL_WRITE);
	while(!(juniper_pci_read_periph(dev->phy_dev, PCI_ICAP_STATUS) & PCI_ICAP_STATUS_DONE));
		//printk(KERN_INFO "Status: 0x%x 0x%x\n", juniper_pci_read(dev->phy_dev, PCI_ICAP_CONTROL), juniper_pci_read(dev->phy_dev, PCI_ICAP_STATUS));

	return int_count * 4;
}

uint32_t juniper_interp_get_arg(struct juniper_accel_device* dev, int argN)
{
	// We just need to read from the PCI-E device...
	// Figure out the address
	unsigned int addr = 0;

	switch(argN) {
		case 0: addr = PCI_CORE_ACCEL_ARG0; break;
		case 1: addr = PCI_CORE_ACCEL_ARG1; break;
		case 2: addr = PCI_CORE_ACCEL_ARG2; break;
		case 3: addr = PCI_CORE_ACCEL_ARG3; break;
	}

	addr |= dev->idx << PCI_CORE_ID_SHIFT;
	return juniper_pci_read_periph(dev->fpga->phy_dev, addr);
}

void juniper_interp_set_arg(struct juniper_accel_device* dev, int argN, uint32_t val)
{
	unsigned int addr = 0;

	switch(argN) {
		case 0: addr = PCI_CORE_ACCEL_ARG0; break;
		case 1: addr = PCI_CORE_ACCEL_ARG1; break;
		case 2: addr = PCI_CORE_ACCEL_ARG2; break;
		case 3: addr = PCI_CORE_ACCEL_ARG3; break;
	}

	addr |= dev->idx << PCI_CORE_ID_SHIFT;
	juniper_pci_write_periph(dev->fpga->phy_dev, addr, val);
}

uint32_t juniper_interp_get_syscall_arg(struct juniper_accel_device* dev, int argN)
{
	// We just need to read from the PCI-E device...
	// Figure out the address
	unsigned int addr = 0;

	switch(argN) {
		case 1: addr = PCI_CORE_ACCEL_SYSCALL_ARG1; break;
		case 2: addr = PCI_CORE_ACCEL_SYSCALL_ARG2; break;
		case 3: addr = PCI_CORE_ACCEL_SYSCALL_ARG3; break;
		case 4: addr = PCI_CORE_ACCEL_SYSCALL_ARG4; break;
		case 5: addr = PCI_CORE_ACCEL_SYSCALL_ARG5; break;
	}
	addr |= dev->idx << PCI_CORE_ID_SHIFT;
	return juniper_pci_read_periph(dev->fpga->phy_dev, addr);
}

void juniper_interp_set_syscall_arg(struct juniper_accel_device* dev, int argN, uint32_t val)
{
	unsigned int addr = 0;

	switch(argN) {
		case 1: addr = PCI_CORE_ACCEL_SYSCALL_OUT_ARG1; break;
		case 2: addr = PCI_CORE_ACCEL_SYSCALL_OUT_ARG2; break;
		case 3: addr = PCI_CORE_ACCEL_SYSCALL_OUT_ARG3; break;
		case 4: addr = PCI_CORE_ACCEL_SYSCALL_OUT_ARG4; break;
		case 5: addr = PCI_CORE_ACCEL_SYSCALL_OUT_ARG5; break;
	}
	addr |= dev->idx << PCI_CORE_ID_SHIFT;
	juniper_pci_write_periph(dev->fpga->phy_dev, addr, val);
}

uint32_t juniper_interp_get_ram_base(struct juniper_accel_device* dev)
{
	unsigned int addr = PCI_CORE_ACCEL_MASTER0;
	addr |= dev->idx << PCI_CORE_ID_SHIFT;

	return juniper_pci_read_periph(dev->fpga->phy_dev, addr);
}

void juniper_interp_set_ram_base(struct juniper_accel_device* dev, uint32_t val)
{
	juniper_pci_write_periph(dev->fpga->phy_dev, PCI_CORE_ACCEL_MASTER0 | (dev->idx << PCI_CORE_ID_SHIFT), val);
	juniper_pci_write_periph(dev->fpga->phy_dev, PCI_CORE_ACCEL_MASTER1 | (dev->idx << PCI_CORE_ID_SHIFT), val);
	juniper_pci_write_periph(dev->fpga->phy_dev, PCI_CORE_ACCEL_MASTER2 | (dev->idx << PCI_CORE_ID_SHIFT), val);
}

uint32_t juniper_interp_get_retVal(struct juniper_accel_device* dev)
{
	unsigned int addr = PCI_CORE_ACCEL_RETVAL;
	addr |= dev->idx << PCI_CORE_ID_SHIFT;

	return juniper_pci_read_periph(dev->fpga->phy_dev, addr);
}

uint32_t juniper_interp_get_interrupt_id(struct juniper_fpga_device* dev)
{
	uint32_t active_interrupt = -1;

	active_interrupt = juniper_pci_read_periph(dev->phy_dev, PCI_INTC_IVR);

	if(active_interrupt == -1)
		return -1;

	// TODO: Handle this properly...
	if(active_interrupt < PCI_ICAP_RESERVED_INTERRUPTS)
		return -1;

	return active_interrupt - PCI_ICAP_RESERVED_INTERRUPTS;
}

void juniper_interp_ack_interrupt(struct juniper_fpga_device* dev, uint32_t intrId)
{
	// Figure out the correct bit to write
	uint32_t ackMask = 0;
	
	// Make space for the reserved interruptes
	intrId += PCI_ICAP_RESERVED_INTERRUPTS;
	ackMask = (1 << intrId);

	juniper_pci_write_periph(dev->phy_dev, PCI_INTC_IAR, ackMask);
}
