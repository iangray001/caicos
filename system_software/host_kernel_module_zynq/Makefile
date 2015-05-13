obj-m += juniper.o
juniper-objs := juniper_main.o juniper_pci.o juniper_sysfs.o juniper_interp.o

all:
	make -C ../zc706_linux_buildroot/output/build/linux-xlnx_3.17 ARCH=arm CROSS_COMPILE=arm-xilinx-eabi- M=$(PWD) modules

clean:
	make -C ../zc706_linux_buildroot/output/build/linux-xlnx_3.17 ARCH=arm CROSS_COMPILE=arm-xilinx-eabi- M=$(PWD) clean