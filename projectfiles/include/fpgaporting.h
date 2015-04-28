/*
 * fpgaporting.h
 *
 * Must be imported at the bottom of jamaica.h
 */

#ifndef FPGAPORTING_H_
#define FPGAPORTING_H_

#ifndef __CAICOS__
#include <ap_cint.h>
#endif

//Synthesis options
#define INLINE_MEMORY_ACCESS_FUNCTIONS
#define INLINE_RESOLVE_TREE_ARRAY
#define INLINE_ARRAY_ACCESS_FUNCTIONS
#define JUNIPER_SUPPORT_FLOATS

//Prototypes of porting functions
int __juniper_call(int call_id);
void create_jamaica_thread();
void force_synthesis_of_syscall_interface();


//PCIe system call declarations
typedef struct {
	volatile unsigned char cmd;
	volatile unsigned int arg1;
	volatile unsigned int arg2;
	volatile unsigned int arg3;
	volatile unsigned int arg4;
	volatile unsigned int arg5;
} PCIE_Syscall;

unsigned int pcie_syscall(unsigned char cmd, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5);


//RAM Access functions
jamaica_ref juniper_ram_get_r(int addr, int subwordoffset);
jamaica_int32 juniper_ram_get_i(int addr, int subwordoffset);
jamaica_uint32 juniper_ram_get_ui(int addr, int subwordoffset);
jamaica_int16 juniper_ram_get_s(int addr, int subwordoffset);
jamaica_uint16 juniper_ram_get_us(int addr, int subwordoffset);
jamaica_int8 juniper_ram_get_b(int addr, int subwordoffset);
jamaica_uint8 juniper_ram_get_ub(int addr, int subwordoffset);
jamaica_float juniper_ram_get_f(int addr, int subwordoffset);
jamaica_int64 juniper_ram_get_l(int addr, int subwordoffset);
jamaica_double juniper_ram_get_d(int addr, int subwordoffset);

void juniper_ram_set_r(int addr, int subwordoffset, jamaica_ref r);
void juniper_ram_set_i(int addr, int subwordoffset, jamaica_int32 i);
void juniper_ram_set_ui(int addr, int subwordoffset, jamaica_uint32 ui);
void juniper_ram_set_s(int addr, int subwordoffset, jamaica_int16 s);
void juniper_ram_set_us(int addr, int subwordoffset, jamaica_uint16 us);
void juniper_ram_set_b(int addr, int subwordoffset, jamaica_int8 b);
void juniper_ram_set_ub(int addr, int subwordoffset, jamaica_uint8 ub);
void juniper_ram_set_f(int addr, int subwordoffset, jamaica_float f);
void juniper_ram_set_l(int addr, int subwordoffset, jamaica_int64 val);
void juniper_ram_set_d(int addr, int subwordoffset, jamaica_double val);

//Declarations of global structures
extern volatile int *__juniper_ram_master;
extern volatile char *__juniper_ram_master_char;
extern volatile short *__juniper_ram_master_short;

extern int __juniper_args[];
extern jamaica_thread __juniper_thread;

#ifndef __CAICOS__
extern volatile uint1 syscall_interrupt;
#endif

extern PCIE_Syscall syscall_args;

#endif /* FPGAPORTING_H_ */
