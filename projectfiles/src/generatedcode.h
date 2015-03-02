/*
 * generatedcode.h
 *
 *  Created on: 14 Aug 2014
 *      Author: ian
 */

#ifndef GENERATEDCODE_H_
#define GENERATEDCODE_H_

#include <jamaica.h>

JBIEXPORT jamaica_int32 JBICALL jam_comp_dataProcessing_ProcessMe_0_sumArray1(jamaica_thread *ct,jamaica_ref r1);
JBIEXPORT void JBICALL jam_comp_simpleTests_MD5_1_md54(jamaica_thread *ct,jamaica_ref r1,jamaica_int32 n17,jamaica_ref r5,jamaica_ref r0,jamaica_ref r2,jamaica_ref r3);


JBIEXPORT jamaica_int32 JBICALL jam_comp_simpleTests_Collatz_0_collauator1(jamaica_thread *ct,jamaica_int32 n0);

typedef unsigned int uint32;
typedef unsigned char uint8;

void md5(uint8 msg[1024], int initial_len, uint8 digest[16]);
int collatz_evaluator(int num);

int fir(int inputSample, int count, int length, int *delayLine, int *impulseResponse);
JBIEXPORT jamaica_int32 JBICALL jam_comp_simpleTests_FIR_1_getOuample1(jamaica_thread *ct,jamaica_int32 n8,jamaica_int32 n9,jamaica_int32 n0,jamaica_ref r1,jamaica_ref r0);


JBIEXPORT jamaica_int32 JBICALL jam_comp_simpleTests_ArraySum_0_sumArayInt1(jamaica_thread *ct,jamaica_ref r1);
int sum_array(int count, int *array);


JBIEXPORT jamaica_ref JBICALL jam_comp_simpleTests_FFT_2_fft1(jamaica_thread *ct,jamaica_ref r3,jamaica_ref r4,jamaica_ref r1,jamaica_ref r2,jamaica_int32 n40,jamaica_ref r0);



#endif /* GENERATEDCODE_H_ */
