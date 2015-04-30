CC=gcc
LD=gcc

JUNIPER_LIB_DIR=../../juniper_fpga_interface
JUNIPER_LIB_NAME=juniper_fpga_interface

CFLAGS=-m32 -I$(JUNIPER_LIB_DIR)
LDFLAGS=-m32 -L$(JUNIPER_LIB_DIR) -ljuniper_fpga_interface


OBJS = main.o

all: test_juniper_fpga_interface

test_juniper_fpga_interface: $(OBJS) $(JUNIPER_LIB_DIR)/libjuniper_fpga_interface.so
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(JUNIPER_LIB_DIR)/libjuniper_fpga_interface.so:
	make -C $(JUNIPER_LIB_DIR)

.PHONY: clean

clean:
	rm -rf $(OBJS) *~ test_juniper_fpga_interface
	make -C $(JUNIPER_LIB_DIR) clean
