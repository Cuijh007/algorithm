#include <string.h>
#include "nn_userdefine_bidirectional.h"
#include "nn_net.h"
#include "nn_common.h"
#include "mid_common.h"

extern int16_t gather_offset_index;  // update dynamically with each for-loop.
extern int16_t eos_offset_index;     // eos position of each sentence.
// extern int8_t sub_layer_ids_flag[];
extern LayerId4Skip_t skip_layer_id_info;

int nn_userdefine_bidirectional(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase){
    LayerParam_t *layers = pNet->pNetInfo->Layers;
    
    UserDefineBidirectionalSetting_t *bidirectional = (UserDefineBidirectionalSetting_t *)param->userDefine;
    int start_id = bidirectional->startId;
    int end_id = bidirectional->endId;

    for(int i = 0; i < bidirectional->runTimes; i++)
    {   
        if(bidirectional->reverse == 0){
            //forward: take data 0~15
            gather_offset_index = i; 
        }else{
            //backward: take data 15~0 
            gather_offset_index = bidirectional->runTimes - i - 1;
        }        

        NetForward(pNet, model_index, start_id, end_id, NULL, NULL);
    }
    
    if(skip_layer_id_info.end_id != start_id - 1)
        skip_layer_id_info.start_id = start_id;
    skip_layer_id_info.end_id = end_id;
    return 0;
}
