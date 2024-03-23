#include "nn_embedding.h"
#include "nn_vpu_control.h"

int nn_embedding(const LayerParam_t *param, void* pBase){
	const int16_t *input_addr = (const int16_t*)(pBase+param->addr.inputAddr*32);
	const int16_t *embedding_addr = (const int16_t*)(param->addr.scaleAddr*64+VPU_PSRAM_BASE);
	int16_t *output_addr = (int16_t *)(pBase+param->addr.outputAddr*32);
    if(param->saveFlag){
        output_addr = (int16_t*)(VPU_PSRAM_BASE+param->addr.saveResultAddr*64);
    }
	for(int c = 0; c < param->inputShape.C; c++){        
		int16_t embedding_index = input_addr[c];
		iet_shell_memcpy32(output_addr, embedding_addr+embedding_index*param->outputShape.C, param->outputShape.C*sizeof(int16_t));
		output_addr += param->outputShape.C;
	}
	return 0;
}