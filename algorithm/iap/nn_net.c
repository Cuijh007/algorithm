#include "nn_file_struct.h"
#include "nn_net.h"
#include "nn_common.h"
#include "nn_conv.h"
#include "nn_dense.h"
#include "nn_activation.h"
#include "nn_rnndenseadd.h"
#include "nn_vpu_control.h"
#include "nn_copy.h"
#include "nn_userdefine.h"
#include "nn_timedelay.h"
#include "nn_multiply.h"
#include "nn_add.h"
#include "nn_bn.h"
#include "nn_saveresult.h"
#include "nn_concate.h"
#include "nn_layernorm.h"
#include "nn_userdefine_a105_post.h"
#include "nn_userdefine_a105_vad.h"
#include "nn_userdefine_dummy.h"
#include "mid_common.h"
#include "iet_qspi_flash.h"

#include "nn_embedding.h"
#include "nn_embedding_v2.h"
#include "nn_top_k.h"
#include "nn_userdefine_bidirectional.h"
#include "nn_userdefine_gather.h"
#include "nn_userdefine_slot.h"

extern LayerId4Skip_t skip_layer_id_info;

int frame_index_to_print = -1;    // index of the frame to print, used to debug single net


static void SetNetInput(uint32_t *dst, const int16_t *src, MatrixShape_t *inputshape)
{
    //*(int*)BACH2_VPU_CONTROL = 0;
    volatile uint32_t * pDst = (volatile uint32_t *)dst;

    short ExtenC = (inputshape->C+15)/16 * 16;
    for(int index=0; index<inputshape->H * inputshape->W; index++)
    {
        for(int channelindex=0; channelindex<ExtenC; channelindex+=2)
        {
            if((channelindex+1) < inputshape->C){
                *(uint32_t *)(pDst++) = ((uint32_t)(((uint32_t)(src[index*inputshape->C+channelindex])) & 0x0000FFFF) | (uint32_t)((((uint32_t)(src[index*inputshape->C+channelindex+1])) << 16 ) & 0xFFFF0000));
            }
            else if((channelindex+1) == inputshape->C){
                *(uint32_t *)(pDst++) = ((uint32_t)(((uint32_t)(src[index*inputshape->C+channelindex])) & 0x0000FFFF));
            }
            else{
                *(uint32_t *)(pDst++) = 0;
            }
        }
    }
}


// the output is aligned with 4bytes already
// & "outputsize" is aligned with 2 also.
static void GetNetOutput(uint32_t *src, int16_t* output, int outputsize)
{
    //*(int*)VPU_CONTROL_REG = 0x00000000;
    volatile uint32_t * pSrc = (volatile uint32_t *)src;
    for(int index=0; index<(outputsize+1)/2; index++)
    {
        int32_t tmp_data = pSrc[index];
        output[index*2] = (short)(tmp_data&0x0000FFFF);
        output[index*2+1] = (short)((tmp_data>>16)&0x0000FFFF);
    }
}

static void DebugLayerInfo(LayerParam_t *pLayer, int layer_id){
    iet_print(IET_PRINT_LOG, "Layer%d {\r\n", layer_id);
    iet_print(IET_PRINT_LOG, "\tlayerId=%d\r\n", pLayer->layerId);
    iet_print(IET_PRINT_LOG, "\tinputShape [%d %d %d]\r\n", pLayer->inputShape.H, pLayer->inputShape.W, pLayer->inputShape.C);
    iet_print(IET_PRINT_LOG, "\toutputShape[%d %d %d]\r\n", pLayer->outputShape.H, pLayer->outputShape.W, pLayer->outputShape.C);
    iet_print(IET_PRINT_LOG, "\tkernelShape[%d %d %d]\r\n", pLayer->kernelShape.H, pLayer->kernelShape.W, pLayer->kernelShape.C);
    //iet_print(IET_PRINT_LOG, "\tconvOutputShape[%d %d %d]\r\n", pLayer->convOutputShape.H, pLayer->convOutputShape.W, pLayer->convOutputShape.C);
    iet_print(IET_PRINT_LOG, "\tvpuPara[%d %d %d %d]\r\n", pLayer->vpuPara.vpu_layer, pLayer->vpuPara.vpu_run, pLayer->vpuPara.vpu_start_addr, pLayer->vpuPara.vpu_end_addr);
    iet_print(IET_PRINT_LOG, "\tstrides[%d %d]\r\n", pLayer->strides[0],pLayer->strides[1]);
    iet_print(IET_PRINT_LOG, "\tpadding[%d %d %d %d]\r\n",pLayer->padding[0][0], pLayer->padding[0][1], pLayer->padding[1][0], pLayer->padding[1][1] );
    iet_print(IET_PRINT_LOG, "\ttype=%d\r\n", pLayer->type);
    iet_print(IET_PRINT_LOG, "\tactivation=%d\r\n", pLayer->activation);
    iet_print(IET_PRINT_LOG, "\ttarget_rms=%d\r\n", pLayer->target_rms);
    iet_print(IET_PRINT_LOG, "\tuse_b=%d use_s=%d use_v=%d, use_b8=%d, model_q=%d\r\n",
        pLayer->use_b, pLayer->use_s, pLayer->use_v, pLayer->use_b8, pLayer->model_q );
    iet_print(IET_PRINT_LOG, "\ttruncate[%d %d %d %d %d %d]\r\n",
        pLayer->truncate[0],pLayer->truncate[1],pLayer->truncate[2],pLayer->truncate[3],pLayer->truncate[4],pLayer->truncate[5] );
    iet_print(IET_PRINT_LOG, "\tinputLayerId[%d %d %d %d] inputLayerSize=%d\r\n",
        pLayer->inputLayerId[0], pLayer->inputLayerId[1], pLayer->inputLayerId[2], pLayer->inputLayerId[3],pLayer->inputLayerSize);
    iet_print(IET_PRINT_LOG, "\toutputLayerId[%d %d %d %d] outputLayerSize=%d\r\n",
        pLayer->outputLayerId[0], pLayer->outputLayerId[1], pLayer->outputLayerId[2], pLayer->outputLayerId[3],pLayer->outputLayerSize);
    iet_print(IET_PRINT_LOG, "\ttimesize=%d\r\n", pLayer->timesize);
    iet_print(IET_PRINT_LOG, "\taddr[%04x %04x %04x %04x %04x %04x %04x %04x]\r\n",
        pLayer->addr.inputAddr, pLayer->addr.outputAddr, pLayer->addr.kernelAddr, pLayer->addr.biasAddr,
        pLayer->addr.scaleAddr, pLayer->addr.vectorAddr, pLayer->addr.biasDestAddr, pLayer->addr.saveResultAddr);
    iet_print(IET_PRINT_LOG, "\tsaveFlag=%d\r\n", pLayer->saveFlag);
    iet_print(IET_PRINT_LOG, "\tpoolingType=%d Size[%d %d]\r\n", pLayer->poolingType,pLayer->poolingSize[0], pLayer->poolingSize[1]);
    // iet_print(IET_PRINT_LOG, "\tpoolingStrides[%d %d]\r\n", pLayer->poolingStrides[0], pLayer->poolingStrides[1]);
    // iet_print(IET_PRINT_LOG, "\tpoolingPadding[%d %d %d %d]\r\n", pLayer->poolingPadding[0][0], pLayer->poolingPadding[0][1],
    //     pLayer->poolingPadding[1][0],pLayer->poolingPadding[1][1] );
    iet_print(IET_PRINT_LOG, "\textraDataOffset: %d\r\n", pLayer->extraDataOffset);
    iet_print(IET_PRINT_LOG, "\tuserDefine: %08x %08x %08x %08x\r\n", pLayer->userDefine[0], pLayer->userDefine[1], pLayer->userDefine[2], pLayer->userDefine[3]);
    iet_print(IET_PRINT_LOG, "\t}\r\n");

}


void DebugNetStruct(NetInfo_t *pNetInfo)
{
    LayerParam_t *pLayer = NULL;

    iet_print(IET_PRINT_LOG, "version:%s\r\n", pNetInfo->Version);

    iet_print(IET_PRINT_LOG, "layersnum : %d\r\n", pNetInfo->LayersNum);

    for(int k=0; k<pNetInfo->NetNum; k++)
    {
        iet_print(IET_PRINT_LOG, "\r\n\nNet %d [%d %d]:\r\n", k, pNetInfo->NetLayersStartId[k], pNetInfo->NetLayersEndId[k]);
        int start_id = pNetInfo->NetLayersStartId[k];
        int end_id = pNetInfo->NetLayersEndId[k];

        if(k < pNetInfo->NetNum - 1)
        {
            end_id = pNetInfo->NetLayersStartId[k+1];
        }
        if(k == pNetInfo->NetNum - 1)
        {
            end_id = pNetInfo->LayersNum;
        }

        for(int i=start_id; i < end_id; i++)
        {
            pLayer = &pNetInfo->Layers[i];
            DebugLayerInfo(pLayer, i);
        }
        iet_print(IET_PRINT_LOG, "MicNumber: %d\r\n", pNetInfo->MicNumber[k]);
        iet_print(IET_PRINT_LOG, "MicAngle:  %d\r\n", pNetInfo->MicAngle[k]);
        iet_print(IET_PRINT_LOG, "FrameLength:  %d\r\n", pNetInfo->FrameLength[k]);
        iet_print(IET_PRINT_LOG, "FrameOffset:  %d\r\n", pNetInfo->FrameOffset[k]);
        iet_print(IET_PRINT_LOG, "MinCmdLength: %d\r\n", pNetInfo->MinCmdLength[k]);
        iet_print(IET_PRINT_LOG, "FeatureDataOffset: %04x\r\n", pNetInfo->FeatureDataOffset[k]);
        iet_print(IET_PRINT_LOG, "PostDataOffset:    %04x\r\n", pNetInfo->PostDataOffset[k]);
        iet_print(IET_PRINT_LOG, "GruStatusOffset:   ");
        for(int i=0; i<MAX_GRU_NUM_IN_MODEL; i++)
        {
            iet_print(IET_PRINT_LOG, "%04x ", pNetInfo->GruStatusOffset[k][i]);
        }
        iet_print(IET_PRINT_LOG, "\r\n");        
        iet_print(IET_PRINT_ALG_DBG, "WeightMemoryType: %d\r\n", pNetInfo->WeightMemoryType[k]);
        iet_print(IET_PRINT_ALG_DBG, "DataMemoryType: %d\r\n", pNetInfo->DataMemoryType[k]);
        iet_print(IET_PRINT_LOG, "CommandNumber: %04x\r\n", pNetInfo->CommandNumber[k]);
        iet_print(IET_PRINT_LOG, "PostProcessMode:    %04x\r\n", pNetInfo->PostProcessMode[k]);
    }
}


void print_netinfo()
{
     Net_t *pNet = &g_net;
     DebugNetStruct(pNet->pNetInfo);
}

/*
 * Print input/output data of the specified layer at the specified frame index
 * NOTE: this function  is valid for data saved on VMEM only, invalid if data saved on PSRAM
 *
 *pNet:         Pointer to the net
 *layer_id:     index of the layer
 *frame_index:  Index of the frame to print, start from 0; -1 for all frames
 *print_in:     1 for print intput data, 0 for no
 *print_out:    1 for print output data, 0 for no
 *print_hex:    1 for print in hex for 32bit, 0 for short in dec
*/
void DebugLayerData(const Net_t *pNet, int layer_id, int frame_index, int print_in, int print_out, Bool print_hex)
{
#define MAX_PRINT_H_SIZE    40
#define MAX_PRINT_W_SIZE    64
#define MAX_PRINT_C_SIZE    256 
#define MAX_PRINT_W_SIZE_PLACEHOLDER    64

    LayerParam_t *pLayers = pNet->pNetInfo->Layers;
    if(frame_index_to_print == frame_index || frame_index == -1)
    {            
        LayerParam_t *curLayer = &pLayers[layer_id];  
    
        // print input feature data of the current layer
        // print only part of them

        // Need to check if this works if data stored on psram

        if(print_in)
        {
            int upperLayerId = curLayer->inputLayerId[0];
            if(upperLayerId == -1)
            {
                int32_t *pInputAddr = (int32_t *)(pNet->pNpuMemBase + curLayer->addr.inputAddr * 32);
                int out_c = ALIGN_16(curLayer->inputShape.C)/2;    // in INT size
                int wc_size  = curLayer->inputShape.W * out_c;
                            
                iet_print(IET_PRINT_LOG, "\r\nPlaceHold[%d] shape: [%d %d %d] ouAddr %p\r\n", upperLayerId, 
                    curLayer->inputShape.H, curLayer->inputShape.W, curLayer->inputShape.C, pInputAddr);
                int print_h = curLayer->inputShape.H > MAX_PRINT_H_SIZE ? MAX_PRINT_H_SIZE : curLayer->inputShape.H;             
                for(int h=0; h<print_h; h++)
                {
                    //iet_print(IET_PRINT_LOG, "H[%d]\r\n", h);
                    int print_w = curLayer->inputShape.W > MAX_PRINT_W_SIZE_PLACEHOLDER ? MAX_PRINT_W_SIZE_PLACEHOLDER : curLayer->inputShape.W;
                    for(int w=0; w<print_w; w++)
                    {                        
                        iet_print(IET_PRINT_LOG, "IF%dH%2dW%2d:", frame_index_to_print, h, w);
                        int print_c = curLayer->inputShape.C > MAX_PRINT_C_SIZE ? MAX_PRINT_C_SIZE: curLayer->inputShape.C;
                        for(int c=0; c<(print_c+1)/2; c++) // count in INT
                        {
                            if(print_hex)
                            {
                                iet_print(IET_PRINT_LOG, "%08x,", pInputAddr[h*wc_size+w*out_c+c]);
                            }
                            else
                            {
                                int32_t tmp_data = pInputAddr[h*wc_size+w*out_c+c];
                                iet_print(IET_PRINT_LOG, "%6d,%6d,", (short)(tmp_data&0x0000FFFF), (short)((tmp_data>>16)&0x0000FFFF));
                            }
                        }
                        iet_print(IET_PRINT_LOG, "\r\n");
                    }
                }                
                return;
            }
            //while(upperLayerId != 0 && upperLayerId != -1)  // 0 is flag of end of input layer ids here
            for(int i = 0; i<curLayer->inputLayerSize; i++)
            {               
                upperLayerId = curLayer->inputLayerId[i];
                if(upperLayerId != -1)
                {
                    LayerParam_t *upperLayer = &pLayers[upperLayerId];
                    int32_t *pOutAddr = upperLayer->saveFlag==1 ? (int32_t *)(pNet->pNpuMemBase + upperLayer->addr.saveResultAddr * 32) 
                                                                : (int32_t *)(pNet->pNpuMemBase + upperLayer->addr.outputAddr * 32);
                    int out_c = ALIGN_16(upperLayer->outputShape.C)/2;  // in INT size
                    int wc_size  = upperLayer->outputShape.W * out_c;
                    
                    iet_print(IET_PRINT_LOG, "\r\nCurLayer[%d] InLayer[%d] shape: [%d %d %d] ouAddr %p\r\n", layer_id, upperLayerId, 
                        upperLayer->outputShape.H, upperLayer->outputShape.W, upperLayer->outputShape.C, pOutAddr);
                    int print_h = upperLayer->outputShape.H > MAX_PRINT_H_SIZE ? MAX_PRINT_H_SIZE : upperLayer->outputShape.H;             
                    for(int h=0; h<print_h; h++)
                    {
                        //iet_print(IET_PRINT_LOG, "H[%d]:\r\n", h);
                        int print_w = upperLayer->outputShape.W > MAX_PRINT_W_SIZE ? MAX_PRINT_W_SIZE : upperLayer->outputShape.W;
                        for(int w=0; w<print_w; w++)
                        {
                            iet_print(IET_PRINT_LOG, "IF%dH%2dW%2d:", frame_index_to_print, h, w);                        
                            int print_c = upperLayer->outputShape.C > MAX_PRINT_C_SIZE ? MAX_PRINT_C_SIZE: upperLayer->outputShape.C;
                            for(int c=0; c<(print_c+1)/2; c++)  // count in INT
                            {
                                if(print_hex)
                                {
                                    iet_print(IET_PRINT_LOG, "%08x,", pOutAddr[h*wc_size+w*out_c+c]);
                                }
                                else
                                {
                                    int32_t tmp_data = pOutAddr[h*wc_size+w*out_c+c];
                                    iet_print(IET_PRINT_LOG, "%6d,%6d,", (short)(tmp_data&0x0000FFFF), (short)((tmp_data>>16)&0x0000FFFF));
                                }
                            }
                            iet_print(IET_PRINT_LOG, "\r\n");
                        }
                    }
                }
                
            }
        }
    
        // print out feature data of the current layer
        if(print_out)
        {
            
            int32_t *pOutAddr = (int32_t *)(pNet->pNpuMemBase + curLayer->addr.outputAddr * 32);
            int out_c = ALIGN_16(curLayer->outputShape.C)/2;    // in INT size
            int wc_size  = curLayer->outputShape.W * out_c;
                        
            if(print_hex)
            {
                pOutAddr = (int32_t *)(VPU_PSRAM_BASE + curLayer->addr.saveResultAddr * 64);
                print_hex = FALSE;
            }
            if(curLayer->saveFlag==1)
            {
                pOutAddr = (int32_t *)(pNet->pNpuMemBase + curLayer->addr.saveResultAddr * 32);
                iet_print(IET_PRINT_LOG, "\r\nCurLayer[%d] out shape: [%d %d %d]\r\n", layer_id, 
                    curLayer->outputShape.H, curLayer->outputShape.W, curLayer->outputShape.C);
            }
            else
            {
                        
                iet_print(IET_PRINT_LOG, "\r\nCurLayer[%d] out shape: [%d %d %d]\r\n", layer_id, 
                    curLayer->outputShape.H, curLayer->outputShape.W, curLayer->outputShape.C);
            }
            
            int print_h = curLayer->outputShape.H > MAX_PRINT_H_SIZE ? MAX_PRINT_H_SIZE : curLayer->outputShape.H;             
            for(int h=0; h<print_h; h++)
            {
                //iet_print(IET_PRINT_LOG, "H[%d]\r\n", h);
                
                int print_w = curLayer->outputShape.W > MAX_PRINT_W_SIZE ? MAX_PRINT_W_SIZE : curLayer->outputShape.W;
                for(int w=0; w<print_w; w++)
                {
                    iet_print(IET_PRINT_LOG, "OF%dH%2dW%2d:", frame_index_to_print, h, w);
                    int print_c = curLayer->outputShape.C > MAX_PRINT_C_SIZE ? MAX_PRINT_C_SIZE: curLayer->outputShape.C;
                    for(int c=0; c<(print_c+1)/2; c++)  // count int INT
                    {
                        if(print_hex)
                        {
                            iet_print(IET_PRINT_LOG, "%08x,", pOutAddr[h*wc_size+w*out_c+c]);
                        }
                        else
                        {
                            int32_t tmp_data = pOutAddr[h*wc_size+w*out_c+c];
                            iet_print(IET_PRINT_LOG, "%6d,%6d,", (short)(tmp_data&0x0000FFFF), (short)((tmp_data>>16)&0x0000FFFF));
                        }
                    }
                    iet_print(IET_PRINT_LOG, "\r\n");
                }
            }
        }
    }

}

/*
 *pNet:           Pointer to the net
 *model_index:    model index in the net, -1 for all models
*/
int NetInit(const Net_t *pNet, int model_index) 
{
    uint32_t ret = 0;
    NetInfo_t *pNetInfo = pNet->pNetInfo;    
    int32_t start_id = pNetInfo->NetLayersStartId[model_index];
    int32_t end_id = pNetInfo->NetLayersEndId[model_index];

    // init the layers with private data
	for (int i = start_id; i <= end_id; i++) 
    {
        LayerParam_t *curLayer = &pNetInfo->Layers[i];
		switch (curLayer->type)
		{
			case TimeDelay:
                nn_timedelay_init(pNet, model_index, curLayer, pNet->pNpuMemBase);
                break;
            case UserDefineWakeup:
                nn_userdefine_wakeup_init(pNet, model_index, curLayer, pNet->pNpuMemBase);
                break;
            case UserDefineVADPost:
                nn_userdefine_a105_vad_init(pNet, model_index, curLayer, pNet->pNpuMemBase);
                break;
            case UserDefineA105Post:
                nn_userdefine_a105_post_init(pNet, model_index, curLayer, pNet->pNpuMemBase);
                break;
			default:
				break;            
		}
	}
    // reset gru status
    nn_GruResetStatus(pNet, model_index, pNet->pNpuMemBase);

    return ret;
}


// NOTE: set the layer/frame info here to debug
// the following setting is valid only when DEBUG_NET_LAYER is enabled.
#define DEBUG_RPINT_IN_HEX  (FALSE) // TRUE print in 4Bytes hex, FALSE for short
int debug_layer_id = 0xFFFF;        // set the layer id, start from 0,  here to print its input/output feature data, -1 for all layers, default 0xFFFF for invalid one
int debug_frame_index = -1;         // set the frame index(from 0) to debug, -1 for all frames
int debug_inout_setting = 2;        // 1 for input, 2 for out, 3 for both
int debug_layer_enable =  1;        // disable/enable debug info out, 0 disable as default for board,  1 for enable as default for LibVPU


/*
 *pNet:     Pointer to the net
 *input:    First address of input feature data
 *output:   First address of the net output
 *start_id: index of the first layer
 *end_id:   index of the last layer
 *result:   for debug, record layers' output, can be set to NULL
*/
int32_t vcode_offset = 0; // addr - vcode_offset = real addr in VPU_CODE_ADDR
int NetForward(const Net_t *pNet, int model_index, int start_id, int end_id, const int16_t *input, int16_t *output)
{    
    // iet_printf("%d,%d,%d,%d\r\n",skip_layer_id_info.start_id,skip_layer_id_info.end_id,start_id,end_id);
	int Ret = 0;
    LayerParam_t *pLayers = pNet->pNetInfo->Layers;
    // net_output_size: aligned to 16(just as C channel), since for softmax with H !=1 in AM double confirm case,
    //                  need to know the buffer len of each frame.
    // use the last layer output size since this may be called for sub-net.
    int32_t net_output_size = pLayers[end_id].outputShape.W *
                              pLayers[end_id].outputShape.H *
                              ALIGN_16(pLayers[end_id].outputShape.C);
    // Take first softmax out as AM out now, or else other way should be used.
#if (defined NLP_1CHIP) || (defined NLP_2CHIP)
    int first_softmax_saved = 1;	// disable softmax result saving for nlp
#else
    int first_softmax_saved = 0;	// for kws and cmd nets
#endif
    
    if(input)
    {
        SetNetInput((uint32_t*)(pLayers[start_id].addr.inputAddr*32+pNet->pNpuMemBase), input, &pLayers[start_id].inputShape);
    }

    // #ifdef DEBUG_NET_LAYER
    //     if(debug_layer_enable && start_id == pNet->pNetInfo->NetLayersStartId[model_index])
    //     {
    //         // the value is correct only when run a model
    //         frame_index_to_print++;
    //     }
    // #endif
    // iet_print(IET_PRINT_ALG_DBG,"N%d start\r\n", model_index);

    // run layer by layer
    for(int i=start_id; i<=end_id; i++)
    {
#if (defined NLP_1CHIP) || (defined NLP_2CHIP)  
        if(i >= skip_layer_id_info.start_id && i <= skip_layer_id_info.end_id){
            continue;
        }
#endif
        // iet_print(IET_PRINT_ALG_DBG,"N L%d\r\n", i);
#ifdef DEBUG_NET_LAYER
        // the input data area may be used by the op as intermediate area
        // so must print it before processing
        if(debug_layer_enable && (i == debug_layer_id || debug_layer_id == -1))
        {
            // valid for bach2c
            DebugLayerData(pNet, i, debug_frame_index, debug_inout_setting&0x1 , FALSE, DEBUG_RPINT_IN_HEX);
        }        
#endif
        LayerParam_t *curLayer = &pLayers[i];
        //DebugLayerInfo(curLayer, i);
        /*layer run on vpu*/
        if(curLayer->vpuPara.vpu_layer)
        {
            // if(curLayer->layerId == debug_layer_id){
            //     //iet_print(IET_PRINT_ALG_DBG, "vpu_start_addr : %d, vpu_end_addr : %d\r\n", curLayer->vpuPara.vpu_start_addr, curLayer->vpuPara.vpu_end_addr);
            //     uint32_t vpucode_addr = (uint32_t)(VPU_CODE_ADDR+(curLayer->vpuPara.vpu_end_addr*sizeof(uint32_t)));
            //     SET_VCODE_REG(vpucode_addr, 0);               
            // }
            
            if(curLayer->type == TimeDelay)
            {
                if(nn_timedelay_vpu(curLayer) == 1){
                    VpuStart(curLayer->vpuPara.vpu_start_addr - vcode_offset);
                    return MID_NPU_SKIP;
                }
            }
#if (defined NLP_1CHIP) || (defined NLP_2CHIP)  
            if(model_index == NET_ID_INTENT && curLayer->type == UserDefineGather){
                nn_userdefine_gather(curLayer, pLayers, pNet->pNpuMemBase);
            }
#endif
            if(curLayer->vpuPara.vpu_run)
            {
                VpuStart(curLayer->vpuPara.vpu_start_addr - vcode_offset);
            }
           
            continue;
        }

        /*conv2d or dense or gru or other*/
        switch(curLayer->type)
        {
            case Conv2D:
                if(curLayer->inputLayerId[0] != (i-1) && curLayer->inputLayerId[0] != -1)
                {
                    // valid for bach1c only
                    int32_t *save_addr = (int32_t*)(pNet->pNpuMemBase+pLayers[curLayer->inputLayerId[0]].addr.saveResultAddr*32);
                    int32_t *input_addr = (int32_t*)(pNet->pNpuMemBase+curLayer->addr.inputAddr*32);
                    int32_t copy_len = (curLayer->inputShape.H * curLayer->inputShape.W * ALIGN_16(curLayer->inputShape.C));
                    for(int index=0; index < (copy_len>>1); index++)
                    {
                        *input_addr++ = *save_addr++;
                        
                    }
                }
                nn_conv(curLayer);
                break;
            case Dense:
                if(curLayer->inputLayerId[0] != (i-1) && curLayer->inputLayerId[0] != -1)
                {
                    // valid for bach1c only
                    int32_t *save_addr = (int32_t*)(pNet->pNpuMemBase+pLayers[curLayer->inputLayerId[0]].addr.saveResultAddr*32);
                    int32_t *input_addr = (int32_t*)(pNet->pNpuMemBase+curLayer->addr.inputAddr*32);
                    int32_t copy_len = (curLayer->inputShape.H * curLayer->inputShape.W * ALIGN_16(curLayer->inputShape.C));
                    for(int index=0; index < (copy_len>>1); index++)
                    {
                        *input_addr++ = *save_addr++;
                        
                    }
                }
                nn_dense(curLayer);
                break;
            case NormalizeLayer:
                break;
            case Activation:
                nn_activation(curLayer, pNet->pNpuMemBase);
                break;
            case Mul:
                nn_multiply(curLayer, pLayers, pNet->pNpuMemBase);
                break;
            case RNNDenseAdd:
                nn_rnndenseadd(curLayer, pLayers, pNet->pNpuMemBase);
                break;
            case RNNState:
                nn_rnnstate(curLayer, pLayers, pNet->pNpuMemBase);
                break;
            case GRUSpecialTreatment:
                nn_gruspecialtrement(curLayer, pLayers, pNet->pNpuMemBase);
                break;
            case TimeDelay:
                {
                    int ret = nn_timedelay(curLayer, pNet->pNpuMemBase);
                    if(ret != 0){                        
                        return MID_NPU_SKIP;
                    }
                }
                break;
            case Copy:
                if(curLayer->inputLayerId[0] != (i-1) && curLayer->inputLayerId[0] != -1)
                {
                    // this should be needed for bach1c only, vcode will take care of this for bach2c
                    // TODO, this should be removed for bach2c

                    // if save on psram, pBase of save and input/output is different
                    iet_print(IET_PRINT_ERROR, "For nlp algo lib, the input layer MUST be the last one\r\n");
                    break;

                    int32_t *save_addr = (int32_t*)(pNet->pNpuMemBase+pLayers[curLayer->inputLayerId[0]].addr.saveResultAddr*32);
                    int32_t *input_addr = (int32_t*)(pNet->pNpuMemBase+curLayer->addr.inputAddr*32);
                    int32_t copy_len = (curLayer->inputShape.H * curLayer->inputShape.W * ALIGN_16(curLayer->inputShape.C));
                    if(pNet->pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
                    {
                        save_addr = (int32_t *)((char*)VPU_PSRAM_BASE + pLayers[curLayer->inputLayerId[0]].addr.saveResultAddr*64);
                    }                    
                    for(int index=0; index < (copy_len>>1); index++)
                    {
                        *input_addr++ = *save_addr++;                        
                    }
                }
                nn_copy(curLayer);
#if (defined NON_NLP)                
                //to save AM out of normal KWS net, invalid for others
                if(curLayer->activation == SOFTMAX && 
                    start_id == pNet->pNetInfo->NetLayersStartId[model_index] &&
                    first_softmax_saved == 0)
                {
                    int32_t am_output_size = pNet->amOutputSize[model_index];
                    GetNetOutput((uint32_t*)((curLayer->addr.outputAddr)*32+pNet->pNpuMemBase), output+net_output_size, am_output_size);
                    first_softmax_saved = 1;
                }
#endif                                
                break;
            case Add:
                nn_add(curLayer, pLayers, pNet->pNpuMemBase, i);
                break;
            case BN:
                if(curLayer->inputLayerId[0] != (i-1) && curLayer->inputLayerId[0] != -1)
                {
                    int32_t *save_addr = (int32_t*)(pNet->pNpuMemBase+pLayers[curLayer->inputLayerId[0]].addr.saveResultAddr*32);
                    int32_t *input_addr = (int32_t*)(pNet->pNpuMemBase+curLayer->addr.inputAddr*32);
                    int32_t copy_len = (curLayer->inputShape.H * curLayer->inputShape.W * ALIGN_16(curLayer->inputShape.C));
                    for(int index=0; index < (copy_len>>1); index++)
                    {
                        *input_addr++ = *save_addr++;
                        
                    }
                }
                nn_bn(curLayer, pNet->pNpuMemBase);
                break;
            case Reshape:
                break;
            case Concatenate:
                Ret = nn_concate(pNet, model_index, curLayer, pNet->pNpuMemBase);
                break;                
            case UserDefineWakeup:
                nn_userdefine_wakeup(pNet, model_index, curLayer, pNet->pNpuMemBase);
                break;
            case LayerNorm:
                nn_layernorm(pNet, model_index, curLayer, pNet->pNpuMemBase);
                break;
            case LogSoftmax:
                // nn_softmax(curLayer, pNet->pNpuMemBase);
                // nn_log_softmax(curLayer, pNet->pNpuMemBase);
                break;
#if (defined NLP_1CHIP) || (defined NLP_2CHIP)                
            case InitState:
                // init GRUSpecialTreatment state use input data
                if(curLayer->inputLayerId[0] != (i-1) && curLayer->inputLayerId[0] != -1)
                {
                    int32_t *save_addr = (int32_t*)(VPU_PSRAM_BASE + pLayers[curLayer->inputLayerId[0]].addr.saveResultAddr*64);
                    int32_t *input_addr = (int32_t*)(pNet->pNpuMemBase + curLayer->addr.inputAddr*32);
                    int32_t copy_len = (curLayer->inputShape.H * curLayer->inputShape.W * ALIGN_16(curLayer->inputShape.C));
                    for(int index=0; index < (copy_len>>1); index++)
                    {
                        *input_addr++ = *save_addr++;
                        
                    }
                }
                Ret = nn_initstate(curLayer, pLayers, pNet->pNpuMemBase);
                break;
            case UserDefineBiDirectional:
                nn_userdefine_bidirectional(pNet, model_index, &pLayers[i], pNet->pNpuMemBase);
                break;
            case UserDefineSlotPost:
                nn_userdefine_slot(pNet, model_index, &pLayers[i], pNet->pNpuMemBase);
                break;
            case DecoderWordIds:
                nn_decoder_word_ids(curLayer, pNet->pNpuMemBase);
                break;
            case UserDefineGather:
                nn_userdefine_gather(&pLayers[i], pLayers, pNet->pNpuMemBase);
                break;
            case Embedding:
                nn_embedding(&pLayers[i], pNet->pNpuMemBase);
                break;
            case UserDefineA105Post:
                {
                    // to operate easily by nlp_intent, it will use output buffer directly
                    int ret = nn_userdefine_a105_post(pNet, model_index, curLayer, pNet->pNpuMemBase, output);
                    if(ret != RESULT_TYPE_VALID_CMD)
                    {
                        // no valid cmd now, don't callback app
                        return MID_NPU_SKIP;
                    }
                }
                break;
            case UserDefineVADPost:
                {
                    // skip the following layer as per the strides[0] setting
                    int ret = nn_userdefine_a105_vad(pNet, model_index, curLayer, pNet->pNpuMemBase, output);
                    if(ret == RESULT_TYPE_SKIP_NEXT_LAYERS){
                        return MID_NPU_SKIP;                        
                    }
                    else if(ret == RESULT_TYPE_VALID_CMD)
                    {
                        // valid cmd is detected, report to app and stop the processing of the current frame with A105
                        return MID_NPU_OK;
                    }
                }
                break;
#endif                
            case UserDefineDummy:
                {
                    // skip the following layer as per the strides[0] setting
                    int ret = nn_userdefine_dummy(pNet, model_index, curLayer, pNet->pNpuMemBase, output);
                    if(ret == RESULT_TYPE_SKIP_NEXT_LAYERS){
                        return MID_NPU_SKIP;
                    }
                }
                break;
            case CustomEmbeddingV2:
                nn_embedding_v2(curLayer, pNet->pNpuMemBase, input);
                break;
            case TopK:
                nn_top_k(curLayer, pNet->pNpuMemBase);
                break;
            default:
                iet_print(IET_PRINT_ALG_DBG, "UNS layer%d\r\n", curLayer->type);
                break;
        }
#ifdef  __x86_64
        // for bach2c, this is not necessary, ISD2BIN adds vcode for this as this flag
        if(1 == curLayer->saveFlag )
        {
            nn_saveresult(curLayer, pNet->pNpuMemBase,  1);
        }
#endif
        // iet_print(IET_PRINT_ALG_DBG, "%s %d, N%d L%d\r\n", __FUNCTION__, __LINE__, model_index, i);

#ifdef DEBUG_NET_LAYER
        // invalid for bach2C
        // if(debug_layer_enable && (i == debug_layer_id || debug_layer_id == -1))     // set the layer id here to print its input/output feature data
        // {
        //     DebugLayerData(pNet, i, debug_frame_index, FALSE , debug_inout_setting&0x2, DEBUG_RPINT_IN_HEX);
        // }        
#endif        
    }

    // copy the out of net(out buffer of the laster layer) to the out buffer specifiedb by app
    // per cases:
    // 1) 1 Chip: intent write the out to the out buffer specifed by app directly, no operation need here for NLP. do it for kws only.
    // 2) 2 Chip: need to do this for chip 1 just as kws, don't need to do for U2 since intent will do it.
    // 3) print to screen only if in layer debug mode
    //
    // NOTE: when debug layer's output, MUST make sure the output is BIG ENOUGHT to avoid acces out of boundary.
    // only write out to the output buffer for normal running  
    if(0)    
    {
        if(stop_layer_id == 0xFFFF)
        {
            // iet_print(IET_PRINT_ALG_DBG,"N%d %d out %p %d\r\n", model_index, __LINE__, output, net_output_size);

            // here copy the net out of the last layer to the out buffer for the caller, invalid for NLP 1CHIP and u2 of 2CHIP
            // UserDefineA105Post will write the final result to out buffer after parsing the NPU out now,

#if (defined NLP_1CHIP) || (defined NLP_2CHIP)
            LayerParam_t *curLayer = &pLayers[end_id];
            if(output && 
                (model_index == NET_ID_WAKEUP || (curLayer->type != UserDefineA105Post && curLayer->type != UserDefineDummy)))
#else
            if(output)
#endif            
            {
                GetNetOutput((uint32_t*)((pLayers[end_id].addr.outputAddr)*32+pNet->pNpuMemBase), output, net_output_size);
            }

        }
        else if(stop_layer_id == end_id)
        {
            // for wakeup model, the stop_layer_id may be over than end_id
            // for debug on board, only print part of out put data
            DebugLayerData(pNet, end_id, -1, FALSE , TRUE, FALSE);
        }
    }
    // iet_print(IET_PRINT_ALG_DBG,"N%d end\r\n", model_index);

    return MID_NPU_OK;
}


Net_t g_net;
NetInfo_t *g_p_net_info;

/*
 * set the layer to stop on chip, call by app debug code
 * frame_index: -1 stop at the specified layer for each frames
 *              other value: stop only at the specified frame for the specified layer,
 *              eg. stop layer 5, stop frame 10,  0~9 frame will run all layers, stop at layer 5 for frame 10.
 *                  run all layers for frame since 11.
 * 
 *              0 for the first frame
 * vcode_addr: 0: stop at the end of the specifed layer
 *             other value: stop at the spcified vcode addr, this can be used if you want to stop at a special
 *             position of the stop layer. for example if you want to stop a layer that without netinfo info
 *             when simplified netinfo is used
 * 
 * model_index: -1 to reset all models
 * 
 * NOTE:  
 *        1)
 *        The way DOESN'T fit for the debugging of net with loop such as GRU, which need to run the whole net
 *        for the frames before the specified one, stop at the spcified loop only.
 *        For gru case, please make sure the end id is changed to the specified layer also besides changing the
 *        vcode in vcode mem
 *        It's better to use set_npu_stop_layer_by_vcode_mem()
 *        2)
 *        The way update the vcode in netinfo only, plase make sure vcode is reload before each frame running
 */
int stop_model_index = 0;   // model to stop
int stop_layer_id = 0xFFFF; // layer to stop
int stop_frame_index = -1;  // to stop the specified frame, the specified layer
int stop_vcode_addr = 0;    // special vcode addr to stop

void set_debug_layer(int model_index, int layer_id, int frame_index, int vcode_addr)
{
    Net_t *pNet = &g_net;
    NetInfo_t *pNetInfo = pNet->pNetInfo;

    if(model_index == -1 || layer_id == 0xFFFF)
    {
        // to reset the status by the caller
        frame_index_to_print = -1;

        stop_model_index = 0;
        stop_layer_id = layer_id;
        stop_frame_index = -1;
        stop_vcode_addr = 0;

        // reset the all models
        for(int i=0; i<pNetInfo->NetNum; i++)
        {
            NetInit(pNet, i);
        }
        return; 
    }

    iet_print(IET_PRINT_LOG,"Orignal: Net_num: %d startId %d endid: %d\r\n", 
        pNetInfo->NetNum, pNetInfo->NetLayersStartId[model_index], pNetInfo->NetLayersEndId[model_index]);
    LayerParam_t *cur_layer = &pNetInfo->Layers[layer_id];
    if(layer_id <pNetInfo->NetLayersStartId[model_index] || layer_id >pNetInfo->NetLayersEndId[model_index])
    {
        iet_print(IET_PRINT_LOG, "Warning:, layer_id %d out of space [%d %d]\r\n", layer_id,
            pNetInfo->NetLayersStartId[model_index], pNetInfo->NetLayersEndId[model_index]);
        // return;
    }
    if(frame_index == -1)
    {
        if( layer_id <= pNetInfo->NetLayersEndId[model_index])
        {
            pNetInfo->NetLayersEndId[model_index] = layer_id;
            iet_print(IET_PRINT_LOG, "set to stop layer %d net %d, its layers[%d %d]\r\n", 
                layer_id, model_index, pNetInfo->NetLayersStartId[model_index], pNetInfo->NetLayersEndId[model_index]);            
        }
        else
        {
            // check if the setting layer is for stage 2 of wakeup
            LayerParam_t *p_layer;
            for(int index = pNetInfo->NetLayersStartId[model_index]; index <= pNetInfo->NetLayersEndId[model_index]; index++)
            {
                p_layer = &pNetInfo->Layers[index];
                if(p_layer->type == UserDefineWakeup)
                {
                    // for 2nd stage of wakeup
                    WakeupModelSetting_t *p_model_setting = (WakeupModelSetting_t *)&p_layer->userDefine;
                    if(layer_id >= p_model_setting->s2_startId && layer_id <= p_model_setting->s2_endId)
                    {
                        p_model_setting->s2_endId = layer_id;
                        iet_print(IET_PRINT_LOG, "set to stop layer %d of wakeup, its layers[%d %d]\r\n", 
                            layer_id, p_model_setting->s2_startId, p_model_setting->s2_endId);
                    }
                }
            }

            
        }
        set_npu_stop_layer(model_index, layer_id, vcode_addr);
        iet_print(IET_PRINT_LOG,"to run: layer %d - %d for each frame\r\n", pNetInfo->NetLayersStartId[model_index], pNetInfo->NetLayersEndId[model_index]);
    }
    else
    {
        iet_print(IET_PRINT_LOG,"to stop at layer %d  of frame %d\r\n", layer_id, frame_index);    
    }
    // iet_print(IET_PRINT_LOG,"pNetInfo: %p vcode addr: %p\r\n", pNetInfo, pNetInfo->VpuCode[model_index][model_index]);
    iet_print(IET_PRINT_LOG,"****Make sure netInfo is reloaded if calling this multi times ****\r\n");

    frame_index_to_print = -1;

    stop_model_index = model_index;
    stop_layer_id = layer_id;
    stop_frame_index = frame_index;
    stop_vcode_addr = vcode_addr;

}

/* 
 * when call this function, make sure vcode will load by Netforward() or mid_npu_start() each frame
 * here change the vcode stored in sram, it will be reload from flash when app reset flow
 * so we can still run multi models if part of models is called by the layer of the start one
 * NOTE: here DOESN't change any content of VPU_CODE memory
 *       CHANGE VCODE in net info memory only, it will be reload from flash by app debug code when reset 
 */
void set_npu_stop_layer(int model_index, int layer_id, int vcode_addr)
{
    Net_t *pNet = &g_net;
    NetInfo_t *pNetInfo = pNet->pNetInfo;
    LayerParam_t *cur_layer = &pNetInfo->Layers[layer_id];
    // MUST set vpu_run flag to let it run here,
    // or else vpu may not start since the vpu_run flag is in layer after the setting one
    pNetInfo->Layers[layer_id].vpuPara.vpu_run= 2;
    iet_print(IET_PRINT_LOG,"NPU stop at %d of frame %d, vpuPara: %d %d %d %d\r\n", layer_id, stop_frame_index,
        cur_layer->vpuPara.vpu_layer, cur_layer->vpuPara.vpu_run,
        cur_layer->vpuPara.vpu_start_addr, cur_layer->vpuPara.vpu_end_addr);

    iet_print(IET_PRINT_LOG,"In [%d %d %d], out[%d %d %d]\r\n", 
                    cur_layer->inputShape.H, cur_layer->inputShape.W, cur_layer->inputShape.C,
                    cur_layer->outputShape.H, cur_layer->outputShape.W, cur_layer->outputShape.C);
    int vcode_index  = cur_layer->vpuPara.vpu_end_addr;
    if(vcode_addr)
    {
        vcode_index = vcode_addr;
    }

    // get the start address of the vcode for the specified net in memory to hold all vcode
    uint16_t *p_vcode = (uint16_t *)pNet->pVpuCode;
    for(int i=0; i<model_index; i++)
    {
        p_vcode += pNet->pNetInfo->VpuCodeSize[i];
    }    
    p_vcode += vcode_index;
  
    if(cur_layer->type == TimeDelay && cur_layer->strides[0] != 1)
    {
        /* for timedely with stride[0] != 1, the'vpu_end_addr' is not the real vcode end addr of this layer
         * it's the addr to change vcode to control the running of the layer
         * two cases:
         * Interval mode: it will drop the input frames if frame count%strides[0] != 0, the 'vpu_end_addr' is the
         * (start addr + 1). we cannot get the real end addr now
         * Normal mode: it will skip the following layers. the 'vpu_end_addr' is (real end addr - 3),
         * even we set it to 0 here, the timedelay() will reset it to control the running of this layer
         * so we set it to next one to make sure this layer can exit as the debug expected.
         * 
         * NOTE: vpu_start_addr is not always the real addr of this layer's vcode, it may include the code for the
         * upper layer
         */
        if(cur_layer->timesType == NORMAL_MODE)
        {
            p_vcode += 1;
        }
        else if(cur_layer->timesType == INTERVAL_MODE)
        {
            p_vcode += 45;  //45: offset to the real end addr, right for the vcode of NLP2.0, need to check for other version
        }

    }
    *((volatile int16_t *)p_vcode) = 0;
    iet_print(IET_PRINT_LOG,"vcode_addr: %d(%p) set to 0\r\n", vcode_index, p_vcode);  
}

/*
 * Use for debug gru etc cases with loop
 * Call this function to change the vcode loaded in vcode memory.
 * To debug cases such as multi GRU calling, eg. intent forward GRU, when try to stop at the specifed loop
 * When this function is called, the loop after it is invalid since vcode is changed.
 * vcode memory should be accessed in INT32, each cmd holds a INT32
 * For gru case, please make sure the end id is changed to the specified layer also besides changing the
 *        vcode in vcode mem
 */
void set_npu_stop_layer_by_vcode_mem(int model_index, int layer_id, int vcode_addr)
{
    Net_t *pNet = &g_net;
    NetInfo_t *pNetInfo = pNet->pNetInfo;
    LayerParam_t *cur_layer = &pNetInfo->Layers[layer_id];
    // MUST set vpu_run flag to let it run here,
    // or else vpu may not start since the vpu_run flag is in layer after the setting one
    pNetInfo->Layers[layer_id].vpuPara.vpu_run= 2;
    iet_print(IET_PRINT_LOG,"NPU stop at %d of frame %d, vpuPara: %d %d %d %d\r\n", layer_id, stop_frame_index,
        cur_layer->vpuPara.vpu_layer, cur_layer->vpuPara.vpu_run,
        cur_layer->vpuPara.vpu_start_addr, cur_layer->vpuPara.vpu_end_addr);

    iet_print(IET_PRINT_LOG,"In [%d %d %d], out[%d %d %d]\r\n", 
                    cur_layer->inputShape.H, cur_layer->inputShape.W, cur_layer->inputShape.C,
                    cur_layer->outputShape.H, cur_layer->outputShape.W, cur_layer->outputShape.C);
    int vcode_index  = cur_layer->vpuPara.vpu_end_addr - vcode_offset;
    if(vcode_addr)
    {
        vcode_index = vcode_addr - vcode_offset;
    }

    // get the start address of the vcode for the specified net in memory to hold all vcode
    uint32_t *p_vcode = (uint32_t *)VPU_CODE_ADDR; 
    p_vcode += vcode_index;
  
    if(cur_layer->type == TimeDelay && cur_layer->strides[0] != 1)
    {
        /* for timedely with stride[0] != 1, the'vpu_end_addr' is not the real vcode end addr of this layer
         * it's the addr to change vcode to control the running of the layer
         * two cases:
         * Interval mode: it will drop the input frames if frame count%strides[0] != 0, the 'vpu_end_addr' is the
         * (start addr + 1). we cannot get the real end addr now
         * Normal mode: it will skip the following layers. the 'vpu_end_addr' is (real end addr - 3),
         * even we set it to 0 here, the timedelay() will reset it to control the running of this layer
         * so we set it to next one to make sure this layer can exit as the debug expected.
         * 
         * NOTE: vpu_start_addr is not always the real addr of this layer's vcode, it may include the code for the
         * upper layer
         */
        if(cur_layer->timesType == NORMAL_MODE)
        {
            p_vcode += 1;
        }
        else if(cur_layer->timesType == INTERVAL_MODE)
        {
            p_vcode += 45;  //45: offset to the real end addr, right for the vcode of NLP2.0, need to check for other version
        }

    }
    *((volatile int32_t *)p_vcode) = 0;
    iet_print(IET_PRINT_LOG,"vcode_addr: %d(%p) set to 0\r\n", vcode_index, p_vcode);  
}
