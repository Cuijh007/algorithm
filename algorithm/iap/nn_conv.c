#include "nn_file_struct.h"
#include "nn_conv.h"
#include "nn_vpu_control.h"
#include "nn_common.h"
#include "nn_pooling.h"
#include "nn_padding.h"

#define CONV_VCODE_DEBUG 0

int Conv2D_Q_new(const LayerParam_t *param)
{
#ifndef ALGORITHM_RELEASE
	unsigned short start_addr = 9;
	int loadw_len = 0;
	int copy_len = 0;
	int op_num = 0;
	int elp = 0;
	int load_dst;
	uint16_t biasDestAddr, convOutputAddr;
	int kernel_c_align = (param->kernelShape.C+15)/16*16;
	int convoutput_c_align = (param->convOutputShape.C+15)/16*16;

	if((param->kernelShape.H*param->kernelShape.W) % 2 == 1){
		biasDestAddr = param->addr.outputAddr;
		convOutputAddr = param->addr.biasDestAddr;
	}else{
		biasDestAddr = param->addr.biasDestAddr;
		convOutputAddr = param->addr.outputAddr;
	}

	//do conv
	int16_t channel_kernel_add = 0;
	int16_t conv_kernel_step = ((param->convOutputShape.C + 15) / 16) * 16;
	MatrixShape_t inputShape = param->inputShape;
	inputShape.H = inputShape.H + param->padding[0][0] + param->padding[0][1];
	inputShape.W = inputShape.W + param->padding[1][0] + param->padding[1][1];

	/*set copy bias code*/
	loadw_len = (kernel_c_align*16 + 511)/512 - 1;
	load_dst = biasDestAddr>>14;
	SET_VCODE_REG(CONV_HP_BIAS_CCFG2, CCFG2(loadw_len, load_dst));
	SET_VCODE_REG(CONV_HP_BIAS_CCFG3, CCFG3(param->addr.biasAddr, 0));
	SET_VCODE_REG(CONV_HP_BIAS_CCFG4, CCFG4(param->addr.biasAddr));
	SET_VCODE_REG(CONV_HP_BIAS_LOADW, LOADW(R15, 0, loadw_len, 0));
	
	SET_VCODE_REG(CONV_HP_DOTC_ELP_L, LOADL(R4, param->kernelShape.W));		
	if(param->model_q == 1)
	{
		SET_VCODE_REG(CONV_HP_DOTC_EBI_L, LOADL(R4, conv_kernel_step/4));
		SET_VCODE_REG(CONV_HP_DOTC_CFGG0, CCFG0(conv_kernel_step/4, 0, param->truncate[0], 1, 0, 1));
		SET_VCODE_REG(CONV_HP_DOTC_NUM, EXEC(DOTC, conv_kernel_step/4));
		channel_kernel_add = param->kernelShape.W *  conv_kernel_step / 4;
		loadw_len = (param->kernelShape.H*param->kernelShape.W*kernel_c_align*4*16 + 511)/512 - 1;
	}
	else
	{
		if(param->use_b8 == 1){
			SET_VCODE_REG(CONV_HP_DOTC_EBI_L, LOADL(R4, conv_kernel_step/2));
			SET_VCODE_REG(CONV_HP_DOTC_CFGG0, CCFG0(conv_kernel_step/2, 0, param->truncate[0], 0, 1, 1));
			SET_VCODE_REG(CONV_HP_DOTC_NUM, EXEC(DOTC, conv_kernel_step/2));
			channel_kernel_add = param->kernelShape.W *  conv_kernel_step/2;
		}else{
			SET_VCODE_REG(CONV_HP_DOTC_EBI_L, LOADL(R4, conv_kernel_step));
			SET_VCODE_REG(CONV_HP_DOTC_CFGG0, CCFG0(conv_kernel_step, 0, param->truncate[0], 0, 0, 1));
			SET_VCODE_REG(CONV_HP_DOTC_NUM, EXEC(DOTC, conv_kernel_step));
			channel_kernel_add = param->kernelShape.W *  conv_kernel_step ;
		}
	}
	
	

	SET_VCODE_REG(CONV_HP_DOTC_CFGG1, CCFG1(0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0));
	SET_VCODE_REG(CONV_HP_DOTC_CCFG2, CCFG2(loadw_len, 1));
	SET_VCODE_REG(CONV_HP_DOTC_CCFG3, CCFG3(param->addr.kernelAddr, 0));
	SET_VCODE_REG(CONV_HP_DOTC_CCFG4, CCFG4(param->addr.kernelAddr));
	SET_VCODE_REG(CONV_HP_DOTC_LOADW, LOADW(R15, 0, loadw_len, 0));

	SET_VCODE_REG(CONV_HP_DOTC_INPUT_ADDR_H, LOADH(R0, param->addr.inputAddr));
	SET_VCODE_REG(CONV_HP_DOTC_INPUT_ADDR_L, LOADL(R0, param->addr.inputAddr));
	SET_VCODE_REG(CONV_HP_DOTC_KERNEL_ADDR_H, LOADH(R1, 0x4000));
	SET_VCODE_REG(CONV_HP_DOTC_KERNEL_ADDR_L, LOADL(R1, 0x4000));
	SET_VCODE_REG(CONV_HP_DOTC_BIAS_ADDR_H, LOADH(R2, biasDestAddr));
	SET_VCODE_REG(CONV_HP_DOTC_BIAS_ADDR_L, LOADL(R2, biasDestAddr));
	SET_VCODE_REG(CONV_HP_DOTC_OUTPUT_ADDR_H, LOADH(R3, convOutputAddr));
	SET_VCODE_REG(CONV_HP_DOTC_OUTPUT_ADDR_L, LOADL(R3, convOutputAddr));

	// if((param->kernelShape.H*param->kernelShape.W)%2 == 1){
	// 	SET_VCODE_REG(CONV_HP_DOTC_OUTPUT_ADDR_H, LOADH(R3, param->addr.biasDestAddr));
	// 	SET_VCODE_REG(CONV_HP_DOTC_OUTPUT_ADDR_L, LOADL(R3, param->addr.biasDestAddr));
	// 	SET_VCODE_REG(CONV_HP_DOTC_DESTBIAS_ADDR_H, LOADH(R4, param->addr.outputAddr));
	// 	SET_VCODE_REG(CONV_HP_DOTC_DESTBIAS_ADDR_L, LOADL(R4, param->addr.outputAddr));
	// }else{
	// 	SET_VCODE_REG(CONV_HP_DOTC_OUTPUT_ADDR_H, LOADH(R3, param->addr.outputAddr));
	// 	SET_VCODE_REG(CONV_HP_DOTC_OUTPUT_ADDR_L, LOADL(R3, param->addr.outputAddr));
	// 	SET_VCODE_REG(CONV_HP_DOTC_DESTBIAS_ADDR_H, LOADH(R4, param->addr.biasDestAddr));
	// 	SET_VCODE_REG(CONV_HP_DOTC_DESTBIAS_ADDR_L, LOADL(R4, param->addr.biasDestAddr));
	// }

	SET_VCODE_REG(CONV_HP_DOTC_OUTPUT_H_H, LOADH(R11, param->convOutputShape.H));
	SET_VCODE_REG(CONV_HP_DOTC_OUTPUT_H_L, LOADL(R11, param->convOutputShape.H));
	SET_VCODE_REG(CONV_HP_DOTC_OUTPUT_W_H, LOADH(R12, param->convOutputShape.W));
	SET_VCODE_REG(CONV_HP_DOTC_OUTPUT_W_L, LOADL(R12, param->convOutputShape.W));
	SET_VCODE_REG(CONV_HP_DOTC_KERNEL_H_H, LOADH(R13, param->kernelShape.H));
	SET_VCODE_REG(CONV_HP_DOTC_KERNEL_H_L, LOADL(R13, param->kernelShape.H));
		
	int16_t channel_input_for = inputShape.W/127;
	if(channel_input_for == 0)
	{
		SET_VCODE_REG(CONV_HP_DOTC_INPUT_FOR_NUM_L, LOADL(R14, 1));
		SET_VCODE_REG(CONV_HP_DOTC_INPUT_FOR_STEP_L, INC(R4, 0));
	}
	else
	{
		SET_VCODE_REG(CONV_HP_DOTC_INPUT_FOR_NUM_L, LOADL(R14, channel_input_for));
		SET_VCODE_REG(CONV_HP_DOTC_INPUT_FOR_STEP_L, INC(R4, 127));
	}
	SET_VCODE_REG(CONV_HP_DOTC_INPUT_LAST_NUM_L, INC(R4, inputShape.W - channel_input_for*127));
	
	int16_t channel_kernel_for = channel_kernel_add/127;
	if(channel_kernel_for == 0)
	{
		SET_VCODE_REG(CONV_HP_DOTC_KERNEL_FOR_NUM_L, LOADL(R14, 1));
		SET_VCODE_REG(CONV_HP_DOTC_KERNEL_FOR_STEP_L, INC(R5, 0));
	}
	else
	{
		SET_VCODE_REG(CONV_HP_DOTC_KERNEL_FOR_NUM_L, LOADL(R14, channel_kernel_for));
		SET_VCODE_REG(CONV_HP_DOTC_KERNEL_FOR_STEP_L, INC(R5, 127));
	}
	SET_VCODE_REG(CONV_HP_DOTC_KERNEL_LAST_NUM_L, INC(R5, (channel_kernel_add-channel_kernel_for*127)));
	
	SET_VCODE_REG(CONV_HP_DOTC_INPUT_ADD_NUM_L, INC(R0, param->strides[1]));
	SET_VCODE_REG(CONV_HP_DOTC_DESTBIAS_ADD_NUM_L, INC(R2, 0));
	SET_VCODE_REG(CONV_HP_DOTC_OUTPUT_ADD_NUM_L, INC(R3, conv_kernel_step/16));

	int16_t dotc_intput_step = param->strides[1];
	int16_t dotc_h_input_step = ((inputShape.W - param->convOutputShape.W*param->strides[0])+(param->strides[1] -1))*1;
	SET_VCODE_REG(CONV_HP_DOTC_INPUT_HADD_NUM_L, INC(R0, dotc_h_input_step));
	
	/*mulc*/
	loadw_len = (kernel_c_align*16 + 511)/512 - 1;
	SET_VCODE_REG(CONV_HP_SCALE_CCFG2, CCFG2(loadw_len, 1));
	SET_VCODE_REG(CONV_HP_SCALE_CCFG3, CCFG3(param->addr.scaleAddr, 0));
	SET_VCODE_REG(CONV_HP_SCALE_CCFG4, CCFG4(param->addr.scaleAddr));
	//SET_VCODE_REG(CONV2D_CCFGF_SCALE, CCFGF(0,0,0,0, op_num));
	SET_VCODE_REG(CONV_HP_SCALE_LOADW, LOADW(R15, 0, loadw_len, 0));

	load_dst = param->addr.inputAddr>>14;
	SET_VCODE_REG(CONV_HP_VECTOR_CCFG2, CCFG2(loadw_len, load_dst));
	SET_VCODE_REG(CONV_HP_VECTOR_CCFG3, CCFG3(param->addr.vectorAddr, 0));
	SET_VCODE_REG(CONV_HP_VECTOR_CCFG4, CCFG4(param->addr.vectorAddr));
	//SET_VCODE_REG(CONV2D_CCFGF_VECTOR, CCFGF(0,0,0,0, op_num));
	SET_VCODE_REG(CONV_HP_VECTOR_LOADW, LOADW(R15, 0, loadw_len, 0));
	
	SET_VCODE_REG(CONV_HP_MULC_ELP_H, LOADH(R4, param->convOutputShape.H*param->convOutputShape.W));
	SET_VCODE_REG(CONV_HP_MULC_ELP_L, LOADL(R4, param->convOutputShape.H*param->convOutputShape.W));
	SET_VCODE_REG(CONV_HP_MULC_EADI_H, LOADH(R4, conv_kernel_step/16));
	SET_VCODE_REG(CONV_HP_MULC_EADI_L, LOADL(R4, conv_kernel_step/16));
	SET_VCODE_REG(CONV_HP_MULC_CCFG0, CCFG0(conv_kernel_step/16, 0, param->truncate[1], 0, 0, 0));
	int16_t relu = param->activation == RELU ? 1 : 0;
	SET_VCODE_REG(CONV_HP_MULC_CCFG1, CCFG1(1, 1, 1, 0, relu, 0, 0, 0, 0, 0, 0));
	SET_VCODE_REG(CONV_HP_MULC_INPUT_ADDR_H, LOADH(R0, convOutputAddr));
	SET_VCODE_REG(CONV_HP_MULC_INPUT_ADDR_L, LOADL(R0, convOutputAddr));
	SET_VCODE_REG(CONV_HP_MULC_SCALE_ADDR_H, LOADH(R1, 0x4000));
	SET_VCODE_REG(CONV_HP_MULC_SCALE_ADDR_L, LOADL(R1, 0x4000));
	SET_VCODE_REG(CONV_HP_MULC_VECTOR_ADDR_H, LOADH(R2, param->addr.inputAddr));
	SET_VCODE_REG(CONV_HP_MULC_VECTOR_ADDR_L, LOADL(R2, param->addr.inputAddr));
	SET_VCODE_REG(CONV_HP_MULC_OUTPUT_ADDR_H, LOADH(R3, param->addr.outputAddr));
	SET_VCODE_REG(CONV_HP_MULC_OUTPUT_ADDR_L, LOADL(R3, param->addr.outputAddr));
	if(param->use_v){
		SET_VCODE_REG(CONV_HP_MULC_NUM, EXEC(MULC, conv_kernel_step/16));
	}else{
		SET_VCODE_REG(CONV_HP_MULC_NUM, EXEC(MUL, conv_kernel_step/16));
	}
	VpuStart(start_addr);
#endif
	return 0;
}


int Conv2D_Common_new(const LayerParam_t *param)
{
#ifndef ALGORITHM_RELEASE
	unsigned short start_addr = 127;
	uint8_t strides[2] = {1,1};
	int loadw_len = 0;
	int copy_len = 0;
	int op_num = 0;
	int elp = 0;
	int load_dst;
	uint16_t biasDestAddr, convOutputAddr;
	int kernel_c_align = (param->kernelShape.C+15)/16*16;
	int convoutput_c_align = (param->convOutputShape.C+15)/16*16;
#if 1
	MatrixShape_t inputShape = param->inputShape;
	inputShape.H = inputShape.H + param->padding[0][0] + param->padding[0][1];
	inputShape.W = inputShape.W + param->padding[1][0] + param->padding[1][1];

	if((param->kernelShape.H*param->kernelShape.W) % 2 == 1){
		biasDestAddr = param->addr.outputAddr;
		convOutputAddr = param->addr.biasDestAddr;
	}else{
		biasDestAddr = param->addr.biasDestAddr;
		convOutputAddr = param->addr.outputAddr;
	}

	/*set copy bias code*/
	loadw_len = (kernel_c_align*16 + 511)/512 - 1;
	//load_dst = biasDestAddr>>14;
	SET_VCODE_REG(CONV2D_CCFG2_BIAS, CCFG2(loadw_len, 1));
	SET_VCODE_REG(CONV2D_CCFG3_BIAS, CCFG3(param->addr.biasAddr, 0));
	SET_VCODE_REG(CONV2D_CCFG4_BIAS, CCFG4(param->addr.biasAddr));
	SET_VCODE_REG(CONV2D_LOADW_BIAS, LOADW(0, 0, loadw_len, 0));
	SET_VCODE_REG(CONV2D_CCFG0_BIAS, CCFG0(0, 0, 0, 0, 0, 0));
	SET_VCODE_REG(CONV2D_CCFG1_BIAS, CCFG1(1, 1, 1, 0,0,0,0,0,0,0,0));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "loadw_len: %d\r\n", loadw_len);
	iet_print(IET_PRINT_ALG_DBG, "CCFG2[0x%08x]: 0x%04x\r\n", CONV2D_CCFG2_BIAS, GET_VCODE_REG(CONV2D_CCFG2_BIAS));
	iet_print(IET_PRINT_ALG_DBG, "CCFG3[0x%08x]: 0x%04x\r\n", CONV2D_CCFG3_BIAS, GET_VCODE_REG(CONV2D_CCFG3_BIAS));
	iet_print(IET_PRINT_ALG_DBG, "CCFG4[0x%08x]: 0x%04x\r\n", CONV2D_CCFG4_BIAS, GET_VCODE_REG(CONV2D_CCFG4_BIAS));
	iet_print(IET_PRINT_ALG_DBG, "LOADW[0x%08x]: 0x%04x\r\n", CONV2D_LOADW_BIAS, GET_VCODE_REG(CONV2D_LOADW_BIAS));
	iet_print(IET_PRINT_ALG_DBG, "CCFG0[0x%08x]: 0x%04x\r\n", CONV2D_CCFG0_BIAS, GET_VCODE_REG(CONV2D_CCFG0_BIAS));
	iet_print(IET_PRINT_ALG_DBG, "CCFG1[0x%08x]: 0x%04x\r\n", CONV2D_CCFG1_BIAS, GET_VCODE_REG(CONV2D_CCFG1_BIAS));
#endif
	/*set A addr*/
	SET_VCODE_REG(CONV2D_EAA_H_BIAS, LOADH(R0, 0x4000));
	SET_VCODE_REG(CONV2D_EAA_L_BIAS, LOADL(R0, 0x4000));
	
	/*set D addr*/
	SET_VCODE_REG(CONV2D_EDA_H_BIAS, LOADH(R1, biasDestAddr));
	SET_VCODE_REG(CONV2D_EDA_L_BIAS, LOADL(R1, biasDestAddr));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "biasDestAddr: 0x%08x\r\n", biasDestAddr);
	iet_print(IET_PRINT_ALG_DBG, "EDA_H[0x%08x]: 0x%04x\r\n", CONV2D_EDA_H_BIAS, GET_VCODE_REG(CONV2D_EDA_H_BIAS));
	iet_print(IET_PRINT_ALG_DBG, "EDA_L[0x%08x]: 0x%04x\r\n", CONV2D_EDA_L_BIAS, GET_VCODE_REG(CONV2D_EDA_L_BIAS));
#endif
	/*set EAI*/
	SET_VCODE_REG(CONV2D_EAI_H_BIAS, LOADH(R4, 0));
	SET_VCODE_REG(CONV2D_EAI_L_BIAS, LOADL(R4, 0));
	
	/*set EDI*/
	copy_len = (convoutput_c_align*16+255)/256;
	SET_VCODE_REG(CONV2D_EDI_H_BIAS, LOADH(R4, copy_len));
	SET_VCODE_REG(CONV2D_EDI_L_BIAS, LOADL(R4, copy_len));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "copy_len: %d\r\n", copy_len);
	iet_print(IET_PRINT_ALG_DBG, "EDI_H[0x%08x]: 0x%04x\r\n", CONV2D_EDI_H_BIAS, GET_VCODE_REG(CONV2D_EDI_H_BIAS));
	iet_print(IET_PRINT_ALG_DBG, "EDI_L[0x%08x]: 0x%04x\r\n", CONV2D_EDI_L_BIAS, GET_VCODE_REG(CONV2D_EDI_L_BIAS));
#endif
	/*set ELP*/
	elp = param->convOutputShape.W*param->convOutputShape.H;
	SET_VCODE_REG(CONV2D_ELP_H_BIAS, LOADH(R4, elp));
	SET_VCODE_REG(CONV2D_ELP_L_BIAS, LOADL(R4, elp));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "elp: %d\r\n", elp);
	iet_print(IET_PRINT_ALG_DBG, "ELP_H[0x%08x]: 0x%04x\r\n", CONV2D_ELP_H_BIAS, GET_VCODE_REG(CONV2D_ELP_H_BIAS));
	iet_print(IET_PRINT_ALG_DBG, "ELP_L[0x%08x]: 0x%04x\r\n", CONV2D_ELP_L_BIAS, GET_VCODE_REG(CONV2D_ELP_L_BIAS));
#endif
	/*set op*/
	op_num = copy_len;
	SET_VCODE_REG(CONV2D_SCR_BIAS, EXEC(SCR, op_num));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "op_num: %d\r\n", op_num);
	iet_print(IET_PRINT_ALG_DBG, "SCR[0x%08x]: 0x%04x\r\n", CONV2D_SCR_BIAS, GET_VCODE_REG(CONV2D_SCR_BIAS));
#endif
	/*set conv ELP*/
	elp = (param->inputShape.C+15)/16;
	SET_VCODE_REG(CONV2D_ELP_H_CONV, LOADH(R4, elp));
	SET_VCODE_REG(CONV2D_ELP_L_CONV, LOADL(R4, elp));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "dotc elp: %d\r\n", elp);
	iet_print(IET_PRINT_ALG_DBG, "ELP_H[0x%08x]: 0x%04x\r\n", CONV2D_ELP_H_CONV, GET_VCODE_REG(CONV2D_ELP_H_CONV));
	iet_print(IET_PRINT_ALG_DBG, "ELP_L[0x%08x]: 0x%04x\r\n", CONV2D_ELP_L_CONV, GET_VCODE_REG(CONV2D_ELP_L_CONV));
#endif
	/*set EAI*/
	SET_VCODE_REG(CONV2D_EAI_H_CONV, LOADH(R4, 1));
	SET_VCODE_REG(CONV2D_EAI_L_CONV, LOADL(R4, 1));
	
	/*set EBI*/
	op_num = kernel_c_align/2;
	SET_VCODE_REG(CONV2D_EBI_H_CONV, LOADH(R4, op_num));
	SET_VCODE_REG(CONV2D_EBI_L_CONV, LOADL(R4, op_num));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "EBI : %d\r\n", op_num);
	iet_print(IET_PRINT_ALG_DBG, "EBI_H[0x%08x]: 0x%04x\r\n", CONV2D_EBI_H_CONV, GET_VCODE_REG(CONV2D_EBI_H_CONV));
	iet_print(IET_PRINT_ALG_DBG, "EBI_L[0x%08x]: 0x%04x\r\n", CONV2D_EBI_L_CONV, GET_VCODE_REG(CONV2D_EBI_L_CONV));
#endif
	/*set config*/
	loadw_len = (kernel_c_align*param->inputShape.C*8 + 511)/512 - 1;
	int loop = param->kernelShape.H*param->kernelShape.W;
	SET_VCODE_REG(CONV2D_CCFG0_CONV, CCFG0(op_num, 0, param->truncate[0], 0, param->use_b8, 1));
	SET_VCODE_REG(CONV2D_CCFG1_CONV, CCFG1(0, 1, 1, 0,0,0,0,0,0,0,0));
	SET_VCODE_REG(CONV2D_CCFG2_CONV, CCFG2(loadw_len, 1));
	SET_VCODE_REG(CONV2D_CCFG3_CONV, CCFG3(param->addr.kernelAddr, loop-1));
	SET_VCODE_REG(CONV2D_CCFG4_CONV, CCFG4(param->addr.kernelAddr));
	SET_VCODE_REG(CONV2D_CCFGF_CONV, CCFGF(0,0,0,0, op_num));
	SET_VCODE_REG(CONV2D_LOADW_CONV, LOADW(R15, 0, loadw_len, 1));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "loadw_len: %d, loop: %d\r\n", loadw_len, loop);
	iet_print(IET_PRINT_ALG_DBG, "CCFG0[0x%08x]: 0x%04x\r\n", CONV2D_CCFG0_CONV, GET_VCODE_REG(CONV2D_CCFG0_CONV));
	iet_print(IET_PRINT_ALG_DBG, "CCFG1[0x%08x]: 0x%04x\r\n", CONV2D_CCFG1_CONV, GET_VCODE_REG(CONV2D_CCFG1_CONV));
	iet_print(IET_PRINT_ALG_DBG, "CCFG2[0x%08x]: 0x%04x\r\n", CONV2D_CCFG2_CONV, GET_VCODE_REG(CONV2D_CCFG2_CONV));
	iet_print(IET_PRINT_ALG_DBG, "CCFG3[0x%08x]: 0x%04x\r\n", CONV2D_CCFG3_CONV, GET_VCODE_REG(CONV2D_CCFG3_CONV));
	iet_print(IET_PRINT_ALG_DBG, "CCFG4[0x%08x]: 0x%04x\r\n", CONV2D_CCFG4_CONV, GET_VCODE_REG(CONV2D_CCFG4_CONV));
	iet_print(IET_PRINT_ALG_DBG, "CCFGF[0x%08x]: 0x%04x\r\n", CONV2D_CCFGF_CONV, GET_VCODE_REG(CONV2D_CCFGF_CONV));
	iet_print(IET_PRINT_ALG_DBG, "LOADW[0x%08x]: 0x%04x\r\n", CONV2D_LOADW_CONV, GET_VCODE_REG(CONV2D_LOADW_CONV));
#endif	
	/*set EAA EBA ECA EDA*/
	copy_len = (convoutput_c_align*16+255)/256;
	SET_VCODE_REG(CONV2D_EAA_H_CONV, LOADH(R0, param->addr.inputAddr));
	SET_VCODE_REG(CONV2D_EAA_L_CONV, LOADL(R0, param->addr.inputAddr));
	SET_VCODE_REG(CONV2D_EBA_H_CONV, LOADH(R1, 0x4000));
	SET_VCODE_REG(CONV2D_EBA_L_CONV, LOADL(R1, 0x4000));
	SET_VCODE_REG(CONV2D_ECA_H_CONV, LOADH(R2, biasDestAddr));
	SET_VCODE_REG(CONV2D_ECA_L_CONV, LOADL(R2, biasDestAddr));
	SET_VCODE_REG(CONV2D_EDA_H_CONV, LOADH(R3, convOutputAddr));
	SET_VCODE_REG(CONV2D_EDA_L_CONV, LOADL(R3, convOutputAddr));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "input: 0x%08x, kernel:%04x, bias:0x%04x, convout:0x%04x\r\n",
	param->addr.inputAddr, 0x4000, biasDestAddr, convOutputAddr);
	iet_print(IET_PRINT_ALG_DBG, "EAA_H[0x%08x]: 0x%04x\r\n", CONV2D_EAA_H_CONV, GET_VCODE_REG(CONV2D_EAA_H_CONV));
	iet_print(IET_PRINT_ALG_DBG, "EAA_L[0x%08x]: 0x%04x\r\n", CONV2D_EAA_L_CONV, GET_VCODE_REG(CONV2D_EAA_L_CONV));
	iet_print(IET_PRINT_ALG_DBG, "EBA_H[0x%08x]: 0x%04x\r\n", CONV2D_EBA_H_CONV, GET_VCODE_REG(CONV2D_EBA_H_CONV));
	iet_print(IET_PRINT_ALG_DBG, "EBA_L[0x%08x]: 0x%04x\r\n", CONV2D_EBA_L_CONV, GET_VCODE_REG(CONV2D_EBA_L_CONV));
	iet_print(IET_PRINT_ALG_DBG, "ECA_H[0x%08x]: 0x%04x\r\n", CONV2D_ECA_H_CONV, GET_VCODE_REG(CONV2D_ECA_H_CONV));
	iet_print(IET_PRINT_ALG_DBG, "ECA_L[0x%08x]: 0x%04x\r\n", CONV2D_ECA_L_CONV, GET_VCODE_REG(CONV2D_ECA_L_CONV));
	iet_print(IET_PRINT_ALG_DBG, "EDA_H[0x%08x]: 0x%04x\r\n", CONV2D_EDA_H_CONV, GET_VCODE_REG(CONV2D_EDA_H_CONV));
	iet_print(IET_PRINT_ALG_DBG, "EDA_L[0x%08x]: 0x%04x\r\n", CONV2D_EDA_L_CONV, GET_VCODE_REG(CONV2D_EDA_L_CONV));
#endif
	/*set ping-pang load loop*/
	SET_VCODE_REG(CONV2D_LOADW_LOOP_L_CONV, LOADL(R15, loop));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "R15 LOOP:[0x%08x]: 0x%04x\r\n", CONV2D_LOADW_LOOP_L_CONV, GET_VCODE_REG(CONV2D_LOADW_LOOP_L_CONV));
#endif
	/*set kernel w,h for times*/
	SET_VCODE_REG(CONV2D_KERNEL_H_H_CONV, LOADH(R11, param->kernelShape.H));
	SET_VCODE_REG(CONV2D_KERNEL_H_L_CONV, LOADL(R11, param->kernelShape.H));
	SET_VCODE_REG(CONV2D_KERNEL_W_H_CONV, LOADH(R12, param->kernelShape.W));
	SET_VCODE_REG(CONV2D_KERNEL_W_L_CONV, LOADL(R12, param->kernelShape.W));

	/*set conv output w,h for times*/
	SET_VCODE_REG(CONV2D_CONVOUT_H_H_CONV, LOADH(R13, param->convOutputShape.H));
	SET_VCODE_REG(CONV2D_CONVOUT_H_L_CONV, LOADL(R13, param->convOutputShape.H));
	SET_VCODE_REG(CONV2D_CONVOUT_W_H_CONV, LOADH(R14, param->convOutputShape.W));
	SET_VCODE_REG(CONV2D_CONVOUT_W_L_CONV, LOADL(R14, param->convOutputShape.W));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "K_H: %d, K_W: %d, CO_H: %d, CO_W: %d\r\n",
	param->kernelShape.H, param->kernelShape.W, param->convOutputShape.H, param->convOutputShape.W);
	iet_print(IET_PRINT_ALG_DBG, "K_H_H[0x%08x]: 0x%04x\r\n", CONV2D_KERNEL_H_H_CONV, GET_VCODE_REG(CONV2D_KERNEL_H_H_CONV));
	iet_print(IET_PRINT_ALG_DBG, "K_H_L[0x%08x]: 0x%04x\r\n", CONV2D_KERNEL_H_L_CONV, GET_VCODE_REG(CONV2D_KERNEL_H_L_CONV));
	iet_print(IET_PRINT_ALG_DBG, "K_W_H[0x%08x]: 0x%04x\r\n", CONV2D_KERNEL_W_H_CONV, GET_VCODE_REG(CONV2D_KERNEL_W_H_CONV));
	iet_print(IET_PRINT_ALG_DBG, "K_W_L[0x%08x]: 0x%04x\r\n", CONV2D_KERNEL_W_L_CONV, GET_VCODE_REG(CONV2D_KERNEL_W_L_CONV));
	iet_print(IET_PRINT_ALG_DBG, "OC_H_H[0x%08x]: 0x%04x\r\n", CONV2D_CONVOUT_H_H_CONV, GET_VCODE_REG(CONV2D_CONVOUT_H_H_CONV));
	iet_print(IET_PRINT_ALG_DBG, "OC_H_L[0x%08x]: 0x%04x\r\n", CONV2D_CONVOUT_H_L_CONV, GET_VCODE_REG(CONV2D_CONVOUT_H_L_CONV));
	iet_print(IET_PRINT_ALG_DBG, "OC_W_H[0x%08x]: 0x%04x\r\n", CONV2D_CONVOUT_W_H_CONV, GET_VCODE_REG(CONV2D_CONVOUT_W_H_CONV));
	iet_print(IET_PRINT_ALG_DBG, "OC_W_L[0x%08x]: 0x%04x\r\n", CONV2D_CONVOUT_W_L_CONV, GET_VCODE_REG(CONV2D_CONVOUT_W_L_CONV));
#endif
	/*set op*/
	op_num = (kernel_c_align)/2;
	SET_VCODE_REG(CONV2D_DOTC_CONV, EXEC(DOTC, op_num));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "dotc op_num: %d\r\n", op_num);
	iet_print(IET_PRINT_ALG_DBG, "DOTC[0x%08x]: 0x%04x\r\n", CONV2D_DOTC_CONV, GET_VCODE_REG(CONV2D_DOTC_CONV));
#endif
	/*set EAA/ECA/EDA step*/
	int16_t step = (param->inputShape.C+15)/16;
	SET_VCODE_REG(CONV2D_EAA_STEP_CONV, INC(R4, step));
	step = (kernel_c_align+15)/16;
	SET_VCODE_REG(CONV2D_ECA_STEP_CONV, INC(R5, step));
	SET_VCODE_REG(CONV2D_EDA_STEP_CONV, INC(R6, step));
	step = ((param->strides[0] - 1)*inputShape.W +
								(inputShape.W - (param->convOutputShape.W-1)*param->strides[1] - 
								param->strides[0])) * ((inputShape.C+15) / 16);
	SET_VCODE_REG(CONV2D_INPUT_OW_STEP_CONV, INC(R4, step));
	step = ((inputShape.C+15)/16);
	SET_VCODE_REG(CONV2D_INPUT_KW_STEP_CONV, INC(R0, step));
	step = (inputShape.W - param->kernelShape.W)*((inputShape.C+15)/16);
	int16_t step_p = step > 127 ? 127 : step;
	step -= step_p;
	SET_VCODE_REG(CONV2D_INPUT_KH0_STEP_CONV, INC(R0, step_p));
	step_p = step > 127 ? 127 : step;
	step -= step_p;
	SET_VCODE_REG(CONV2D_INPUT_KH1_STEP_CONV, INC(R0, step_p));
	step_p = step > 127 ? 127 : step;
	step -= step_p;
	SET_VCODE_REG(CONV2D_INPUT_KH2_STEP_CONV, INC(R0, step_p));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "EAA_STEP[0x%08x]: 0x%04x\r\n", CONV2D_EAA_STEP_CONV, GET_VCODE_REG(CONV2D_EAA_STEP_CONV));
	iet_print(IET_PRINT_ALG_DBG, "ECA_STEP[0x%08x]: 0x%04x\r\n", CONV2D_ECA_STEP_CONV, GET_VCODE_REG(CONV2D_ECA_STEP_CONV));
	iet_print(IET_PRINT_ALG_DBG, "EDA_STEP[0x%08x]: 0x%04x\r\n", CONV2D_EDA_STEP_CONV, GET_VCODE_REG(CONV2D_EDA_STEP_CONV));
	iet_print(IET_PRINT_ALG_DBG, "Input ow step[0x%08x]: 0x%04x\r\n", CONV2D_INPUT_OW_STEP_CONV, GET_VCODE_REG(CONV2D_INPUT_OW_STEP_CONV));
	iet_print(IET_PRINT_ALG_DBG, "INPUT_KW[0x%08x]: 0x%04x\r\n", CONV2D_INPUT_KW_STEP_CONV, GET_VCODE_REG(CONV2D_INPUT_KW_STEP_CONV));
	iet_print(IET_PRINT_ALG_DBG, "INPUT_KH0[0x%08x]: 0x%04x\r\n", CONV2D_INPUT_KH0_STEP_CONV, GET_VCODE_REG(CONV2D_INPUT_KH0_STEP_CONV));
	iet_print(IET_PRINT_ALG_DBG, "INPUT_KH1[0x%08x]: 0x%04x\r\n", CONV2D_INPUT_KH1_STEP_CONV, GET_VCODE_REG(CONV2D_INPUT_KH1_STEP_CONV));
	iet_print(IET_PRINT_ALG_DBG, "INPUT_KH2[0x%08x]: 0x%04x\r\n", CONV2D_INPUT_KH2_STEP_CONV, GET_VCODE_REG(CONV2D_INPUT_KH2_STEP_CONV));
#endif
	/*set load2 scale and vector*/
	loadw_len = (kernel_c_align*16 + 511)/512 - 1;
	SET_VCODE_REG(CONV2D_CCFG2_SCALE, CCFG2(loadw_len, 1));
	SET_VCODE_REG(CONV2D_CCFG3_SCALE, CCFG3(param->addr.scaleAddr, 0));
	SET_VCODE_REG(CONV2D_CCFG4_SCALE, CCFG4(param->addr.scaleAddr));
	SET_VCODE_REG(CONV2D_CCFGF_SCALE, CCFGF(0,0,0,0, op_num));
	SET_VCODE_REG(CONV2D_LOADW_SCALE, LOADW(R0, 0, loadw_len, 0));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "scale :loadw_len: %d\r\n", loadw_len);
	iet_print(IET_PRINT_ALG_DBG, "CCFG2[0x%08x]: 0x%04x\r\n", CONV2D_CCFG2_SCALE, GET_VCODE_REG(CONV2D_CCFG2_SCALE));
	iet_print(IET_PRINT_ALG_DBG, "CCFG3[0x%08x]: 0x%04x\r\n", CONV2D_CCFG3_SCALE, GET_VCODE_REG(CONV2D_CCFG3_SCALE));
	iet_print(IET_PRINT_ALG_DBG, "CCFG4[0x%08x]: 0x%04x\r\n", CONV2D_CCFG4_SCALE, GET_VCODE_REG(CONV2D_CCFG4_SCALE));
	iet_print(IET_PRINT_ALG_DBG, "LOADW[0x%08x]: 0x%04x\r\n", CONV2D_LOADW_SCALE, GET_VCODE_REG(CONV2D_LOADW_SCALE));
#endif
	load_dst = param->addr.inputAddr>>14;
	SET_VCODE_REG(CONV2D_CCFG2_VECTOR, CCFG2(loadw_len, load_dst));
	SET_VCODE_REG(CONV2D_CCFG3_VECTOR, CCFG3(param->addr.vectorAddr, 0));
	SET_VCODE_REG(CONV2D_CCFG4_VECTOR, CCFG4(param->addr.vectorAddr));
	SET_VCODE_REG(CONV2D_CCFGF_VECTOR, CCFGF(0,0,0,0, op_num));
	SET_VCODE_REG(CONV2D_LOADW_VECTOR, LOADW(R0, 0, loadw_len, 0));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "vector :loadw_len: %d, load_dst: %d\r\n", loadw_len, load_dst);
	iet_print(IET_PRINT_ALG_DBG, "CCFG2[0x%08x]: 0x%04x\r\n", CONV2D_CCFG2_VECTOR, GET_VCODE_REG(CONV2D_CCFG2_VECTOR));
	iet_print(IET_PRINT_ALG_DBG, "CCFG3[0x%08x]: 0x%04x\r\n", CONV2D_CCFG3_VECTOR, GET_VCODE_REG(CONV2D_CCFG3_VECTOR));
	iet_print(IET_PRINT_ALG_DBG, "CCFG4[0x%08x]: 0x%04x\r\n", CONV2D_CCFG4_VECTOR, GET_VCODE_REG(CONV2D_CCFG4_VECTOR));
	iet_print(IET_PRINT_ALG_DBG, "LOADW[0x%08x]: 0x%04x\r\n", CONV2D_LOADW_VECTOR, GET_VCODE_REG(CONV2D_LOADW_VECTOR));
#endif

	/*set mulc ccfg0 ccfg1*/
	op_num = (convoutput_c_align*16 + 255)/256;
	int16_t relu = param->activation == RELU ? 1 : 0;
	SET_VCODE_REG(CONV2D_CCFG0_MULC, CCFG0(op_num, 0, param->truncate[1], 0, 0, 0));
	SET_VCODE_REG(CONV2D_CCFG1_MULC, CCFG1(1, 1, 1, 0,relu,0,0,0,0,0,0));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "mulc :op_num: %d, relu: %d\r\n", op_num, relu);
	iet_print(IET_PRINT_ALG_DBG, "CCFG0[0x%08x]: 0x%04x\r\n", CONV2D_CCFG0_MULC, GET_VCODE_REG(CONV2D_CCFG0_MULC));
	iet_print(IET_PRINT_ALG_DBG, "CCFG1[0x%08x]: 0x%04x\r\n", CONV2D_CCFG1_MULC, GET_VCODE_REG(CONV2D_CCFG1_MULC));
#endif
	/*set EAA EBA ECA EDA*/
	copy_len = (convoutput_c_align*16+255)/256;
	SET_VCODE_REG(CONV2D_EAA_H_MULC, LOADH(R0, 0x4000));
	SET_VCODE_REG(CONV2D_EAA_L_MULC, LOADL(R0, 0x4000));
	SET_VCODE_REG(CONV2D_EBA_H_MULC, LOADH(R1, param->addr.biasDestAddr));
	SET_VCODE_REG(CONV2D_EBA_L_MULC, LOADL(R1, param->addr.biasDestAddr));
	SET_VCODE_REG(CONV2D_ECA_H_MULC, LOADH(R2, param->addr.inputAddr));
	SET_VCODE_REG(CONV2D_ECA_L_MULC, LOADL(R2, param->addr.inputAddr));
	SET_VCODE_REG(CONV2D_EDA_H_MULC, LOADH(R3, param->addr.outputAddr));
	SET_VCODE_REG(CONV2D_EDA_L_MULC, LOADL(R3, param->addr.outputAddr));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "input: 0x%04x, scale:%04x, vector:0x%04x, out:0x%04x\r\n",
	param->addr.biasDestAddr, 0x4000, param->addr.inputAddr, param->addr.outputAddr);
	iet_print(IET_PRINT_ALG_DBG, "EAA_H[0x%08x]: 0x%04x\r\n", CONV2D_EAA_H_MULC, GET_VCODE_REG(CONV2D_EAA_H_MULC));
	iet_print(IET_PRINT_ALG_DBG, "EAA_L[0x%08x]: 0x%04x\r\n", CONV2D_EAA_L_MULC, GET_VCODE_REG(CONV2D_EAA_L_MULC));
	iet_print(IET_PRINT_ALG_DBG, "EBA_H[0x%08x]: 0x%04x\r\n", CONV2D_EBA_H_MULC, GET_VCODE_REG(CONV2D_EBA_H_MULC));
	iet_print(IET_PRINT_ALG_DBG, "EBA_L[0x%08x]: 0x%04x\r\n", CONV2D_EBA_L_MULC, GET_VCODE_REG(CONV2D_EBA_L_MULC));
	iet_print(IET_PRINT_ALG_DBG, "ECA_H[0x%08x]: 0x%04x\r\n", CONV2D_ECA_H_MULC, GET_VCODE_REG(CONV2D_ECA_H_MULC));
	iet_print(IET_PRINT_ALG_DBG, "ECA_L[0x%08x]: 0x%04x\r\n", CONV2D_ECA_L_MULC, GET_VCODE_REG(CONV2D_ECA_L_MULC));
	iet_print(IET_PRINT_ALG_DBG, "EDA_H[0x%08x]: 0x%04x\r\n", CONV2D_EDA_H_MULC, GET_VCODE_REG(CONV2D_EDA_H_MULC));
	iet_print(IET_PRINT_ALG_DBG, "EDA_L[0x%08x]: 0x%04x\r\n", CONV2D_EDA_L_MULC, GET_VCODE_REG(CONV2D_EDA_L_MULC));
#endif
	/*mulc elp*/
	elp = param->outputShape.H*param->outputShape.W;
	SET_VCODE_REG(CONV2D_ELP_H_MULC, LOADH(R4, elp));
	SET_VCODE_REG(CONV2D_ELP_L_MULC, LOADL(R4, elp));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "mulc :elp: %d\r\n", elp);
	iet_print(IET_PRINT_ALG_DBG, "ELP_H[0x%08x]: 0x%04x\r\n", CONV2D_ELP_H_MULC, GET_VCODE_REG(CONV2D_ELP_H_MULC));
	iet_print(IET_PRINT_ALG_DBG, "ELP_L[0x%08x]: 0x%04x\r\n", CONV2D_ELP_L_MULC, GET_VCODE_REG(CONV2D_ELP_L_MULC));
#endif
	/*set EBI EDI*/
	SET_VCODE_REG(CONV2D_EBDI_H_MULC, LOADH(R4, op_num));
	SET_VCODE_REG(CONV2D_EBDI_L_MULC, LOADL(R4, op_num));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "EBDI_H[0x%08x]: 0x%04x\r\n", CONV2D_EBDI_H_MULC, GET_VCODE_REG(CONV2D_EBDI_H_MULC));
	iet_print(IET_PRINT_ALG_DBG, "EBDI_H[0x%08x]: 0x%04x\r\n", CONV2D_EBDI_L_MULC, GET_VCODE_REG(CONV2D_EBDI_L_MULC));
#endif
	/*set mulc op_num*/
	SET_VCODE_REG(CONV2D_MULC_MULC, EXEC(MULC, op_num));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "MULC_MULC[0x%08x]: 0x%04x\r\n", CONV2D_MULC_MULC, GET_VCODE_REG(CONV2D_MULC_MULC));	
#endif
#endif
	VpuStart(start_addr);
#endif
	return 0;
}

int nn_conv(const LayerParam_t *param)
{
	//if need padding , just do it
	int16_t padding = param->padding[0][0] + 
						  param->padding[0][1] +
						  param->padding[1][0] +
						  param->padding[1][1];
	if(padding != 0)
	{	
		uint8_t (*paddingSize)[2] = param->padding;
		nn_padding(param->addr.inputAddr, param->addr.outputAddr, param->addr.biasAddr, param->addr.biasDestAddr, paddingSize, &param->inputShape);		
	}
	int Ret = 0;
	if(param->model_q == 1 || param->inputShape.C <= 16)
	{
		Ret = Conv2D_Q_new(param);
	}
	else
	{
		Ret = Conv2D_Common_new(param);
	}
	
	return Ret;
}
