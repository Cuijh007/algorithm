#ifndef __PADDING_H__
#define __PADDING_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "nn_file_struct.h"
#include "nn_pooling.h"

extern int nn_padding(const uint16_t inputAddrA, uint16_t inputAddrB, uint16_t inputAddrC, uint16_t inputAddrD, uint8_t **paddingsize, MatrixShape_t *inputShape);

#ifdef __cplusplus
}
#endif
#endif