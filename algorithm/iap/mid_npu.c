/******************************************************************************
*  MID NPU Control Module Source File for Chopin.
*
*  Copyright (C) 2019-2020 IET Limited. All rights reserved.
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License version 3 as
*  published by the Free Software Foundation.
*
*  You should have received a copy of the GNU General Public License
*  along with OST. If not, see <http://www.gnu.org/licenses/>.
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*  @file     mid_npu.c
*  @brief    NPU control module Source File
*  @author   Zhiyun.Ling
*  @email    Zhiyun.Ling@intenginetech.com
*  @version  1.0
*  @date     2019-12-17
*  @license  GNU General Public License (GPL)
*
*------------------------------------------------------------------------------
*  Remark         : Description
*------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*------------------------------------------------------------------------------
*  2019/12/17 | 1.0       | Zhiyun.Ling    | Create file
*  2020/02/28 | 1.1       | Hui.Zhang      | Fill Function
*
******************************************************************************/

/******************************* Include *************************************/
#include "mid_npu.h"
#include "nn_file_struct.h"
#include "nn_common.h"
#include "nn_net.h"
#include "nn_version.h"
#include "nn_timedelay.h"
#include "nn_userdefine_dummy.h"
#include "nn_vpu_control.h"
#include "iet_qspi_flash.h"


#define MAX_NET_INPUT_SIZE  (1*5*64)      // change this value if max input size changed 
// #define DEBUG_NPU
// #define DEBUG_MODEL_RUN_ON_FLASH

// for debug layers
extern int debug_layer_id;      
extern int debug_frame_index;
extern int debug_inout_setting;
extern int frame_index_to_print;
extern int debug_layer_enable;

/******************************* variable   **********************************/
extern Net_t g_net;
extern NetInfo_t *g_p_net_info;
extern int32_t kept_word_flag;

// extern int16_t *p_intent_threshold;

// base addr of the weight data for the whole net on flash
uint32_t weight_base_flash = 0;

#ifdef NLP_2CHIP
// base addr of the kws nets, it runs on flash
uint32_t kws_weight_flash_addr = 0;
#endif

// the weight base of the current model
uint32_t current_weight_base = 0;

// temporary variables to debug models run on flash
#ifdef DEBUG_MODEL_RUN_ON_FLASH
uint32_t net_run_on_flash_addr[MAX_MODEL_SUPPORTED] = {0};
#endif

// for NLP2.0, kws weight offset in the weight part is store in pNet.reserved.
// make sure this match with isd2bin
typedef struct 
{
    uint16_t offset_l16; // low 16bits of offset
    uint16_t offset_h16; // high 16bits of offset
} NetOffset;

/******************************* Function   **********************************/
static uint32_t parse_net(Net_t *pNet, MID_NPU_MODEL_INFO *p_mode_info, int32_t *model_number)
{
    NetInfo_t *pNetInfo = pNet->pNetInfo;
    int32_t net_num  = pNetInfo->NetNum;
    
    if(pNetInfo->NetNum<=0)
    {
        pNetInfo->NetNum = 1;
        pNetInfo->NetLayersStartId[0] = 0;
        pNetInfo->NetLayersEndId[0] = pNetInfo->LayersNum - 1;
    }

    //for NLP 1CHIP, over than 2 nets exists, but only the first 2 nets are needed by app
    // Doesn't support NLP and SV working in one bin
    for(int i=0; i<MAX_MODEL_SUPPORTED; i++)
    {
        if(pNet->pNetInfo->TrainType[i] == NLP_2CHIP_MODEL
            || pNet->pNetInfo->TrainType[i] == NLP_MODEL)
        {
            // NLP take 4 subnets, but only 1 is needed by APP
            net_num -= 3;
            break;
        }
        if(pNet->pNetInfo->TrainType[i] == SV_MODEL)
        {
            // SV takes 2 subnets, but only 1 is needed by APP
            net_num -= 1;
            break;            
        }
    }

    *model_number = net_num;
    for(int i=0; i<net_num; i++)
    {
        int index;
        LayerParam_t *p_layer;
        
        // Get net frame info
        p_mode_info->model_index = i;
        if(pNetInfo->FrameLength[i] <= 0)
        {
            pNetInfo->FrameLength[i] = 32;
        }
        p_mode_info->frame_length = pNetInfo->FrameLength[i];
        if(pNetInfo->FrameOffset[i] <= 0)
        {
            pNetInfo->FrameOffset[i] = 32;
        }
        p_mode_info->frame_offset = pNetInfo->FrameOffset[i];

        // Get net input/output size
        index = pNetInfo->NetLayersStartId[i];
        p_layer = &pNetInfo->Layers[index];
        p_mode_info->input_size = p_layer->inputShape.H *
                                  p_layer->inputShape.W *
                                  p_layer->inputShape.C;
        pNet->inputSize[i] = p_mode_info->input_size;

        index = pNetInfo->NetLayersEndId[i];
        if(index == 0)
        {
            iet_print(IET_PRINT_ERROR, "NetLayersEndId cannot be zero\n");
            return E_ERR;
        }

        // set net output size
        p_layer = &pNetInfo->Layers[index];
        pNet->netOutputSize[i] = p_layer->outputShape.H *
                                   p_layer->outputShape.W *
                                   ALIGN_16(p_layer->outputShape.C);

        pNet->amOutputSize[i] = 0;
#if (!defined NLP_1CHIP) && (!defined NLP_2CHIP)
        // set AM output size of normal KWS or last layer of 1st stage of wakeup net for debug
        // both use softmax
        // NOTE: 
        // take first softmax out in the net as AM now, or else new way should be considered.
        // if multi softmax used in the net, think about here and data copy in NetForward again.
        for(int k=pNetInfo->NetLayersStartId[i]; k<pNetInfo->NetLayersEndId[i]; k++)
        {
            p_layer = &pNetInfo->Layers[k];
            if(p_layer->type == Copy && p_layer->activation == SOFTMAX)
            {
                // 'H' & 'w' may be not 1 for AM part of K002, multi frames are processed together
                // its result is not used for debug since to enough memory to store
                // set here just for code compilant weith other models.
                pNet->amOutputSize[i] = ALIGN_16(p_layer->outputShape.C);
                break;
            }
        }
#endif
#ifdef NLP_2CHIP
        if(i == NET_ID_A105)
        {
            // valid for A105 on U2 of two chip case only
            //TODO: for A105 2nd, should do this in ISD2BIN, use intent out as final result
            // iet_print(IET_PRINT_ALG_DBG, "NET[%d]: %d %d\r\n", i,pNetInfo->NetLayersStartId[i],pNetInfo->NetLayersEndId[i]);
            for(int k=pNetInfo->NetLayersStartId[i]; k<=pNetInfo->NetLayersEndId[i]; k++)
            {
                p_layer = &pNetInfo->Layers[k];
                if(p_layer->type == UserDefineA105Post)
                {
                    pNet->netOutputSize[i] = NLP_OUT_SIZE;
                    break;
                }
            }
        }
#endif  
        // for NLP with 1chip
#ifdef NLP_1CHIP
        if(i == NET_ID_A105)
        {
            // Two nets for NLP, kws + NLP
            // NLP includes 4 sub-nets, 3 splitted from A105 to run from different memory(psram, flash) + intent
            // 4 sub-nets of NLP run as one nets.
            // the netOutputSize gotten by above is the out size of A105-1 instead of the final out, so mut set here
            pNet->netOutputSize[i] = NLP_OUT_SIZE;
        }
#endif        

        p_mode_info->result_size = pNet->netOutputSize[i] + pNet->amOutputSize[i];
        // iet_print(IET_PRINT_LOG, "Net[%d]:  out result_size : %d, %d\r\n", i, pNet->netOutputSize[i], pNet->amOutputSize[i]);
        // check mic info
        if(pNetInfo->MicNumber[i] <= 0)
        {
            pNetInfo->MicNumber[i] = 2;
        }
        p_mode_info->mic_number = pNetInfo->MicNumber[i];      
        p_mode_info++;

    } 

#if (defined __x86_64)&&(defined DEBUG_NET_LAYER)
    DebugNetStruct(pNetInfo);
#endif

    return E_OK;
}

/*
 * Called by application with parsed weight info
 */
uint32_t mid_npu_init(MID_NPU_INIT_PARAMS *p_init, MID_NPU_MODEL_INFO *model_info, int32_t *model_number)
{
    Net_t *pNet = &g_net;
    memset(pNet, 0, sizeof(Net_t));
    pNet->pNetInfo = (NetInfo_t*)p_init->p_net_info;
    g_p_net_info = (NetInfo_t*)p_init->p_net_info;
    pNet->pWeight = p_init->p_weight;
    pNet->pMicWeightMatrix = p_init->p_mic_weight_matrix;
    pNet->pVpuCode = p_init->p_vpu_code;
    pNet->pPrivateData = p_init->p_private_data;
    pNet->pNpuMemBase = (void *)VPU_VMEM_BASE;

#ifdef NLP_2CHIP
    //TODO: app call setting weight base before this one, so set the kws weight addr here
    NetOffset *p_kws_offset =(NetOffset *)(&(pNet->pNetInfo->Reserved[0]));
    uint32_t kws_weight_offset = (p_kws_offset->offset_h16<<16)|p_kws_offset->offset_l16;
    kws_weight_flash_addr = weight_base_flash + kws_weight_offset;
    iet_print(IET_PRINT_APP_KEY,"res_init, %p, [L%d, net: %d:(%d - %d)(%d - %d) ]： %04x %04x %04x %04x\r\n",
        &(pNet->pNetInfo->Reserved[0]),
        g_p_net_info->LayersNum, g_p_net_info->NetNum,
        g_p_net_info->NetLayersStartId[0], g_p_net_info->NetLayersEndId[0],
        g_p_net_info->NetLayersStartId[1], g_p_net_info->NetLayersEndId[1],
        g_p_net_info->Reserved[0], g_p_net_info->Reserved[1], g_p_net_info->Reserved[2], g_p_net_info->Reserved[3]);
#endif

#ifdef __x86_64
    uint16_t g_board_ver = g_p_net_info->BoardVersion;
    printf("g_board_ver : %d\n", g_p_net_info->BoardVersion);
    if(g_board_ver == BACH1C){
        pNet->pNpuMemBase = malloc(VPU_MEM_SIZE);
        memcpy((char*)pNet->pNpuMemBase, p_init->p_weight, VPU_MEM_SIZE);
    }else if(g_board_ver == BACH2C){
        pNet->pNpuMemBase = malloc(VPU_MEM_SIZE*2);
        memset((char*)pNet->pNpuMemBase, 0, VPU_MEM_SIZE);  
        memcpy((char*)pNet->pNpuMemBase+VPU_MEM_SIZE, p_init->p_weight, VPU_MEM_SIZE);
    }
#else
    if(g_p_net_info->BoardVersion == BACH2C)
    {
        // TODO: /*REG_VPU_AXI_1KB_WRAP*/ is this necessary? to do by app ? 
        // SET_REG(0x51b00210, (1<<28));
        *(volatile uint32_t *)0x51b00210 = (1<<28);
        // iet_print(IET_PRINT_ALG_DBG, "0x51b00210 0x%X\r\n", GET_REG(0x51b00210));
        *(volatile uint32_t *)0x51b004d4 = 0x7;
        
    	*(volatile uint32_t *)0x51b004e0 = p_init->p_weight;
    	*(volatile uint32_t *)0x51b004e4 = p_init->p_weight;      
    }
    else
    {
        /*load vpu code to VPU_CODE_ADDR*/
        uint16_t *p_vpu_code = (uint16_t *)pNet->pVpuCode;
        int vpucode_size = pNet->pNetInfo->VpuCodeSize[0];        
        for(int i = 0; i < vpucode_size; i++)
        {
            *((int *)VPU_CODE_ADDR + i) = ((int)p_vpu_code[i] & 0x00000FFFF);            
        }
    }
#endif

    uint32_t ret = parse_net(pNet, model_info, model_number);

#ifdef DEBUG_MODEL_RUN_ON_FLASH
    // valid for 1chip only
    // for debug of models called by dummy
    {
        for(int i=0; i<pNet->pNetInfo->NetNum; i++)
        {
            int start_id = pNet->pNetInfo->NetLayersStartId[i];
            int end_id = pNet->pNetInfo->NetLayersEndId[i];
            for(int idx=start_id; idx <= end_id; idx++)
            {
                LayerParam_t *pLayers = pNet->pNetInfo->Layers;
                LayerParam_t *curLayer = &pLayers[idx];
                if(curLayer->type == UserDefineDummy)
                {
                    UserDefineDummySetting_t *p_setting = (UserDefineDummySetting_t *)&curLayer->userDefine;
                    uint32_t net_weight_offset = (p_setting->offset_h16<<16)|p_setting->offset_l16;
                    uint32_t flash_addr = weight_base_flash + net_weight_offset;
                    uint32_t model_to_run = p_setting->net_id;
                    net_run_on_flash_addr[model_to_run] = flash_addr;
                    break;
                }
            }
            iet_print(IET_PRINT_LOG,"net_run_on_flash_addr[%d]: %08x\r\n", i, net_run_on_flash_addr[i]);
        }

    }
#endif

    if(ret == E_OK)
        return MID_NPU_OK;
    else
        return E_ERR;

}

uint32_t mid_npu_open(MID_NPU_OPEN_CONFIG *npu_config)
{
    uint32_t ret = MID_NPU_OK;
    if(npu_config == NULL)
    {
        iet_print(IET_PRINT_ALG_DBG,"Invalid para\r\n");
        return MID_NPU_ERR;
    }

    Net_t *pNet = &g_net;
    NetInfo_t *pNetInfo = g_p_net_info;

    // Check weight data
    switch(npu_config->verify_mode)
    {
        case MID_NPU_VERIFY_NONE:
            break;
        case MID_NPU_VERIFY_CRC:
            ret = MID_NPU_ERR;
            iet_print(IET_PRINT_ALG_DBG, "UNS:VERIFY_CRC\r\n");
            break;
        default:
            break;

    }

    // set npu running mode
    switch(npu_config->run_mode)
    {
        case MID_NPU_RUN_FRAME:
            break;
        case MID_NPU_RUN_LAYER:
            ret = MID_NPU_ERR;
            iet_print(IET_PRINT_ALG_DBG, "UNS:RUN_LAYER\r\n");
            break;
        default:
            break;
    }

    // Set callback function of npu runnging if needed
    pNet->npu_callback = (void*)npu_config->npu_callback;
#ifdef __x86_64
    pNet->npu_layer_callback = (void*)npu_config->npu_layer_callback;
#endif

    for(int i=0; i<pNetInfo->NetNum; i++)
    {
        // initialize private data of the layers
        NetInit(pNet, i);
    }

    return ret;
}

uint32_t mid_npu_close(void)
{
    Net_t *pNet = &g_net;
    memset(pNet, 0, sizeof(Net_t));
    return MID_NPU_OK;
}

uint32_t mid_npu_start(int32_t model_index, int16_t *input_buffer, int16_t *out_result)
{
    uint32_t start = iet_systick_read();
    int32_t ret = MID_NPU_ERR;
    Net_t *pNet = &g_net;
    NetInfo_t *pNetInfo = pNet->pNetInfo;
    int32_t startId = pNetInfo->NetLayersStartId[model_index];
    int32_t endId = pNetInfo->NetLayersEndId[model_index];
    
    if(startId == pNet->pNetInfo->NetLayersStartId[model_index])
    {
        // the value is correct only when run a model
        // if(model_index == NET_ID_A105)
        if(model_index == 0)
        {
            frame_index_to_print++;
        }
    }
#ifdef DEBUG_NET_LAYER
    if(model_index == stop_model_index && stop_frame_index != -1)
    {
        if(frame_index_to_print == stop_frame_index)
        {
            iet_print(IET_PRINT_LOG, "Frame %d, stop at layer %d\r\n", frame_index_to_print, stop_layer_id);
            set_npu_stop_layer(model_index, stop_layer_id, stop_vcode_addr);
            endId = stop_layer_id;
        }
    }
#endif

#ifdef NLP_2CHIP
    //if(pNet->pNetInfo->WeightMemoryType[model_index] == MEMORY_TYPE_FLASH)
    if(model_index == NET_ID_WAKEUP)
    {
        // uint32_t kws_weight_offset = *((int*)&pNetInfo->Reserved[0]);
        // uint32_t kws_weight_flash_addr = (uint32_t)(nlp_u1_flashbase + nlp_weight_offset + kws_weight_offset);
        *(volatile uint32_t *)0x51b004e0 = (uint32_t)kws_weight_flash_addr;
        *(volatile uint32_t *)0x51b004e4 = (uint32_t)kws_weight_flash_addr;
        current_weight_base = kws_weight_flash_addr;
    }
    else
    {
        *(volatile uint32_t *)0x51b004e0 = (uint32_t)pNet->pWeight;
        *(volatile uint32_t *)0x51b004e4 = (uint32_t)pNet->pWeight;
        current_weight_base = pNet->pWeight;
    }
#else
    // set NPU operation base address, valid for kws and A105 1st of NLP for 1 chip
    *(volatile uint32_t *)0x51b004e0 = (uint32_t)pNet->pWeight;
    *(volatile uint32_t *)0x51b004e4 = (uint32_t)pNet->pWeight;
    current_weight_base = pNet->pWeight;
#endif
    /*load vpu code to VPU_CODE_ADDR*/
    load_vcode_layers(model_index, startId, endId);
    // load_vcode(model_index);

#ifdef DEBUG_MODEL_RUN_ON_FLASH
    // for debug models run from flash, valid for 1chip only
    if(pNet->pNetInfo->WeightMemoryType[model_index] == MEMORY_TYPE_FLASH)
    {
    	*(volatile uint32_t *)0x51b004e0 = net_run_on_flash_addr[model_index];
    	*(volatile uint32_t *)0x51b004e4 = net_run_on_flash_addr[model_index];
    }
    else
    {
    	*(volatile uint32_t *)0x51b004e0 = pNet->pWeight;
    	*(volatile uint32_t *)0x51b004e4 = pNet->pWeight;
    }    
    iet_print(IET_PRINT_APP_KEY, "Net%d pWeight %08x %08x, reg: %08x\r\n", 
        model_index,  pNet->pWeight, net_run_on_flash_addr[model_index], *(volatile uint32_t *)0x51b004e0);
#endif    

#if (defined NLP_1CHIP)
    // for intent test
    // remove this after debug done, cannot used to 2chip, NET_ID_WAKEUP == NET_ID_INTENT
    if(model_index != NET_ID_INTENT)
#endif    
    {
        int32_t s_time = iet_systick_read();
        ret = NetForward(pNet, model_index, startId, endId, input_buffer, out_result);
        int32_t e_time = iet_systick_read();
        iet_print(IET_PRINT_APP_KEY, "T= %d\r\n", e_time - s_time);
    }

    // for intent test
#if (defined NLP_1CHIP)
    if(model_index == NET_ID_INTENT)
    {
        extern int16_t intent_input[];
        out_result[128] = 0;  
        for(int i=0; i<16; i++)
        {   
            intent_input[i] = input_buffer[i];
            if(input_buffer[i] == 2)
            {
                extern int16_t eos_offset_index;     // eos position of each sentence.
                eos_offset_index = i;
                break;
            }
        }
        ret = intent_start(NET_ID_INTENT, input_buffer, out_result);   
        model_index = 1;     
    }
#endif    
    // uint32_t end = iet_systick_read();
    // iet_print(IET_PRINT_LOG, "%s F%d time = %d\r\n", __FUNCTION__, frame_index_to_print,  end - start);
    // iet_printf("%s time = %d\r\n", __FUNCTION__,  end - start);
#ifndef __x86_64
    if(pNet->npu_callback != NULL && ret == MID_NPU_OK)
    {
        ((mid_npu_callback)pNet->npu_callback)(model_index);
    }
    if(ret == MID_NPU_SKIP){
        ret = MID_NPU_OK;
    }
#endif
    return ret;

}


uint32_t mid_npu_stop(const int32_t model_index)
{
    Net_t *pNet = &g_net;
    NetInfo_t *pNetInfo = pNet->pNetInfo;
    int16_t last_layer_id = pNetInfo->NetLayersEndId[model_index];
    int16_t *outputAddr = (int16_t*)(pNetInfo->Layers[last_layer_id].addr.outputAddr*32 +
                          pNet->pNpuMemBase);
    memset(outputAddr, 0, sizeof(int16_t)*pNet->netOutputSize[model_index]);
    return MID_NPU_OK;
}

inline void timedelay_clear(int32_t model_index)
{
    Net_t *pNet = &g_net;
    NetInfo_t *pNetInfo = pNet->pNetInfo;
    int32_t startId = pNetInfo->NetLayersStartId[model_index];
    int32_t endId = pNetInfo->NetLayersEndId[model_index];
    for(int i=startId; i<=endId; i++)
    {
        if(pNetInfo->Layers[i].type == TimeDelay)
        {
            nn_timedelay_init(pNet, model_index, &pNetInfo->Layers[i], pNet->pNpuMemBase);
        }
    }
}

//TODO: check the size of each clear requirement
uint32_t mid_npu_control(const int32_t model_index, uint32_t ctrl_cmd, void *param)
{
    Net_t *pNet = &g_net;
    NetInfo_t *pNetInfo = pNet->pNetInfo;
    void *pBase = pNet->pNpuMemBase;

    if(model_index >= pNetInfo->NetNum)
    {
        iet_print(IET_PRINT_ALG_DBG, "NET: %d,out[0,%d)\r\n", model_index, pNetInfo->NetNum);
        return MID_NPU_ERR;
    }

    switch(ctrl_cmd)
    {
        case MID_NPU_CMD_BIN_VERSION:
            *(char**)param = pNetInfo->Version;
            break;
        case MID_NPU_CMD_GET_NET_TYPE:
            // TrainType (NetType)
            *(uint16_t*)param = pNetInfo->TrainType[model_index];
            break;    
        case MID_NPU_CMD_CLEAR_SAVE:
        {
            int32_t startId = pNetInfo->NetLayersStartId[model_index];
            int32_t endId = pNetInfo->NetLayersEndId[model_index];
            int32_t g_weight_multiple = 32;
            if(pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
                g_weight_multiple = 64;
            for(int i=startId; i<=endId; i++)
            {
                LayerParam_t *curLayer = &pNetInfo->Layers[i];
                if(curLayer->saveFlag == 1)
                {
                    int32_t *saveAddr = (int32_t *)(pBase + curLayer->addr.saveResultAddr*g_weight_multiple);
                    int32_t saveSize = curLayer->outputShape.H *curLayer->outputShape.W *
                                         ALIGN_16(curLayer->outputShape.C)/2;
                    for(int index=0; index<saveSize; index++)
                    {
                        saveAddr[index] = 0;
                    }
                }
            }
            break;
        }
        case MID_NPU_CMD_CLEARTIMES:
        {
            timedelay_clear(model_index);
            break;
        }
        case MID_NPU_CMD_CLEAR_GRU_STATE:
            nn_GruResetStatus(pNet, model_index, pNet->pNpuMemBase);
            break;
        case MID_NPU_CMD_INIT_PRIVATE_DATA:
            NetInit(pNet, model_index);
            break;
        case MID_NPU_CMD_MODEL_INFO:
            *(char**)param = NULL;
            break;
        case MID_NPU_CMD_LIB_COMPILEDATE:
            *(char**)param = get_lib_compile_date();
            break;
        case MID_NPU_CMD_LIB_VERSION:
            *(char**)param = get_lib_version();
            break;
        case MID_NPU_CMD_SET_KWS_FLASHBASE:
        {
            weight_base_flash = *(uint32_t *)param;
            // iet_print(IET_PRINT_LOG, "weight_base_flash: %08x\r\n", weight_base_flash );
            break;
        }
        case MID_NPU_CMD_SET_MAX_INTENT_NUM:
            break;
        case MID_NPU_CMD_SET_INTENT_THRESHOLD:
            // p_intent_threshold  = (int16_t *)param;
            break;
        case MID_NPU_CMD_NLP_POST_CLEAR:
#ifdef NLP_1CHIP
            // clear the buffering data also to avoid that the words in buffer is spotting
            if(model_index == NET_ID_A105)
            {
                timedelay_clear(NET_ID_A105);
                timedelay_clear(NET_ID_A105_2);
                timedelay_clear(NET_ID_A105_3);
                iet_print(IET_PRINT_ALG_DBG, "POST_CLEA\r\n");
                nn_userdefine_a105_post_init(pNet, -1, NULL, NULL);
            }
#endif
#ifdef NLP_2CHIP
            if(model_index == NET_ID_A105)
            {
                timedelay_clear(NET_ID_A105);
                nn_userdefine_a105_post_init(pNet, -1, NULL, NULL);
            }
#endif
            break;
        case MID_NPU_CMD_NLP_SET_MAX_KEPT_TIME:
        {
#if (defined NLP_1CHIP) || (defined NLP_2CHIP)
            extern int32_t words_max_kept_time_in_frame;
            words_max_kept_time_in_frame = (int)param/32; // 32ms per frame after subsampling, update this value if model changed
#endif
            break;
        }
        case MID_NPU_CMD_SET_SPL:
        {
            // doesn't consider the case that each model with different SPL data
            spl_fifo[0][spl_wr] = (int16_t)((*(uint32_t*)param)&0xFFFF);
            spl_fifo[1][spl_wr] = (int16_t)(((*(uint32_t*)param)>>16)&0xFFFF);
            spl_wr++;
            if(spl_wr == MAX_FRAME_SPL)
            {
                spl_wr = 0;
            }
            break;
        }        
        case MID_NPU_CMD_LAYER_DEBUG_SETTING:
#ifndef __x86_64
            debug_layer_enable = 1;
            debug_frame_index  = (int)param>>16;
            debug_layer_id = ((((int)param&0xFF00)>>8)<<24)>>24;
            debug_inout_setting = (int)param & 0xFF;
            frame_index_to_print = -1;
            iet_print(IET_PRINT_ERROR,"LayerId=%d frameId=%d inout=%d\r\n", debug_layer_id, debug_frame_index, debug_inout_setting);

#endif            
            break;
        case MID_NPU_CMD_LAYER_DEBUG_ENABLE:
#ifndef __x86_64
            debug_layer_enable  = (int)param;
#endif
            break;
        case MID_NPU_CMD_USERDEFINE_CONFIRM:
#ifndef __x86_64
            nn_userdefine_s2_confirm_set(model_index, (int)param);
#else
            UserdefineS2ConfirmSet(model_index, *(int *)param);

#endif
            break;            
        case MID_NPU_CMD_LAYER_INFO_PRINT:
#if (defined __x86_64)&&(defined DEBUG_NET_LAYER)
            DebugNetStruct(pNetInfo);
#endif        
            break;
        case MID_NPU_CMD_SET_KEPT_WORD:
#ifndef __x86_64
            kept_word_flag = (int)param;
#endif
            break;

        default:
            break;
    }
    return MID_NPU_OK;
}
