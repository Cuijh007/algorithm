#ifndef _ISR_DEBUG_H_
#define _ISR_DEBUG_H_

#ifdef __x86_64
	#include <math.h>
	#include <string.h>
	#define HW_FFT 0
	#define HW_LOG 1
	#define ISR_DEBUG_LOG
	#define TEST_WAV
#else
	#define HW_FFT 1
	#define HW_LOG 1
	//#define DEBUG_FFT_DATA
	//#define DEBUG_VPU_DATA
	//#define DEBUG_VPU_WEIGHT_DATA

	//#define RUN_IN_M3
	#define RUN_IN_FPGA
	//#define TEST_WAV
	//#define TEST_DECODER
#endif 

#endif
