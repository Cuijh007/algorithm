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
#include "nn_vpu_control.h"
#include "iet_qspi_flash.h"
#include "nn_activation.h"
#include "nn_copy.h"
#include "intent_result.h"

#define BOS            4
#define EOS            2
#define MAX_WORD_NUM   16

// for debug layers
extern int debug_layer_id;      
extern int debug_frame_index;
extern int debug_inout_setting;
extern int frame_index_to_print;
extern int debug_layer_enable;
// for debug intent
extern int stop_frame_index;

/******************************* variable   **********************************/

LayerId4Skip_t skip_layer_id_info = {-1, -1};


/******************************* Function   **********************************/

void print_layer_out(int32_t layer_id)
{
    Net_t *pNet = &g_net;
    NetInfo_t *pNetInfo = pNet->pNetInfo;
    LayerParam_t *pLayers = pNet->pNetInfo->Layers;
    LayerParam_t *curLayer = &pLayers[layer_id];
    iet_print(IET_PRINT_ALG_DBG, "L%d Type %d Size[%d %d %d]\r\n", 
        stop_layer_id, curLayer->type, curLayer->outputShape.H, curLayer->outputShape.W, curLayer->outputShape.C);
    // if(net_output_size>= 1)//8*16)
    {
        volatile uint32_t * pSrc = (volatile uint32_t *)((curLayer->addr.outputAddr)*32+pNet->pNpuMemBase);
        iet_print(IET_PRINT_ALG_DBG, "outAddr: [%p, %04x] save: %04x\r\n", pSrc, curLayer->addr.outputAddr, curLayer->addr.saveResultAddr);
        int extendOutC = ALIGN_16(curLayer->outputShape.C)/2;
        int index = 0;
        int print_c_size = extendOutC;
        int print_h_size = curLayer->outputShape.H;
        int print_w_size = curLayer->outputShape.W;
        if(print_c_size > 4)
            print_c_size = 4;
        if(print_h_size > 8)
            print_h_size = 8;
        for(int j=0; j<print_h_size; j++)
        {
            for(int i=0; i<print_w_size; i++)
            {
                iet_print(IET_PRINT_ALG_DBG, "L%2dF%3d-[%2d %2d]:", layer_id, frame_index_to_print, j, i);
                for(int j=0; j<print_c_size; j++)
                {
                    int32_t value = pSrc[j];
                    int16_t value_0 = (short)(value&0xFFFF);
                    int16_t value_1 = (short)((value>>16)&0xFFFF);
                    iet_print(IET_PRINT_ALG_DBG, "%6d, %6d, ", value_0, value_1);                        
                    index++;
                }
                pSrc += extendOutC;
                iet_print(IET_PRINT_ALG_DBG, "\r\n");
            }
        }
        iet_print(IET_PRINT_ALG_DBG, "\r\n");   

    }    
}

// TODO: update to support input data in both cases: VMEM or PSRAM
int F2wEmbedding(const LayerParam_t *param, void *pBase, int save_flag){
	const int16_t *input_addr = (const int16_t*)(pBase+param->addr.inputAddr*32);
	const int16_t *embedding_addr = (const int16_t*)(param->addr.scaleAddr*64+VPU_PSRAM_BASE);
	int16_t *output_addr = (int16_t *)(pBase+param->addr.outputAddr*32);
    if(save_flag){
        output_addr = (int16_t*)(VPU_PSRAM_BASE+param->addr.saveResultAddr*64);
    }
	for(int c = 0; c < param->inputShape.C; c++){        
		int16_t embedding_index = input_addr[c];
		iet_shell_memcpy32(output_addr, embedding_addr+embedding_index*param->outputShape.C, param->outputShape.C*sizeof(int16_t));
		output_addr += param->outputShape.C;
	}

	return 0;
}

/*
 * Parameters:
 *  input_buffer: word IDs from A105
 *  out_result:
 *      [result_in_string<max 128 short>][original_out of intent net<max 128 short>]
 *        +--> eg. |34-30C|<FL:234_345,DS:0h>                              +              
 *                                                                         +
 *               | A:8*2 for intent| B: 16*2 for slot | C: 16 word ids| <--+
 */
uint32_t intent_start(int32_t model_index, int16_t *input_words, int16_t *out_result)
{
    int32_t start_time, end_time;
    uint32_t ret = 0;

    Net_t *pNet = &g_net;
    NetInfo_t *pNetInfo = pNet->pNetInfo;
    int32_t start_id = pNetInfo->NetLayersStartId[model_index];
    int32_t end_id = pNetInfo->NetLayersEndId[model_index];

    /* Intent running on psram*/
    *(volatile uint32_t *)0x51b004e0 = pNet->pWeight;
    *(volatile uint32_t *)0x51b004e4 = pNet->pWeight;

    /*load vpu code to VPU_CODE_ADDR*/
    load_vcode_layers(model_index, start_id, end_id);

    mid_npu_control(model_index, MID_NPU_CMD_CLEAR_GRU_STATE, NULL);
    mid_npu_control(model_index, MID_NPU_CMD_CLEARTIMES, NULL);
    mid_npu_control(model_index, MID_NPU_CMD_CLEAR_SAVE, NULL);
   
    skip_layer_id_info.start_id = -1;
    skip_layer_id_info.end_id = -1;

    start_time = iet_systick_read();

    ret = NetForward(pNet, model_index, start_id, end_id, input_words, out_result);

    // for debug net output
	// int16_t net_endId = pNetInfo->NetLayersEndId[NET_ID_INTENT];
	// int16_t model_output_size = pNetInfo->Layers[net_endId].outputShape.C;
    // iet_print(IET_PRINT_ALG_DBG, "NetOut:\r\n");
    // for(int i=0; i<model_output_size; i++)
    // {
    //     iet_print(IET_PRINT_ALG_DBG, "%6d,", out_result[i]);
    //     if((i+1)%16==0)
    //     {
    //         iet_print(IET_PRINT_ALG_DBG, "\r\n");
    //     }        
    // }
    // iet_print(IET_PRINT_ALG_DBG, "\r\n");

    skip_layer_id_info.start_id = -1;
    skip_layer_id_info.end_id = -1;

    end_time = iet_systick_read();

    // iet_print(IET_PRINT_ALG_DBG, "Intent_NetForward time:%d\r\n", end_time-start_time);    
    return ret;

}