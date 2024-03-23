#include "nn_add.h"

int nn_add(const LayerParam_t *param, void* players, void *pBase, int16_t layerIndex)
{
#ifndef ALGORITHM_RELEASE
	LayerParam_t *layers = (LayerParam_t *)players;
	if(param->inputLayerSize != 2)
	{
		iet_print(IET_PRINT_ERROR, "%s %d\r\n", __FUNCTION__, __LINE__);
		return 0;
	}

	const MatrixShape_t *inputShape = &param->inputShape;
	const MatrixShape_t *outputShape = &param->outputShape;
	const uint16_t inputAddrA = param->addr.inputAddr;
	const uint16_t inputAddrB = param->addr.biasDestAddr;
	const uint16_t inputAddrC = param->addr.kernelAddr;
	const uint16_t inputAddrD = param->addr.outputAddr;
	/*iet_print(IET_PRINT_ALG_DBG, "inputAddr:0x%04x, scaleAddr:0x%04x, vectorAddr:0x%04x, outputAddr:0x%04x\r\n",
			param->addr.inputAddr, param->addr.kernelAddr, param->addr.biasDestAddr, param->addr.outputAddr);*/
	int16_t inputAId = param->inputLayerId[0];
	int16_t inputBId = param->inputLayerId[1];
	int32_t inputSize = inputShape->H * inputShape->W * ((inputShape->C+15)/16) * 8;
	if(inputAId != (layerIndex-1) && inputBId != (layerIndex-1))
	{
		//copy inputAId saveresult to inputAddrA
		int32_t *Aaddr = (int32_t*)(pBase + layers[inputAId].addr.saveResultAddr*32);
		int32_t *Adstaddr = (int32_t*)(pBase + inputAddrA*32);
		for(int inputindex=0; inputindex<inputSize; inputindex++)
		{
			Adstaddr[inputindex] = Aaddr[inputindex];
		}
	}
	else
	{
		inputBId = (inputAId == (layerIndex-1))?inputBId:inputAId;
	}
	int32_t *Baddr = (int32_t*)(pBase + layers[inputBId].addr.saveResultAddr*32);
	int32_t *Bdstaddr = (int32_t*)(pBase + inputAddrB*32);
	for(int inputindex=0; inputindex<inputSize; inputindex++)
	{
		Bdstaddr[inputindex] = Baddr[inputindex];
	}

	const int16_t truncate = 0;
	const int16_t EE = 0;
	//add code data 4
	const uint8_t operateCode = 4;
	uint16_t ccfg1_data = 0;
	if(param->activation == RELU){
		ccfg1_data = CCFG1(1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0);
	}else{
		ccfg1_data = CCFG1(1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0);
	}
	const uint8_t strides[2] = {1,1};

	nn_common(inputAddrA, inputAddrB, inputAddrC, inputAddrD, strides, inputShape, outputShape, EE, truncate, ccfg1_data, operateCode, 0, 0, 0);
#endif
	return 0;
}
