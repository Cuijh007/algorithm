#ifndef __USERDEFINE_CTC_POST_H__
#define __USERDEFINE_CTC_POST_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include "nn_file_struct.h"
//#include "nn_common.h"
//#include "mid_common.h"

#define MAX_WAKEUP_WORD_NUM         (30+1)    // the first one for silent

typedef struct UserDefineA105PostPrivateData {
	int16_t vad_threshold;         
}UserDefineA105PostPrivateData;

int vad_intent_check(int vad_value, int16_t *output);
int nn_userdefine_a105_post(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase, int16_t *output) ;
int nn_userdefine_a105_post_init(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase);

#ifdef __cplusplus
}
#endif
#endif 
