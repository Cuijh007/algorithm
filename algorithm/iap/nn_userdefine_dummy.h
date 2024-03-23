#ifndef __USERDEFINE_DUMMY_H__
#define __USERDEFINE_DUMMY_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include "nn_file_struct.h"

typedef struct UserDefineDummySetting_t
{
	int16_t net_id;         // id of net to start by this layer
	uint16_t offset_l16;	// weight data add offset - low 16bit, refer to the base of the net weight in bin file
	uint16_t offset_h16;	// weight data add offset - high 16bits, refer to the base of the net weight in bin file
	int16_t reserved[7];
}UserDefineDummySetting_t;

/*
 * layer to connect two sub-nets
 * to run the sub-net speicifed by the parameters
 * 
 */
int nn_userdefine_dummy(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase, int16_t *output);

#ifdef __cplusplus
}
#endif
#endif 
