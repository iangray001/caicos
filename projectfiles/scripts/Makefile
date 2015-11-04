CC=gcc
LD=gcc
MAKE=make

JAMAICATARGET := $SUB_JAMAICATARGET

C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.o)))

CC_FLAGS := -m32 -O2 -fno-builtin -I$(JAMAICATARGET)/include -Isrc -Ijuniper_fpga_interface -DNDEBUG 
LD_FLAGS := -m32 -L$(JAMAICATARGET)/lib -Ljuniper_fpga_interface -ljuniper_fpga_interface -pthread -lrt -ljamaica_ -lm -ldl

all: mkdirs main

mkdirs:
	mkdir -p obj
 
juniper_fpga_interface/libjuniper_fpga_interface.so:
	cd juniper_fpga_interface && $(MAKE)

malloc_preload/malloc_preload.so:
	cd malloc_preload && $(MAKE)
	
host_kernel_module/juniper.ko:
	cd host_kernel_module && $(MAKE)
	
main: juniper_fpga_interface/libjuniper_fpga_interface.so malloc_preload/malloc_preload.so host_kernel_module/juniper.ko $(OBJ_FILES)
	@echo [LD] main
	@$(LD) $(OBJ_FILES) src/Main__nc.o $(LD_FLAGS) -o main

obj/%.o: src/%.c
	@echo [CC] $@
	@$(CC) $(CC_FLAGS) -c -o $@ $<

.PHONY: clean mkdirs

clean:
	cd juniper_fpga_interface && $(MAKE) clean
	cd malloc_preload && $(MAKE) clean
	cd host_kernel_module && $(MAKE) clean
	rm -rf obj
