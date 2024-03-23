#include "nn_userdefine_a105_vad.h"
#include "nn_userdefine_a105_post.h"
#include "nn_file_struct.h"
#include "nn_net.h"
#include "nn_common.h"
#include "mid_common.h"
#include "mid_npu.h"


// for debug only
#ifdef __x86_64

#endif 


/*
 * vad checking, get the voice inactive frame number
 */
int nn_userdefine_a105_vad(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase, int16_t *output) 
{
    LayerParam_t *pLayers = &pNet->pNetInfo->Layers[0];
    LayerParam_t *input = &pLayers[param->inputLayerId[1]];    
    int32_t *inputAddr = (int32_t *)((char*)pBase + param->addr.inputAddr*32);
    int out_size = input->outputShape.H * input->outputShape.W * input->outputShape.C;
    if(input->saveFlag==1)
    {
        if(pNet->pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
        {
            inputAddr = (int32_t *)((char*)VPU_PSRAM_BASE + input->addr.saveResultAddr*64);
        }
        else
        {
            inputAddr = (int32_t *)((char*)pBase + input->addr.saveResultAddr*32);
        }    
    }       
    int32_t vad_c = param->inputShape.C;

    int32_t valid_id = 0;
    int16_t max_score = -32768;
    int32_t *p_data = inputAddr;
    for(int k=0; k<vad_c/2; k++)
    {
        int32_t value = *p_data;
        int16_t value_0 = value&0xFFFF;
        int16_t value_1 = (value>>16)&0xFFFF;
        if(value_0 > max_score)
        {
            max_score = value_0;
            valid_id = 2*k;
        }
        if(value_1 > max_score)
        {
            max_score = value_1;
            valid_id = 2*k+1;
        }
        p_data++;
    }

    VadPrivateData_t *p_private_data = (VadPrivateData_t *)param->userDefine;    

    if(valid_id == 0)
        p_private_data->vad_inactive_frame_count++;
    else
        p_private_data->vad_inactive_frame_count = 0;

    p_private_data->g_stride_now++;
    if(p_private_data->g_stride_now == param->strides[0])
    {
        p_private_data->g_stride_now = 0;
        return RESULT_TYPE_NORMAL;
    }
    else
    {
        /* vad_intent_check avoid work with post*/
        int ret = vad_intent_check(p_private_data->vad_inactive_frame_count, output);
        if(ret == RESULT_TYPE_VALID_CMD)
        {
            // cmd end is detected, intent process has been done, the result is in 'output'
            // skip the 
            return RESULT_TYPE_VALID_CMD;
        }

        return RESULT_TYPE_SKIP_NEXT_LAYERS;
    }
}


int nn_userdefine_a105_vad_init(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase)
{
    // reset local varibles
    VadPrivateData_t *p_private_data = (VadPrivateData_t *)param->userDefine;
    p_private_data->g_stride_now = 0;
    p_private_data->vad_inactive_frame_count = 0; 
    return 0;
}

