obj-m += juniper.o
juniper-objs := juniper_main.o juniper_pci.o juniper_sysfs.o juniper_interp.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean