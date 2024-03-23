#ifndef __USERDEFINE_VAD_POST_H__
#define __USERDEFINE_VAD_POST_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include "nn_file_struct.h"


typedef struct VadPrivateData_t
{
    // at most 32bytes private data
	uint32_t g_stride_now;              // frame count for stride[0]
    uint32_t vad_inactive_frame_count;
}VadPrivateData_t;

int nn_userdefine_a105_vad(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase, int16_t *output);

#ifdef __cplusplus
}
#endif
#endif 
