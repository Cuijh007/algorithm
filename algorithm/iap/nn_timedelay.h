#ifndef __TIMEDELAY_H__
#define __TIMEDELAY_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include "nn_file_struct.h"
#include "nn_vpu_control.h"

#define TIMED_CCFG0_COPY 	0xA0208744
#define TIMED_CCFG1_COPY 	0xA0208748

#define TIMED_ELP_H_COPY0	0xA0208750
#define TIMED_ELP_L_COPY0	0xA0208754
#define TIMED_EADI_H_COPY0	0xA020875C
#define TIMED_EADI_L_COPY0	0xA0208760
#define TIMED_EAA_H_COPY0	0xA020876C
#define TIMED_EAA_L_COPY0	0xA0208770
#define TIMED_EDA_H_COPY0	0xA0208774
#define TIMED_EDA_L_COPY0	0xA0208778
#define TIMED_SCR_COPY0		0xA0208784

#define TIMED_ELP_H_COPY1	0xA0208788
#define TIMED_ELP_L_COPY1	0xA020878C
#define TIMED_EADI_H_COPY1	0xA0208794
#define TIMED_EADI_L_COPY1	0xA0208798
#define TIMED_EAA_H_COPY1	0xA02087A4
#define TIMED_EAA_L_COPY1	0xA02087A8
#define TIMED_EDA_H_COPY1	0xA02087AC
#define TIMED_EDA_L_COPY1	0xA02087B0
#define TIMED_SCR_COPY1		0xA02087BC

#define TIMED_ELP_H_COPY2	0xA02087C0
#define TIMED_ELP_L_COPY2	0xA02087C4
#define TIMED_EADI_H_COPY2	0xA02087CC
#define TIMED_EADI_L_COPY2	0xA02087D0
#define TIMED_EAA_H_COPY2	0xA02087DC
#define TIMED_EAA_L_COPY2	0xA02087E0
#define TIMED_EDA_H_COPY2	0xA02087E4
#define TIMED_EDA_L_COPY2	0xA02087E8
#define TIMED_SCR_COPY2		0xA02087F4

typedef struct TimeDelayPrivateData
{
    // at most 32bytes private data
	uint32_t g_stride_now;
}TimeDelayPrivateData_t;


int nn_timedelay_init(Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase);
int nn_timedelay(const LayerParam_t *param, void *pBase);
int nn_timedelay_vpu(const LayerParam_t *param);

#ifdef __cplusplus
}
#endif
#endif 