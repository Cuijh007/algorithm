#include "nn_padding.h"
#include "nn_saveresult.h"
#include "nn_common.h"


/**********************************************************************
*param : inputAddrA, inputAddrB, inputAddrC, inputAddrD
*input : inputAddrA, 
*output: inputAddrB
*
*************************************************************************/
int nn_padding(const uint16_t inputAddrA, uint16_t inputAddrB, uint16_t inputAddrC, uint16_t inputAddrD, uint8_t **paddingsize, MatrixShape_t *inputShape)
{
#ifndef ALGORITHM_RELEASE
	//iet_print(IET_PRINT_ALG_DBG, "nn_padding\r\n");
	uint8_t (*paddingSize)[2] = paddingsize;
	uint8_t strides[2] = {1,1};
	uint16_t ExtenC = ((inputShape->C + 15) / 16) * 16;
	MatrixShape_t outputShape = *inputShape;
	outputShape.C = ExtenC;
	//store input data
	nn_copydata(inputAddrA, inputAddrD, inputAddrC, inputAddrB, strides, &outputShape, &outputShape, 1);
    //set dest addr data to zero
    outputShape.H = inputShape->H + paddingSize[0][0] + paddingSize[0][1];
	outputShape.W = inputShape->W + paddingSize[1][0] + paddingSize[1][1];
	nn_copydata(inputAddrB, inputAddrD, inputAddrC, inputAddrA, strides, &outputShape, &outputShape, 0);
	int32_t copyLen = inputShape->W * ExtenC; 
	for(int hindex=0; hindex<inputShape->H; hindex++)
	{
		uint16_t inputAdd = hindex*(inputShape->W)*(ExtenC/16);
		uint16_t copyInputAddr = inputAddrB + inputAdd;
		uint16_t outputAdd = ((hindex+paddingSize[0][0])*(inputShape->W+paddingSize[1][0]+paddingSize[1][1]) + 
							 paddingSize[1][0])*(ExtenC/16);
		uint16_t copyOutputAddr = inputAddrA + outputAdd;
		//iet_print(IET_PRINT_ALG_DBG, "nn_padding copywidthdata\r\n");
		#if 0
		nn_copywidthdata(copyInputAddr, copyOutputAddr, copyLen/16, 1);
		#else
		MatrixShape_t tmpShape;
		tmpShape.H = 1;
		tmpShape.W = 1;
		tmpShape.C = copyLen;
		uint16_t ccfg1_data = CCFG1(1,0,0,0,0,0,0,0,0,0,0);
		nn_common(copyInputAddr, inputAddrC, inputAddrD, copyOutputAddr, strides, &tmpShape, &tmpShape, 1, 0, ccfg1_data, 2, 1, 1, 1);
		#endif
	}
#endif
	return 0;
}
