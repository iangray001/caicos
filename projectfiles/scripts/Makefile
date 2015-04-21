CC=gcc
LD=gcc
MAKE=make

JAMAICATARGET := /home/ian/Documents/jamaica/current32/target/linux-x86

C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.o)))

CC_FLAGS := -m32 -O2 -I$(JAMAICATARGET)/include -Isrc -Ijuniper_fpga_interface -DNDEBUG 
LD_FLAGS := -m32 -L$JAMAICATARGET/lib -Ljuniper_fpga_interface -ljuniper_fpga_interface -pthread -lrt -ljamaica_ -lm -ldl

all: mkdirs main

mkdirs:
	mkdir -p obj
 
juniper_fpga_interface/libjuniper_fpga_interface.so:
	cd juniper_fpga_interface && $(MAKE)

malloc_preload/malloc_preload.so:
	cd malloc_preload && $(MAKE)
	
main: juniper_fpga_interface/libjuniper_fpga_interface.so malloc_preload/malloc_preload.so $(OBJ_FILES)
	$(LD) $(LD_FLAGS) $(OBJ_FILES) -o main

obj/%.o: src/%.c
	$(CC) $(CC_FLAGS) -c -o $@ $<

.PHONY: clean mkdirs

clean:
	cd juniper_fpga_interface && $(MAKE) clean
	cd malloc_preload && $(MAKE) clean