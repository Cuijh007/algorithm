#ifndef __COPY_H__
#define __COPY_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "nn_file_struct.h"
#include "nn_common.h"
#include "nn_vpu_control.h"

int nn_copy(const LayerParam_t *param);

#ifdef __cplusplus
}
#endif
#endif