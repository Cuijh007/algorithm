#include "nn_multiply.h"

int nn_multiply(const LayerParam_t *param, void* players, void* pBase)
{
#ifndef ALGORITHM_RELEASE
	LayerParam_t *layers = (LayerParam_t *)players;
	int16_t* inputid = param->inputLayerId;
	int16_t outputC = param->inputShape.H * param->inputShape.W * param->inputShape.C;
	int16_t ExtenOutput = param->inputShape.H * param->inputShape.W * ((param->inputShape.C+15)/16) * 16;
	const uint32_t* inputaddrA = layers[inputid[0]].addr.saveResultAddr == 0 ?
                                 (const uint32_t *)((layers[inputid[0]].addr.outputAddr * 32) + pBase):
                                 (const uint32_t *)((layers[inputid[0]].addr.saveResultAddr * 32) + pBase);
	const uint32_t* inputaddrB = layers[inputid[1]].addr.saveResultAddr == 0 ?
                                 (const uint32_t *)((layers[inputid[1]].addr.outputAddr * 32) + pBase) :
                                 (const uint32_t *)((layers[inputid[1]].addr.saveResultAddr * 32) + pBase);
	uint32_t* outputaddr = param->addr.outputAddr*32 + pBase;
	for(int index=0; index<ExtenOutput/2; index++)
	{
		int tmpA = inputaddrA[index];
		int tmpB = inputaddrB[index];
		int16_t lowdata = (int16_t)((((int32_t)(int16_t)(tmpA&0x0000FFFF)) * (int16_t)(tmpB&0x0000FFFF)) >> (param->truncate[0]));
		int highdata = 0;
		if(index*2<outputC)
		{
			highdata = (int16_t)((((int32_t)(int16_t)((tmpA>>16)&0x0000FFFF)) * (int16_t)((tmpB>>16)&0x0000FFFF)) >> (param->truncate[0]));
		}
		outputaddr[index] = (int32_t)lowdata&0x0000FFFF | ((int32_t)highdata<<16)&0xFFFF0000 ;
		//iet_print(IET_PRINT_ALG_DBG, "%d,%d,", (int16_t)(lowdata), (int16_t)(highdata));
	}
#endif
	return 0;
}

