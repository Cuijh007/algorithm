#include <string.h>
#include "nn_userdefine_slot.h"
#include "nn_net.h"
#include "nn_common.h"
#include "mid_common.h"
#include "nn_vpu_control.h"

extern int16_t gather_offset_index;  // update dynamically with each for-loop.
extern int16_t eos_offset_index;     // eos position of each sentence.
extern LayerId4Skip_t skip_layer_id_info;
#define MAX_SLOT_NUM    64

int nn_userdefine_slot(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase){
    LayerParam_t *layers = pNet->pNetInfo->Layers;
    
    UserDefineSlotSetting_t *slot_setting = (UserDefineSlotSetting_t *)param->userDefine;
    int start_id = slot_setting->startId;
    int end_id = slot_setting->endId;

    int slots_num = layers[end_id].outputShape.C;
    int pred_slot_index[MAX_SLOT_NUM] = {0};

    int32_t *output_addr = (int32_t *)(pBase + param->addr.outputAddr*32);
    uint16_t *p_save_addr = (uint16_t *)(VPU_PSRAM_BASE + param->addr.saveResultAddr * 64 );

    for(int i = 0; i < eos_offset_index; i++)
    {   
        // 0~ actual loop-times
        gather_offset_index = i; 

        NetForward(pNet, model_index, start_id, end_id, NULL, NULL);
        int16_t *end_layer_output_addr = (int16_t*)(pNet->pNpuMemBase+layers[end_id].addr.outputAddr*32);
        int16_t max_index = 0;
        int16_t max_num = -32768;
        
        for(int16_t j = 0; j < slots_num; j++){            
            if(end_layer_output_addr[j] > max_num){
                max_index = j;
                max_num = end_layer_output_addr[j];
            }
        }
        pred_slot_index[i] = max_index;

        int16_t outputHigh = SAT_INT16T(max_num);
        int16_t outputlow = SAT_INT16T(max_index);
		output_addr[i] = ((int32_t)outputHigh<<16)&0xFFFF0000 | (int32_t)outputlow&0x0000FFFF;

        p_save_addr[i*2] = max_index;
        p_save_addr[i*2+1] = max_num;

    }

    int32_t output_size = param->outputShape.W * param->outputShape.H * ALIGN_16(param->outputShape.C);
	iet_shell_memcpy32(output_addr, p_save_addr, sizeof(int16_t) * output_size);

    skip_layer_id_info.start_id = start_id;
    skip_layer_id_info.end_id = end_id;
    return 0;
}

int nn_decoder_word_ids(const LayerParam_t *param, void *pBase){
    
    DecoderWordIdsSetting_t *decoder_word_ids = (DecoderWordIdsSetting_t *)param->userDefine;
    int bos = decoder_word_ids->bos;

    const int16_t *input_addr = (const int16_t*)(pBase+param->addr.inputAddr*32);
    int32_t *output_addr = (int32_t *)(pBase+param->addr.outputAddr*32);

    int max_index_position = 0;

    if(gather_offset_index == 0){
        int16_t bosLow = (int16_t)(bos&0xFFFF);
        output_addr[0] =(int32_t) ((int32_t)bosLow&0x0000FFFF);
    }else{
        max_index_position = (gather_offset_index-1)*2;
        int16_t inputTmp = input_addr[max_index_position];
        output_addr[0] = (int32_t) ((int32_t)inputTmp&0x0000FFFF);
    }

    return 0;
}
