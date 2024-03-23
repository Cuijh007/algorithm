#ifndef __EMBEDDING_H__
#define __EMBEDDING_H__

#include "nn_file_struct.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern int nn_top_k(const LayerParam_t *param, void* pBase);
#ifdef __cplusplus
}
#endif

#endif