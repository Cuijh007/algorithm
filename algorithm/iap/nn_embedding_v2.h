#ifndef __EMBEDDING_H__
#define __EMBEDDING_H__

#include "nn_file_struct.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern int nn_embedding_v2(const LayerParam_t *param, void* pBase, int16_t *p_in);
#ifdef __cplusplus
}
#endif

#endif