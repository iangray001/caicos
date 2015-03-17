/*
 * fpgaporting.h
 *
 * Must be imported at the bottom of jamaica.h
 */

#ifndef FPGAPORTING_H_
#define FPGAPORTING_H_

//Prototypes of porting functions
void create_jamaica_thread(jamaica_thread *t);
int __juniper_call(int call_id);


//RAM Access functions
jamaica_ref juniper_ram_get_r(int addr, int subwordoffset);
jamaica_int32 juniper_ram_get_i(int addr, int subwordoffset);
jamaica_uint32 juniper_ram_get_ui(int addr, int subwordoffset);
jamaica_int16 juniper_ram_get_s(int addr, int subwordoffset);
jamaica_uint16 juniper_ram_get_us(int addr, int subwordoffset);
jamaica_int8 juniper_ram_get_b(int addr, int subwordoffset);
jamaica_uint8 juniper_ram_get_ub(int addr, int subwordoffset);
jamaica_float juniper_ram_get_f(int addr, int subwordoffset);

void juniper_ram_set_r(int addr, int subwordoffset, jamaica_ref r);
void juniper_ram_set_i(int addr, int subwordoffset, jamaica_int32 i);
void juniper_ram_set_ui(int addr, int subwordoffset, jamaica_uint32 ui);
void juniper_ram_set_s(int addr, int subwordoffset, jamaica_int16 s);
void juniper_ram_set_us(int addr, int subwordoffset, jamaica_uint16 us);
void juniper_ram_set_b(int addr, int subwordoffset, jamaica_int8 b);
void juniper_ram_set_ub(int addr, int subwordoffset, jamaica_uint8 ub);
void juniper_ram_set_f(int addr, int subwordoffset, jamaica_float f);

//Definitions of global structures
extern volatile int *__juniper_ram_master;
extern int __juniper_args[];
extern jamaica_thread __juniper_thread;

#endif /* FPGAPORTING_H_ */
