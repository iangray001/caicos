#include <stdlib.h>
#include <stdio.h>
#include "jamaica.h"
#include "generatedcode.h"

#ifdef __JAMAICA__

#ifdef __cplusplus
extern "C" {
#endif



JBIEXPORT jamaica_float JBICALL jam_comp_java_lang_Float_58_floatValue17(jamaica_thread *ct,jamaica_ref r0) /* java/lang/Float.floatValue()F */  {
 jamaica_int32 tp;
 /* JAMAICA_C_STACK_OVERFLOW not needed for leaf methods */
 tp=ct->m.cli+0;
 if(tp > ct->javaStackSize) { goto LABEL_tSOE; } 
 return JAMAICA_BLOCK_GET_DATA32(r0,3).f;
LABEL_tSOE:    jamaica_throwStackOvrflwErr(ct);     return 0;
}


JBIEXPORT jamaica_double JBICALL jam_comp_java_lang_Double_51_doublValue17(jamaica_thread *ct,jamaica_ref r0) /* java/lang/Double.doubleValue()D */  {
 jamaica_int32 tp;
 /* JAMAICA_C_STACK_OVERFLOW not needed for leaf methods */
 tp=ct->m.cli+0;
 if(tp > ct->javaStackSize) { goto LABEL_tSOE; } 
 return JAMAICA_BLOCK_GET_DATA64(r0,4).d;
LABEL_tSOE:    jamaica_throwStackOvrflwErr(ct);     return 0;
}


JBIEXPORT jamaica_double JBICALL jam_comp_unitTests_Types_0_types1(jamaica_thread *ct,jamaica_int32 n0,jamaica_float f0,jamaica_double d0,jamaica_ref r0,jamaica_ref r1) /* unitTests/Types.types(IFDLjava/lang/Float;Ljava/lang/Double;)D */  {
 jamaica_int32 tp;
 jamaica_GCEnv *gc = ct->m.gcenv;
 jamaica_ref *l=&(ct->m.cl[ct->m.cli]);
 jamaica_float f1=0;
 jamaica_double d1=0;
 JAMAICA_C_STACK_OVERFLOW(ct, goto LABEL_tSOE;);
 tp=ct->m.cli+1;
 if(tp > ct->javaStackSize) { goto LABEL_tSOE; } 
 ct->m.cli = tp;
 
#ifdef JAMAICA_PARALLEL
{jamaica_ref old_ref=l[0];if(old_ref!=JAMAICA_NULL){jamaica_cpu *cpu=ct->cpu;jamaica_ref white=cpu->gc.white;if(JAMAICA_BLOCK_GET_R(old_ref,0)==white){jamaica_ref gL, result; gL=cpu->gc.greyList;JAMAICA_NATIVE_THREAD_COMPARE_AND_SWAP_REF(&(JAMAICA_BLOCK_GET_R(old_ref,0)),white,gL,result);if(result==white){cpu->gc.greyList=old_ref;}}}l[0]=r1;}
#else
{jamaica_ref new_ref=r1;if(new_ref!=JAMAICA_NULL)if(JAMAICA_BLOCK_GET_R(new_ref,0)==gc->white){ JAMAICA_BLOCK_GET_R(new_ref,0)=gc->greyList;gc->greyList=new_ref;}l[0]=new_ref;}
#endif

 f1=jam_comp_java_lang_Float_58_floatValue17(ct,r0);
 
#ifdef JAMAICA_PARALLEL
{jamaica_ref old_ref=l[0];if(old_ref!=JAMAICA_NULL){jamaica_cpu *cpu=ct->cpu;jamaica_ref white=cpu->gc.white;if(JAMAICA_BLOCK_GET_R(old_ref,0)==white){jamaica_ref gL, result; gL=cpu->gc.greyList;JAMAICA_NATIVE_THREAD_COMPARE_AND_SWAP_REF(&(JAMAICA_BLOCK_GET_R(old_ref,0)),white,gL,result);if(result==white){cpu->gc.greyList=old_ref;}}}l[0]=JAMAICA_NULL;}
#else
{l[0]=JAMAICA_NULL;}
#endif

 d1=jam_comp_java_lang_Double_51_doublValue17(ct,r1);
 ct->m.cli-=1;
 return ((((((jamaica_double)n0)+((jamaica_double)f0))+((jamaica_double)f1))+d1)+d0);
LABEL_tSOE:    jamaica_throwStackOvrflwErr(ct);     return 0;
}



#else
#error 'jamaica.h' not found!
#endif

#ifdef __cplusplus
}
#endif

