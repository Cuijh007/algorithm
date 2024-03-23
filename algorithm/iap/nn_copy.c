#include "nn_copy.h"
#include "nn_pooling.h"


int nn_copy(const LayerParam_t *param)
{
	int nothingTodo = 1;
	if(param->activation != LINEAR)
	{
		nn_activation(param, VPU_VMEM_BASE);
		nothingTodo = 0;
	}

	if(param->poolingType != NONE_POOL)
	{
		if(param->activation != LINEAR)
		{
			//copy param->addr.outputAddr to param->addr.inputAddr
		}
		else
		{
			nn_pooling(param, VPU_VMEM_BASE);
			nothingTodo = 0;
		}
	}

	if(nothingTodo == 1)
	{
		//copy param->addr.inputAddr to param->addr.outputAddr
		//iet_print(IET_PRINT_ALG_DBG, "Layer1 nothingTodo\r\n");
		uint32_t *inputAddrA = (uint32_t*)(param->addr.inputAddr*32 + VPU_VMEM_BASE);
		uint32_t *inputAddrB = (uint32_t*)(param->addr.outputAddr*32 + VPU_VMEM_BASE);
		nn_m3_copydata(param->addr.inputAddr, 
                           param->addr.outputAddr,
                           &param->inputShape);
	}
}