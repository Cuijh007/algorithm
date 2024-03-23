#include "nn_concate.h"


/* 
 * concate two inputs in C direction, the input data should be aligned in 16 for C direction.
 * NOTE: this function doesn't process the case that the real input C data is not aligned to 16.
 *       special processing should be coded if needed.
 *       all C channel is aliged to 16 for cmd model, recheck if it's valid for denoise
 *       valid for data stored on VMEM or PSRAM
 *
 * the input and output address may be in VPU memeory, should be accessed in INT32 by CPU
 * Don't use memcpy to operate VPU memory, the behavior is no guarentee.
 *
 */
int nn_concate(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase)
{
    LayerParam_t *pLayers = &pNet->pNetInfo->Layers[0];
    int32_t *output = (int32_t *)(pBase + param->addr.outputAddr*32);
    int32_t *inputAddrA = (int32_t *)(pBase + param->addr.inputAddr*32);
    int32_t *inputAddrB = (int32_t *)(pBase + param->addr.inputAddr*32);
 
    if(pNet->pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
    {
        if(pLayers[param->inputLayerId[0]].saveFlag == 1)
        {
            inputAddrA = (int32_t *)((char*)VPU_PSRAM_BASE + pLayers[param->inputLayerId[0]].addr.saveResultAddr*64);
        }
        if(pLayers[param->inputLayerId[1]].saveFlag == 1)
        {
            inputAddrB = (int32_t *)((char*)VPU_PSRAM_BASE + pLayers[param->inputLayerId[1]].addr.saveResultAddr*64);
        }
    }
    else
    {
        if(pLayers[param->inputLayerId[0]].saveFlag == 1)
        {
            inputAddrA = (int32_t *)((char*)pBase + pLayers[param->inputLayerId[0]].addr.saveResultAddr*32);
        }
        if(pLayers[param->inputLayerId[1]].saveFlag == 1)
        {
            inputAddrA = (int32_t *)((char*)pBase + pLayers[param->inputLayerId[1]].addr.saveResultAddr*32);
        }        
    }   
    
    int32_t input1C = pLayers[param->inputLayerId[0]].outputShape.C;
    int32_t input2C = pLayers[param->inputLayerId[1]].outputShape.C;
    int32_t padding_len = ALIGN_16(param->outputShape.C) - input1C -input2C;
    padding_len = padding_len/2;
    
    for(int h=0; h<param->outputShape.H; h++)
    {
        for(int w=0; w<param->outputShape.W; w++)
        {
            int32_t *inputA = inputAddrA;
            int32_t *inputB = inputAddrB;
            for(int i=0; i<input1C/2; i++)
            {
                *output++ = *inputA++;
            }            
            if((input1C & 0x1) == 0)
            {
                for(int i=0; i<input2C/2; i++)
                {
                    *output++ = *inputB++;
                }
                if(input2C & 0x1)
                {
                    *output++ = (*inputB)&0x0000FFFF;
                }
            }
            else
            {
                int32_t tmp = (*inputA)&0x0000FFFF;
                for(int i=0; i<(input2C+1)/2; i++)
                {
                    int32_t data = *inputB++;                    
                    *output++ = tmp | (data<<16);
                    tmp = (data>>16)&0x0000FFFF;
                }
                if((input2C & 0x1) == 0)
                {
                    *output++ = tmp;
                }                
            }

            for(int i=0; i<padding_len; i++)
            {
                *output++ = 0;
            }
         
            inputAddrA += ALIGN_16(input1C)/2;
            inputAddrB += ALIGN_16(input2C)/2;                       
        }
    }

    return 0;
}

