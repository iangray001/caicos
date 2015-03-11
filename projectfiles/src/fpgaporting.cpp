/*
 * fpgaporting.c
 *
 *  Created on: 16 Jul 2014
 *      Author: ian
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
void create_jamaica_thread(jamaica_thread *t) {
	t->javaStackSize = 1000000; //TODO Set to something sensible
	t->state = JAMAICA_THREAD_STATE_RUNNING; //FPGA threads are always running
	//t->m.cl = 0; //TODO the locals list
	//t->m.cli = 0; //TODO locals list count
	//t->m.gcenv = &nullGcEnv;
}




INLINE_FUNCTION jamaica_bool jamaicaThreads_checkCStackOverflow(jamaica_thread *ct) {
	return FALSE;
}


jamaica_ref juniper_ram_get_r(int addr, int subwordoffset) {return 0;}
jamaica_int32 juniper_ram_get_i(int addr, int subwordoffset) {return 0;}
jamaica_uint32 juniper_ram_get_ui(int addr, int subwordoffset) {return 0;}
jamaica_int16 juniper_ram_get_s(int addr, int subwordoffset) {return 0;}
jamaica_uint16 juniper_ram_get_us(int addr, int subwordoffset) {return 0;}
jamaica_int8 juniper_ram_get_b(int addr, int subwordoffset) {return 0;}
jamaica_uint8 juniper_ram_get_ub(int addr, int subwordoffset) {return 0;}
jamaica_float juniper_ram_get_f(int addr, int subwordoffset) {return 0;}

void juniper_ram_set_r(int addr, int subwordoffset, jamaica_ref r) {}
void juniper_ram_set_i(int addr, int subwordoffset, jamaica_int32 i) {}
void juniper_ram_set_ui(int addr, int subwordoffset, jamaica_uint32 ui) {}
void juniper_ram_set_s(int addr, int subwordoffset, jamaica_int16 s) {}
void juniper_ram_set_us(int addr, int subwordoffset, jamaica_uint16 us) {}
void juniper_ram_set_b(int addr, int subwordoffset, jamaica_int8 b) {}
void juniper_ram_set_ub(int addr, int subwordoffset, jamaica_uint8 ub) {}
void juniper_ram_set_f(int addr, int subwordoffset, jamaica_float f) {}


jamaica_int8 jamaicaGC_GetArray8(jamaica_ref b, jamaica_int32 i) {return 0;}
jamaica_int8 jamaicaGC_GetArray8_d0(jamaica_ref b, jamaica_int32 i) {return 0;}
void jamaicaGC_SetArray8(jamaica_ref b, jamaica_int32 i, jamaica_int8 v) {}
void jamaicaGC_SetArray8_d0(jamaica_ref b, jamaica_int32 i, jamaica_int8 v) {}

jamaica_int16 jamaicaGC_GetArray16(jamaica_ref b, jamaica_int32 i) {return 0;}
jamaica_int16 jamaicaGC_GetArray16_d0(jamaica_ref b, jamaica_int32 i) {return 0;}
void jamaicaGC_SetArray16(jamaica_ref b, jamaica_int32 i, jamaica_int16 v) {}
void jamaicaGC_SetArray16_d0(jamaica_ref b, jamaica_int32 i, jamaica_int16 v) {}

jamaica_int32 jamaicaGC_GetArray32(jamaica_ref b, jamaica_int32 i) {
	//If a contiguous array:
	return __juniper_ram_master[b + 4 + i].i;

	//return b->data[i+4].i;
}

jamaica_int32 jamaicaGC_GetArray32_d0(jamaica_ref b, jamaica_int32 i) {
	//If a contiguous array:
	return __juniper_ram_master[b + 4 + i].i;
}

void jamaicaGC_SetArray32(jamaica_ref b, jamaica_int32 i, jamaica_int32 v) {}
void jamaicaGC_SetArray32_d0(jamaica_ref b, jamaica_int32 i, jamaica_int32 v) {}

jamaica_int64 jamaicaGC_GetArray64(jamaica_ref b, jamaica_int32 i) {return 0;}
jamaica_int64 jamaicaGC_GetArray64_d0(jamaica_ref b, jamaica_int32 i) {return 0;}
void jamaicaGC_SetArray64(jamaica_ref b, jamaica_int32 i, jamaica_int64 v) {}
void jamaicaGC_SetArray64_d0(jamaica_ref b, jamaica_int32 i, jamaica_int64 v) {}

jamaica_double jamaicaGC_GetArrayDouble(jamaica_ref b, jamaica_int32 i) {return 0.0;}
jamaica_double jamaicaGC_GetArrayDouble_d0(jamaica_ref b, jamaica_int32 i) {return 0.0;}
void jamaicaGC_SetArrayDouble(jamaica_ref b, jamaica_int32 i, jamaica_double v) {}
void jamaicaGC_SetArrayDouble_d0(jamaica_ref b, jamaica_int32 i, jamaica_double v) {}

jamaica_float jamaicaGC_GetArrayFloat(jamaica_ref b, jamaica_int32 i) {return 0.0;}
jamaica_float jamaicaGC_GetArrayFloat_d0(jamaica_ref b, jamaica_int32 i) {return 0.0;}
void jamaicaGC_SetArrayFloat(jamaica_ref b, jamaica_int32 i, jamaica_float v) {}
void jamaicaGC_SetArrayFloat_d0(jamaica_ref b, jamaica_int32 i, jamaica_float v) {}

jamaica_int32 jamaicaGC_GetArray32Ref(jamaica_ref b, jamaica_int32 i) {return (jamaica_int32) 0;}
jamaica_int32 jamaicaGC_GetArray32Ref_d0(jamaica_ref b, jamaica_int32 i) {return (jamaica_int32) 0;}
void jamaicaGC_SetArray32Ref(jamaica_ref b, jamaica_int32 i, jamaica_int32 v) {}
void jamaicaGC_SetArray32Ref_d0(jamaica_ref b, jamaica_int32 i, jamaica_int32 v) {}




jamaica_bool jamaicaGC_haltFunction(const char *function, const char *filename, unsigned long lineNb, const char *text) {
	return TRUE;
}


jamaica_ref jamaicaInterpreter_allocJavaObject(jamaica_thread *ct,jamaica_ref cl) {
	//TODO whut? How is a single jamaica_ref enough information?!
	return JAMAICA_NULL;
}

jamaica_ref jamaicaInterpreter_allocSimpleArray(jamaica_thread *ct,jamaica_int32 sz, jamaica_ref cl) {
	return JAMAICA_NULL;
}

jamaica_ref jamaicaInterpreter_allocMultiArray(jamaica_thread *ct,jamaica_int32 *s,jamaica_int32 first_dim,jamaica_int32 dimensions, jamaica_ref cl) {
	return JAMAICA_NULL;
}

jamaica_int32 jamaicaInterpreter_initialize_class_helper(jamaica_thread *ct, jamaica_ref clazz, jamaica_int32  startpc) {
	//TODO What does this do?
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

//These are all marked with JAMAICA_FUNCTION_ATTRIBUTE(noreturn) because exceptions are implemented with setjmp/longjmp
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Winvalid-noreturn"
void jamaica_throwNullPtrExc(jamaica_thread *ct) {printf("jamaica_throwNullPtrExc\n");}
void jamaica_throwIncompClassChangeErr(jamaica_thread *ct) {printf("jamaica_throwIncompClassChangeErr\n");}
void jamaica_throwAbstrMethodErr(jamaica_thread *ct) {printf("jamaica_throwAbstrMethodErr\n");}
void jamaica_throwClassCastExc(jamaica_thread *ct) {printf("jamaica_throwClassCastExc\n");}
void jamaica_throwArrIdxOutOfBndsExc(jamaica_thread *ct) {printf("jamaica_throwArrIdxOutOfBndsExc\n");}
void jamaica_throwArrIdxOutOfBndsExcMsg(jamaica_thread *ct, jamaica_ref msg) {printf("jamaica_throwArrIdxOutOfBndsExcMsg\n");}
void jamaica_throwOutOfMemErr(jamaica_thread *ct) {printf("jamaica_throwOutOfMemErr\n");}
void jamaica_throwNegArrSzExc(jamaica_thread *ct) {printf("jamaica_throwNegArrSzExc\n");}
void jamaica_throwArithmExc(jamaica_thread *ct) {printf("jamaica_throwArithmExc\n");}
void jamaica_throwIllMonitorStateExc(jamaica_thread *ct) {printf("jamaica_throwIllMonitorStateExc\n");}
void jamaica_throwArrStoreExc(jamaica_thread *ct) {printf("jamaica_throwArrStoreExc\n");}
void jamaica_throwNoClassDefFndErr(jamaica_thread *ct) {printf("jamaica_throwNoClassDefFndErr\n");}
void jamaica_throwInterruptedExc(jamaica_thread *ct) {printf("jamaica_throwInterruptedExc\n");}
void jamaica_throwInternalErr(jamaica_thread *ct) {printf("jamaica_throwInternalErr\n");}
void jamaica_throwInternalErrMsg(jamaica_thread *ct, jamaica_ref msg) {printf("jamaica_throwInternalErrMsg\n");}
void jamaica_throwInternalErrcmsg(jamaica_thread *ct, const char *msg) {printf("jamaica_throwInternalErrcmsg\n");}
void jamaica_throwIllArgExccmsg(jamaica_thread *ct, const char *msg) {printf("jamaica_throwIllArgExccmsg\n");}
void jamaica_throwStringIdxOutOfBndsExc(jamaica_thread *ct, jamaica_int32 index, jamaica_int32 lowerBound, jamaica_int32  upperBound) {printf("jamaica_throwStringIdxOutOfBndsExc\n");}
void jamaica_throwIllAssgmtErr(jamaica_thread *ct) {printf("jamaica_throwIllAssgmtErr\n");}
void jamaica_throwStackOvrflwErr(jamaica_thread *ct) {printf("jamaica_throwStackOvrflwErr\n");}
//#pragma GCC diagnostic pop


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
