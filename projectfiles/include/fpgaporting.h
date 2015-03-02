/*
 * fpgaporting.h
 *
 * Must be imported at the bottom of jamaica.h
 */

#ifndef FPGAPORTING_H_
#define FPGAPORTING_H_

void create_jamaica_thread(jamaica_thread *t);

//RAM Access functions
jamaica_ref juniper_ram_fetch_r(int addr, int subwordoffset);
jamaica_int32 juniper_ram_fetch_i(int addr, int subwordoffset);
jamaica_uint32 juniper_ram_fetch_ui(int addr, int subwordoffset);
jamaica_int16 juniper_ram_fetch_s(int addr, int subwordoffset);
jamaica_uint16 juniper_ram_fetch_us(int addr, int subwordoffset);
jamaica_int8 juniper_ram_fetch_b(int addr, int subwordoffset);
jamaica_uint8 juniper_ram_fetch_ub(int addr, int subwordoffset);
jamaica_float juniper_ram_fetch_f(int addr, int subwordoffset);

extern volatile jamaica_data32 *__juniper_ram_master;

#endif /* FPGAPORTING_H_ */
