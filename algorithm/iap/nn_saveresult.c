#include "nn_saveresult.h"
#include "nn_vpu_control.h"
#include "nn_common.h"

// for bach2c, ISD2BIN will add vcode to process this if saveflag =1
void nn_saveresult(const LayerParam_t *param, void *pBase, int16_t save_flag)
{
#ifndef ALGORITHM_RELEASE	
	#if 0  // for bach1c
	//iet_print(IET_PRINT_ALG_DBG, "nn_saveresult\r\n");
	*(int*)VPU_CONTROL_REG = 0x00000000;
	int16_t ExtenOutputC = (param->outputShape.C + 15)/16 * 16;
	int16_t output_channels = param->outputShape.H * param->outputShape.W;
	uint16_t ccfg0_data = 0;
	uint16_t ccfg1_data = 0;

	ccfg0_data = CCFG0(0, 0 ,0 ,0 ,0);
	setRegistData(COPY_SAVE_CCFG0, ccfg0_data, 0, 16);
	ccfg1_data = CCFG1(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	setRegistData(COPY_SAVE_CCFG1, ccfg1_data, 0, 16);
	setRegistData(COPY_SAVE_NUM_L, output_channels, 0, 8);
	setRegistData(COPY_SAVE_INPUTSTEP_L, ExtenOutputC/16, 0, 8);
	setRegistData(COPY_SAVE_OUTPUTSTEP_L, ExtenOutputC/16, 0, 8);
	setRegistData(COPY_SAVE_INPUTADDR_H, (param->addr.outputAddr>>8)&0x00FF, 0, 8);
	setRegistData(COPY_SAVE_INPUTADDR_L, (param->addr.outputAddr)&0xFF, 0, 8);
	setRegistData(COPY_SAVE_OUTPUTADDR_H, (param->addr.saveResultAddr>>8)&0x00FF, 0, 8);
	setRegistData(COPY_SAVE_OUTPUTADDR_L, (param->addr.saveResultAddr)&0x00FF, 0, 8);
	setRegistData(COPY_SAVE_ENABLE, save_flag, 0, 8);
	setRegistData(COPY_SAVE_SCR_NUM, ExtenOutputC/16, 0, 8);
	VpuStart(444);
    VpuWaitDone(0);
	*(int*)VPU_CONTROL_REG = 0x00000000;
	#else
	int16_t ExtenOutputC = (param->outputShape.C + 15)/16 * 16;
	int32_t outputSize = param->outputShape.H * param->outputShape.W * ExtenOutputC;
	const int32_t *input_addr = param->addr.outputAddr * 32 + pBase;
	int32_t *output_addr = param->addr.saveResultAddr*32 + pBase;
	for(int index=0; index<outputSize/2; index++)
	{
		if(save_flag == 1)
		{
			output_addr[index] = input_addr[index];
		}
		else
		{
			output_addr[index] = 0;
		}
	}
	#endif
#endif
	return ;
}


/*********************************
*
*note: len need div 16
*
*********************************/
void nn_copywidthdata(const uint16_t inputAddr,
						  const uint16_t outputAddr,
						  const int32_t len,
						  uint16_t save_flag)
{
//TODO: the following is for bach1c, rewrite this if it's needed.    
#if 0
	//清空VPU标志位，使VPU寄存器可写
	*(int*)VPU_CONTROL_REG = 0x00000000;
	uint16_t ccfg0_data = 0;
	uint16_t ccfg1_data = 0;
	ccfg0_data = CCFG0(0, 0 ,0 ,0 ,0);
	setRegistData(COPY_SAVE_CCFG0, ccfg0_data, 0, 16);
	ccfg1_data = CCFG1(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	setRegistData(COPY_SAVE_CCFG1, ccfg1_data, 0, 16);
	setRegistData(COPY_SAVE_NUM_L, 1, 0, 8);
	setRegistData(COPY_SAVE_INPUTSTEP_L, len, 0, 8);
	setRegistData(COPY_SAVE_OUTPUTSTEP_L, len, 0, 8);
	setRegistData(COPY_SAVE_INPUTADDR_H, (inputAddr>>8)&0x00FF, 0, 8);
	setRegistData(COPY_SAVE_INPUTADDR_L, (inputAddr)&0xFF, 0, 8);
	setRegistData(COPY_SAVE_OUTPUTADDR_H, (outputAddr>>8)&0x00FF, 0, 8);
	setRegistData(COPY_SAVE_OUTPUTADDR_L, (outputAddr)&0x00FF, 0, 8);
	setRegistData(COPY_SAVE_ENABLE, save_flag, 0, 8);
	setRegistData(COPY_SAVE_SCR_NUM, len, 0, 8);
	VpuStart(445);
    VpuWaitDone(0);
	*(int*)VPU_CONTROL_REG = 0x00000000;
#endif    
}


void nn_copydata(const uint16_t inputAddrA, 
					  const uint16_t inputAddrB,
					  const uint16_t inputAddrC,
					  const uint16_t inputAddrD, 
					  const uint8_t *strides, 
					  const MatrixShape_t *inputShape, 
					  const MatrixShape_t *outputShape,
					  const int16_t EE)
{
	//iet_print(IET_PRINT_ALG_DBG, "nn_copydata\n");
	uint16_t ccfg1_data = CCFG1(1,0,0,0,0,0,0,0,0,0,0);
	nn_common(inputAddrA, inputAddrB, inputAddrC, inputAddrD, strides, inputShape, outputShape, EE, 0, ccfg1_data, 2, 0, 1, 1);
}

void nn_m3_copydata(const uint16_t inputAddrA,
						const uint16_t inputAddrB,
						const MatrixShape_t *inputShape)
{
	int32_t outputSize = inputShape->H * inputShape->W * ALIGN_16(inputShape->C);
	const uint32_t *input_addr = (const uint32_t*)(inputAddrA * 32 + VPU_VMEM_BASE);
	uint32_t *output_addr = (uint32_t*)(inputAddrB*32 + VPU_VMEM_BASE);
	for(int index=0; index<outputSize/2; index++)
	{
		output_addr[index] = input_addr[index];

	}
}
