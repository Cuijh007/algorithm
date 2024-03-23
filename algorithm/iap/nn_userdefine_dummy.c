#include "nn_userdefine_dummy.h"
#include "nn_file_struct.h"
#include "nn_net.h"
#include "nn_common.h"
#include "mid_common.h"
#include "mid_npu.h"


// for debug only
#ifdef __x86_64

#endif 



/*
 * connect layer between two models.
 * model_index: model id of the caller
 * 
 */
int nn_userdefine_dummy(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase, int16_t *output) 
{
    UserDefineDummySetting_t *p_setting = (UserDefineDummySetting_t *)&param->userDefine;
    uint32_t model_to_run = p_setting->net_id;
    uint32_t start_id = pNet->pNetInfo->NetLayersStartId[model_to_run];
    uint32_t end_id = pNet->pNetInfo->NetLayersEndId[model_to_run];

#ifndef __x86_64
    // setting weight base for the models to run
    if(pNet->pNetInfo->WeightMemoryType[model_to_run] == MEMORY_TYPE_FLASH)
    {
        uint32_t net_weight_offset = (p_setting->offset_h16<<16)|p_setting->offset_l16;
        uint32_t flash_addr = weight_base_flash + net_weight_offset;

    	*(volatile uint32_t *)0x51b004e0 = flash_addr;
    	*(volatile uint32_t *)0x51b004e4 = flash_addr;
        current_weight_base = flash_addr;
    }
    else
    {
    	*(volatile uint32_t *)0x51b004e0 = pNet->pWeight;
    	*(volatile uint32_t *)0x51b004e4 = pNet->pWeight;
        current_weight_base = pNet->pWeight;
    }
#endif
    // iet_print(IET_PRINT_ALG_DBG, "dummy: %d: %d %d, base: %08x\r\n", model_to_run, start_id, end_id, *(volatile uint32_t *)0x51b004e0);      

    // load_vcode(model_to_run);
    load_vcode_layers(model_to_run, start_id, end_id);


    uint32_t ret = NetForward(pNet, model_to_run, start_id, end_id, NULL, output);
    if(ret == MID_NPU_SKIP)
    {
        return RESULT_TYPE_SKIP_NEXT_LAYERS;
    }
    else
    {
        return RESULT_TYPE_NORMAL;
    }
}


int nn_userdefine_dummy_init(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase)
{
    return 0;
}

