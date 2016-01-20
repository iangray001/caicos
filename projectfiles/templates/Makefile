include Makefile.inc

C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.o)))

CC_FLAGS := $(M32) -O2 -fno-builtin -I$(JAMAICATARGET)/include -Isrc -Ijuniper_fpga_interface -DNDEBUG 
LD_FLAGS := $(M32) -L$(JAMAICATARGET)/lib -Ljuniper_fpga_interface 
LD_LIBS := -ljuniper_fpga_interface -Bstatic -ljamaica_ -Bdynamic -lm -lpthread -lnsl -ldl -lrt -Bstatic -ljamaica_ -Bdynamic -lm -lpthread -lnsl -ldl -lrt -Wl,--dynamic-list=/home/ian/Documents/jamaica/current32/target/linux-arm-le/etc/exported_symbols.txt

all: mkdirs main

mkdirs:
	mkdir -p obj
 
juniper_fpga_interface/libjuniper_fpga_interface.so:
	cd juniper_fpga_interface && $(MAKE)

malloc_preload/malloc_preload.so:
	cd malloc_preload && $(MAKE)
	
main: juniper_fpga_interface/libjuniper_fpga_interface.so malloc_preload/malloc_preload.so $(OBJ_FILES)
	@echo [LD] main
	@$(LD) $(LD_FLAGS) src/Main__nc.o $(OBJ_FILES) $(LD_LIBS) -o main 

obj/%.o: src/%.c
	@echo [CC] $@
	@$(CC) $(CC_FLAGS) -c -o $@ $<

.PHONY: clean mkdirs

clean:
	cd juniper_fpga_interface && $(MAKE) clean
	cd malloc_preload && $(MAKE) clean
	rm -rf obj
