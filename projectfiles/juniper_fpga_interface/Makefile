include ../Makefile.inc 

CFLAGS=-O2 -fPIC $(M32)
LDFLAGS=-fPIC -shared $(M32)

OBJS=juniper_fpga_interface.o

all: libjuniper_fpga_interface.so

libjuniper_fpga_interface.so: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	@echo [CC] $@
	@$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -rf $(OBJS) libjuniper_fpga_interface.so *~
 
