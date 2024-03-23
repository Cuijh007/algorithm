#ifndef __LAYERNORM_H__
#define __LAYERNORM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "nn_file_struct.h"
#include "nn_common.h"
#include "nn_vpu_control.h"


int nn_layernorm(const Net_t *pNet, int model_index, const LayerParam_t *param, void* pBase);


#ifdef __cplusplus
}
#endif
#endif