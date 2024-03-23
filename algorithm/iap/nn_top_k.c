#include "nn_embedding.h"
#include "nn_vpu_control.h"

#define TOP_K   10

static int16_t in[4096];
int nn_top_k(const LayerParam_t *param, void* pBase){
	int32_t *output_addr = (int32_t *)(pBase+param->addr.outputAddr*32);
    int32_t input_size_div_2 = (param->inputShape.C+1)>>1;
    // int16_t in[4096];

    // copy input from vmem to SRAM
    int32_t *p_dst = in;
    int32_t *p_src = (int32_t*)(pBase+param->addr.inputAddr*32);
    for(int i = 0; i<input_size_div_2; i++)
    {
        *p_dst++ = *p_src++;
    }

    // get top K
    for(int i=0; i<TOP_K; i++)
    {
        for(int j=param->inputShape.C-1; j>=i+1; j--)
        {
            if(in[j]>in[j-1])
            {
                int16_t tmp = in[j-1];
                in[j-1] = in[j];
                in[j] = tmp;
            }
        }
    }
    // copy result to output buffer
    int32_t *p_result = (int32_t *)in;
    for(int i=0; i<TOP_K/2; i++)
    {
        output_addr[i] = *p_result++;
    }
	return 0;
}