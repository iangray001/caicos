CC=gcc
LD=gcc

CFLAGS=-O2 -fPIC -m32
LDFLAGS=-fPIC -shared -m32

OBJS=juniper_fpga_interface.o

all: libjuniper_fpga_interface.so

libjuniper_fpga_interface.so: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -rf $(OBJS) libjuniper_fpga_interface.so *~
 
