/*
 * fpgaporting.c
 *
 * Contains the implementations of JamaicaVM functions that are required by the FPGA porting layer.
 * Normally these would be implemented by libjamaica, but we cannot use that.
 *
 * Primarily contains:
 *		Direct memory access through the juniper_ram_[get|set]_<type> functions
 * 			Calls to these functions are injected by caicos when it sees accesses to memory in the generated C code.
 * 		Array accesses through jamaicaGC_[Get|Set]Array<type>
 * 			These are already in the C code, and implement contiguous and tree array fetches
 * 		Various functions that are nulled that we do not need in the porting layer (such as exceptions)
 */

#include <jamaica.h>
#include <stdio.h>
#include <stdlib.h>

#include "fpgaporting.h"


//jamaica_GCEnv nullGcEnv = {};

/**
 * We need to pass a jamaica_thread instance around because the generated code uses it.
 * We do not, however, actually need to populate most of it with real information.
 */
void create_jamaica_thread() {
#pragma HLS INLINE
	__juniper_thread.javaStackSize = 1000000;
	__juniper_thread.state = JAMAICA_THREAD_STATE_RUNNING; //FPGA threads are always running
	//__juniper_thread.m.cl = 0; //TODO the locals list
	//__juniper_thread.m.cli = 0; //TODO locals list count
	//__juniper_thread.m.gcenv = &nullGcEnv;
}


#define LOBYTE(a) ((a) & 0xFF)
#define SAR(a, b) (((signed) (a))>>(b)) //Assumes a is int type


//We cannot run out of stack when it is in hardware!
jamaica_bool jamaicaThreads_checkCStackOverflow(jamaica_thread *ct) {
	return FALSE;
}


jamaica_ref juniper_ram_get_r(int addr, int subwordoffset) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
	return __juniper_ram_master[addr];
}

jamaica_int32 juniper_ram_get_i(int addr, int subwordoffset) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
	return __juniper_ram_master[addr];
}

jamaica_uint32 juniper_ram_get_ui(int addr, int subwordoffset) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
	return __juniper_ram_master[addr];
}

jamaica_int16 juniper_ram_get_s(int addr, int subwordoffset) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_short bundle=MAXI offset=slave
	return __juniper_ram_master_short[addr+subwordoffset];
}

jamaica_uint16 juniper_ram_get_us(int addr, int subwordoffset) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_short bundle=MAXI offset=slave
	return __juniper_ram_master_short[addr+subwordoffset];
}

jamaica_int8 juniper_ram_get_b(int addr, int subwordoffset) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_char bundle=MAXI offset=slave
	return __juniper_ram_master_char[addr+subwordoffset];
}

jamaica_uint8 juniper_ram_get_ub(int addr, int subwordoffset) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_char bundle=MAXI offset=slave
	return __juniper_ram_master_char[addr+subwordoffset];
}

jamaica_float juniper_ram_get_f(int addr, int subwordoffset) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#ifdef JUNIPER_SUPPORT_FLOATS
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
	int temp = __juniper_ram_master[addr];
	return *(float *)&temp;
#else
	return 0;
#endif
}

void juniper_ram_set_r(int addr, int subwordoffset, jamaica_ref r) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
	__juniper_ram_master[addr] = r;
}


void juniper_ram_set_i(int addr, int subwordoffset, jamaica_int32 i) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
	__juniper_ram_master[addr] = i;
}


void juniper_ram_set_ui(int addr, int subwordoffset, jamaica_uint32 ui) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
	__juniper_ram_master[addr] = ui;
}

void juniper_ram_set_s(int addr, int subwordoffset, jamaica_int16 s) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_short bundle=MAXI offset=slave
	__juniper_ram_master_short[addr+subwordoffset] = s;
}

void juniper_ram_set_us(int addr, int subwordoffset, jamaica_uint16 us) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_short bundle=MAXI offset=slave
	__juniper_ram_master_short[addr+subwordoffset] = us;
}

void juniper_ram_set_b(int addr, int subwordoffset, jamaica_int8 b) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_char bundle=MAXI offset=slave
	__juniper_ram_master_char[addr+subwordoffset] = b;
}

void juniper_ram_set_ub(int addr, int subwordoffset, jamaica_uint8 ub) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#pragma HLS INTERFACE m_axi port=__juniper_ram_master_char bundle=MAXI offset=slave
	__juniper_ram_master_char[addr+subwordoffset] = ub;
}

void juniper_ram_set_f(int addr, int subwordoffset, jamaica_float f) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
#ifdef JUNIPER_SUPPORT_FLOATS
#pragma HLS INTERFACE m_axi port=__juniper_ram_master bundle=MAXI offset=slave
	__juniper_ram_master[addr] = *(int *)&f;
#endif
}


jamaica_int64 juniper_ram_get_l(int addr, int subwordoffset) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
	int lw = __juniper_ram_master[addr];
	int hw = __juniper_ram_master[addr+1];
	return (((long long) hw) << 32) | ((long long) lw);
}

void juniper_ram_set_l(int addr, int subwordoffset, jamaica_int64 val) {
#ifdef INLINE_MEMORY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
	int lw = (int) (val & 0xFFFFFFFF);
	int hw = (int) ((val & 0xFFFFFFFF00000000) >> 32);
	__juniper_ram_master[addr] = lw;
	__juniper_ram_master[addr+1] = hw;
}

jamaica_double juniper_ram_get_d(int addr, int subwordoffset) {
	int lw = __juniper_ram_master[addr];
	int hw = __juniper_ram_master[addr+1];
	long long tmp = (((long long) hw) << 32) | ((long long) lw);
	return *(double *)&tmp;
}

void juniper_ram_set_d(int addr, int subwordoffset, jamaica_double val) {
	long long tmp = *(long long *)&val;
	__juniper_ram_master[addr] = (int) (tmp & 0xFFFFFFFF);
	__juniper_ram_master[addr+1] = (int) ((tmp & 0xFFFFFFFF00000000) >> 32);
}


/*
 * Jamaica stores arrays as trees when memory gets fragmented. When accessing an array, it is necessary
 * to walk down the tree to the correct leaf first.
 * byteoffset should be 0 if the array is storing 32-bit values, 1 for 16-bit, and 2 for 8-bit.
 */
int resolve_tree_array(int ref, int *idx, unsigned char byteoffset) {
#ifdef INLINE_RESOLVE_TREE_ARRAY
#pragma HLS INLINE
#endif
	int segment, ecx;
	segment = ref + 4;
	ecx = (__juniper_ram_master[ref + 3] & 0xF) * 3;
	while(ecx != 0x0) {
		segment = __juniper_ram_master[segment + SAR(*idx, LOBYTE(ecx + byteoffset))] / 4;
		*idx = *idx - (SAR(*idx, LOBYTE(ecx + byteoffset)) << LOBYTE(ecx + byteoffset));
		ecx = ecx - 0x3;
	}
	return segment;
}


jamaica_int8 jamaicaGC_GetArray8(jamaica_ref b, jamaica_int32 i) {
#ifdef INLINE_ARRAY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
	int segment = resolve_tree_array(b, &i, 2);
	return __juniper_ram_master_char[segment*4 + i];
}

void jamaicaGC_SetArray8(jamaica_ref b, jamaica_int32 i, jamaica_int8 v) {
#ifdef INLINE_ARRAY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
	int segment = resolve_tree_array(b, &i, 2);
	__juniper_ram_master_char[segment*4 + i] = v;
}

jamaica_int16 jamaicaGC_GetArray16(jamaica_ref b, jamaica_int32 i) {
#ifdef INLINE_ARRAY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
	int segment = resolve_tree_array(b, &i, 1);
	return __juniper_ram_master_short[segment*2 + i];
}


void jamaicaGC_SetArray16(jamaica_ref b, jamaica_int32 i, jamaica_int16 v) {
#ifdef INLINE_ARRAY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
	int segment = resolve_tree_array(b, &i, 1);
	__juniper_ram_master_short[segment*2 + i] = v;
}

jamaica_int32 jamaicaGC_GetArray32(jamaica_ref b, jamaica_int32 i) {
#ifdef INLINE_ARRAY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
	int segment = resolve_tree_array(b, &i, 0);
	return __juniper_ram_master[segment + i];
}

void jamaicaGC_SetArray32(jamaica_ref b, jamaica_int32 i, jamaica_int32 v) {
#ifdef INLINE_ARRAY_ACCESS_FUNCTIONS
#pragma HLS INLINE
#endif
	int segment = resolve_tree_array(b, &i, 0);
	__juniper_ram_master[segment + i] = v;
}



jamaica_int64 jamaicaGC_GetArray64(jamaica_ref b, jamaica_int32 i) {return 0;}
void jamaicaGC_SetArray64(jamaica_ref b, jamaica_int32 i, jamaica_int64 v) {}
jamaica_double jamaicaGC_GetArrayDouble(jamaica_ref b, jamaica_int32 i) {return 0.0;}
void jamaicaGC_SetArrayDouble(jamaica_ref b, jamaica_int32 i, jamaica_double v) {}
jamaica_float jamaicaGC_GetArrayFloat(jamaica_ref b, jamaica_int32 i) {return 0.0;}
void jamaicaGC_SetArrayFloat(jamaica_ref b, jamaica_int32 i, jamaica_float v) {}


jamaica_int32 jamaicaGC_GetArray32Ref(jamaica_ref b, jamaica_int32 i) {return (jamaica_int32) 0;}
void jamaicaGC_SetArray32Ref(jamaica_ref b, jamaica_int32 i, jamaica_int32 v) {}

void jamaicaGC_SetRefArray(jamaica_thread *ct, jamaica_ref b, jamaica_int32 i, jamaica_ref r) {}
jamaica_ref jamaicaGC_GetRefArray(jamaica_ref b, jamaica_int32 i) {return JAMAICA_NULL;}

jamaica_int32 jamaicaGC_GetArrayLength(jamaica_ref b) {
	return __juniper_ram_master[b+JAMAICA_ARRAY_LENGTH_DEPTH] >> JAMAICA_ARRAY_LENGTH_SHIFT;
}




/**
 * Currently these functions are nulled because either they will not be called from hardware,
 * or they are not required to do anything.
 */

jamaica_ref jamaicaGC_PlainAllocHeadBlock(jamaica_thread *ct, jamaica_uint32 refs) {
	return (jamaica_ref) 0;
}

jamaica_bool jamaicaGC_haltFunction(const char *function, const char *filename, unsigned long lineNb, const char *text) {
	return TRUE;
}

jamaica_ref jamaicaInterpreter_allocJavaObject(jamaica_thread *ct,jamaica_ref cl) {
	return JAMAICA_NULL;
}

jamaica_ref jamaicaInterpreter_allocSimpleArray(jamaica_thread *ct,jamaica_int32 sz, jamaica_ref cl) {
	return JAMAICA_NULL;
}

jamaica_ref jamaicaInterpreter_allocMultiArray(jamaica_thread *ct,jamaica_int32 *s,jamaica_int32 first_dim,jamaica_int32 dimensions, jamaica_ref cl) {
	return JAMAICA_NULL;
}

jamaica_int32 jamaicaInterpreter_initialize_class_helper(jamaica_thread *ct, jamaica_ref clazz, jamaica_int32  startpc) {
	return 0;
}

void jamaicaInterpreter_enterMonitor(jamaica_thread *ct, jamaica_ref obj) {

}

void jamaicaInterpreter_exitMonitor(jamaica_thread *ct, jamaica_ref obj) {

}

/*
 * This is currently a problem because even if we could do the resolution, HLS cannot call an arbitrary function pointer.
 */
jamaica_ref jamaicaInterpreter_getInterfaceMethod(jamaica_thread *ct, jamaica_ref target, jamaica_ref method) {
	return 0;
}




jamaica_int32 jamaicaInterpreter_castDoubleToInteger(jamaica_double d) {
	//TODO Test
	return (jamaica_int32) d;
}

jamaica_int64 jamaicaInterpreter_castDoubleToLong(jamaica_double d) {
	//TODO Test
	return (jamaica_int64) d;
}

void jamaicaScheduler_syncPointFull(jamaica_thread *ct) {
	//This is removed because we won't need it in hardware
}


/*
 * In the original Jamiaca, these are marked with JAMAICA_FUNCTION_ATTRIBUTE(noreturn)
 * which is a GCC extension because exceptions are implemented with setjmp/longjmp.
 *
 * The current version of caicos does not support exceptions in the translated hardware.
 *
 * GCC will warn if a function is marked with noreturn, and it can actually return.
 * It can be told to ignore this by wrapping the functions in:
 *   #pragma GCC diagnostic push
 *   #pragma GCC diagnostic ignored "-Winvalid-noreturn"
 *   ...
 *   #pragma GCC diagnostic pop
 */
//#define CAICOS_EXCEPTIONPRINT(s) printf(s);
#define CAICOS_EXCEPTIONPRINT(s)
void jamaica_clearRefsOnExc(jamaica_thread *ct, jamaica_int32 tp) {}
void jamaica_propagate_exception(jamaica_thread *ct) {}
void jamaicaExceptions_throw(jamaica_thread *ct, jamaica_ref exception) {CAICOS_EXCEPTIONPRINT("jamaicaExceptions_throw")}
void jamaica_throw_class(jamaica_thread *ct, jamaica_ref clazz, jamaica_ref msg) {CAICOS_EXCEPTIONPRINT("jamaica_throw_class\n")}
void jamaica_throwNullPtrExc(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwNullPtrExc\n")}
void jamaica_throwIncompClassChangeErr(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwIncompClassChangeErr\n")}
void jamaica_throwAbstrMethodErr(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwAbstrMethodErr\n")}
void jamaica_throwClassCastExc(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwClassCastExc\n")}
void jamaica_throwArrIdxOutOfBndsExc(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwArrIdxOutOfBndsExc\n")}
void jamaica_throwArrIdxOutOfBndsExcMsg(jamaica_thread *ct, jamaica_ref msg) {CAICOS_EXCEPTIONPRINT("jamaica_throwArrIdxOutOfBndsExcMsg\n")}
void jamaica_throwOutOfMemErr(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwOutOfMemErr\n")}
void jamaica_throwNegArrSzExc(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwNegArrSzExc\n")}
void jamaica_throwArithmExc(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwArithmExc\n")}
void jamaica_throwIllMonitorStateExc(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwIllMonitorStateExc\n")}
void jamaica_throwArrStoreExc(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwArrStoreExc\n")}
void jamaica_throwNoClassDefFndErr(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwNoClassDefFndErr\n")}
void jamaica_throwInterruptedExc(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwInterruptedExc\n")}
void jamaica_throwInternalErr(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwInternalErr\n")}
void jamaica_throwInternalErrMsg(jamaica_thread *ct, jamaica_ref msg) {CAICOS_EXCEPTIONPRINT("jamaica_throwInternalErrMsg\n")}
void jamaica_throwInternalErrcmsg(jamaica_thread *ct, const char *msg) {CAICOS_EXCEPTIONPRINT("jamaica_throwInternalErrcmsg\n")}
void jamaica_throwIllArgExccmsg(jamaica_thread *ct, const char *msg) {CAICOS_EXCEPTIONPRINT("jamaica_throwIllArgExccmsg\n")}
void jamaica_throwStringIdxOutOfBndsExc(jamaica_thread *ct, jamaica_int32 index, jamaica_int32 lowerBound, jamaica_int32  upperBound) {CAICOS_EXCEPTIONPRINT("jamaica_throwStringIdxOutOfBndsExc\n")}
void jamaica_throwIllAssgmtErr(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwIllAssgmtErr\n")}
void jamaica_throwStackOvrflwErr(jamaica_thread *ct) {CAICOS_EXCEPTIONPRINT("jamaica_throwStackOvrflwErr\n")}



/*INLINE_FUNCTION jamaica_bool jamaicaThreads_checkCStackOverflow(jamaica_thread *ct)
{
  #if defined(JAMAICA_WITH_CHECK_C_STACK_OVERFLOW) || defined(JAMAICA_PROFILE)
    jamaica_int32 remaining_c_stack;
    assert(ct != NULL);
    remaining_c_stack = jamaicaThreads_remainingCStack(ct);
    #ifdef JAMAICA_PROFILE
      if (ct->min_remaining_c_stack > remaining_c_stack)
        {
          ct->min_remaining_c_stack = remaining_c_stack;
        }
    #endif
    #ifdef JAMAICA_WITH_CHECK_C_STACK_OVERFLOW
      return remaining_c_stack <= 0;
    #else 
      return FALSE;
    #endif
  #else
    JAMAICA_VARIABLE_UNUSED(ct);
    return FALSE;
  #endif
}*/

/*INLINE_FUNCTION jamaica_int32 jamaicaThreads_remainingCStack(jamaica_thread *ct)
{
  jamaica_int32 result;
  #if defined(JAMAICA_WITH_CHECK_C_STACK_OVERFLOW) || defined(JAMAICA_PROFILE)
    volatile char var;
    assert(ct              != NULL);
    assert(ct->cStackLimit != NULL);
    result = JAMAICA_STACK_GROWS_UP
      ? ((char*)ct->cStackLimit - &(var))
      : (&(var) - (char*)ct->cStackLimit);
  #else
    JAMAICA_VARIABLE_UNUSED(ct);
    result = 1;
  #endif
  return result;
}*/
