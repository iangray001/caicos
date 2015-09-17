#!/bin/sh

# This script initiates a rescan of the PCI devices in the system,
# looks for a Xilinx PCI device, and determines its memory address.
# It then uses the mtdblock and phram modules at that address to make the 
# card's memory available to the kernel.
# Must be run as root, and will only handle the first found FPGA.

if [ "$(id -u)" != "0" ]; then
	echo "This script must be run as root" 1>&2
	exit 1
fi

devid=`lspci -mm | grep Xilinx | cut -d ' ' -f 1`
device=/sys/bus/pci/devices/0000:$devid

if [ -z $devid ]; then
	echo "No existing Xilinx device found"
else
	echo "Removing Xilinx device at $device"
	echo 1 > $device/remove
fi

echo "Rescanning PCI..."
echo 1 > /sys/bus/pci/rescan
echo "Xilinx PCI devices:"
lspci | grep Xilinx

addr=`lspci -v | grep $devid -A 2 | grep "Memory at" | cut -d ' ' -f 3`
echo "Card found at address: $addr"

rmmod juniper
rmmod mtdblock
rmmod phram
modprobe mtdblock
modprobe phram phram=JUNIPER2,0x$addr,0x20000000
insmod ../software/host_kernel_module/juniper.ko
