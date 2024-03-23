#ifndef __USERDEFINE_SLOT_H__
#define __USERDEFINE_SLOT_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include "nn_file_struct.h"

typedef struct UserDefineSlotSetting_t {
	int16_t startId;         
	int16_t endId;
    int16_t reverse;
	uint16_t runTimes;
}UserDefineSlotSetting_t;

typedef struct DecoderWordIdsSetting_t {
	int16_t bos;         
}DecoderWordIdsSetting_t;

/*
 * layer to control slot decoder net
 * 
 */
int nn_userdefine_slot(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase);
int nn_decoder_word_ids(const LayerParam_t *param, void *pBase);

#ifdef __cplusplus
}
#endif
#endif 
