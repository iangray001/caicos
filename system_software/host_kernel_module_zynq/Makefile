obj-m += juniper.o
juniper-objs := juniper_main.o juniper_pci.o juniper_sysfs.o juniper_interp.o

KERN_PATH=/home/jamie/juniper/zc706_linux_buildroot/buildroot-2015.05/output/build/linux-4.0.5/

all:
	make -C ${KERN_PATH} ARCH=arm CROSS_COMPILE=arm-xilinx-eabi- M=$(PWD) modules

clean:
	make -C ${KERN_PATH} ARCH=arm CROSS_COMPILE=arm-xilinx-eabi- M=$(PWD) clean