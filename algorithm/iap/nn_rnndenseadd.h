#ifndef __RNNDENSEADD_LAYER_H__
#define __RNNDENSEADD_LAYER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "nn_file_struct.h"
#include "nn_dense.h"
#include "nn_conv.h"

typedef struct InitStateSetting_t {
	int16_t init_layer_id;         
}InitStateSetting_t;

int nn_rnndenseadd(const LayerParam_t *param, void* pBase, void *players);
int nn_rnnstate(const LayerParam_t *param, void *players, void* pBase);
int nn_gruspecialtrement(const LayerParam_t *param, void* players, void* pBase);
int nn_GruResetStatus(Net_t *pNet, int model_index, void *pBase);

#ifdef __cplusplus
}
#endif

#endif