#ifndef __USERDEFINE_BIDIRECTIONAL_H__
#define __USERDEFINE_BIDIRECTIONAL_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include "nn_file_struct.h"

typedef struct UserDefineBidirectionalSetting_t {
	int16_t startId;         
	int16_t endId;
    int16_t reverse;
	uint16_t runTimes;
}UserDefineBidirectionalSetting_t;

/*
 * layer to control bidirectional GRU 
 * 
 */
int nn_userdefine_bidirectional(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase);

#ifdef __cplusplus
}
#endif
#endif 
