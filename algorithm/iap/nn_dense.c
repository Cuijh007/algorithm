#include "nn_file_struct.h"
#include "nn_vpu_control.h"
#include "nn_dense.h"

#define CONV_VCODE_DEBUG 0

int nn_dense(const LayerParam_t *param)
{
#ifndef ALGORITHM_RELEASE
	unsigned short start_addr = 0x16f;
	uint8_t strides[2] = {1,1};
	int loadw_len = 0;
	int copy_len = 0;
	int op_num = 0;
	int elp = 0;
	int load_dst;
	uint16_t biasDestAddr, convOutputAddr;

	biasDestAddr = param->addr.outputAddr;
	convOutputAddr = param->addr.biasDestAddr;
	int kernel_c_align = (param->kernelShape.C+15)/16*16;
	int convoutput_c_align = (param->outputShape.C+15)/16*16;

	//iet_print(IET_PRINT_ALG_DBG, "dense \r\n");

	/*set copy bias code*/
	loadw_len = (kernel_c_align*16 + 511)/512 - 1;
	load_dst = biasDestAddr>>14;
	SET_VCODE_REG(DENSE_CCFG2_BIAS, CCFG2(loadw_len, load_dst));
	SET_VCODE_REG(DENSE_CCFG3_BIAS, CCFG3(param->addr.biasAddr, 0));
	SET_VCODE_REG(DENSE_CCFG4_BIAS, CCFG4(param->addr.biasAddr));
	SET_VCODE_REG(DENSE_LOADW_BIAS, LOADW(0, 0, loadw_len, 0));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "loadw_len: %d, load_dst: %d\r\n", loadw_len, load_dst);
	iet_print(IET_PRINT_ALG_DBG, "CCFG2[0x%08x]: 0x%04x\r\n", DENSE_CCFG2_BIAS, GET_VCODE_REG(DENSE_CCFG2_BIAS));
	iet_print(IET_PRINT_ALG_DBG, "CCFG3[0x%08x]: 0x%04x\r\n", DENSE_CCFG3_BIAS, GET_VCODE_REG(DENSE_CCFG3_BIAS));
	iet_print(IET_PRINT_ALG_DBG, "CCFG4[0x%08x]: 0x%04x\r\n", DENSE_CCFG4_BIAS, GET_VCODE_REG(DENSE_CCFG4_BIAS));
	iet_print(IET_PRINT_ALG_DBG, "LOADW[0x%08x]: 0x%04x\r\n", DENSE_LOADW_BIAS, GET_VCODE_REG(DENSE_LOADW_BIAS));	
#endif
	
	/*set dense ELP*/
    elp = (param->inputShape.C+15)/16;
    SET_VCODE_REG(DENSE_ELP_H_DENSE, LOADH(R4, elp));
    SET_VCODE_REG(DENSE_ELP_L_DENSE, LOADL(R4, elp));    
    
    /*set EBI*/
    copy_len = (kernel_c_align*16*8+255)/256;
    SET_VCODE_REG(DENSE_EBI_H_DENSE, LOADH(R4, copy_len));
    SET_VCODE_REG(DENSE_EBI_L_DENSE, LOADL(R4, copy_len));

    /*set dense config*/
	loadw_len = (kernel_c_align*param->inputShape.C*8 + 511)/512 - 1;
	int loop = 1;
	op_num = convoutput_c_align/2;
	SET_VCODE_REG(DENSE_CCFG0_DENSE, CCFG0(op_num, 0, param->truncate[0], 0, param->use_b8, 1));
	SET_VCODE_REG(DENSE_CCFG1_DENSE, CCFG1(0, 1, 1, 0,0,0,0,0,0,0,0));
	SET_VCODE_REG(DENSE_CCFG2_DENSE, CCFG2(loadw_len, 1));
	SET_VCODE_REG(DENSE_CCFG3_DENSE, CCFG3(param->addr.kernelAddr, loop-1));
	SET_VCODE_REG(DENSE_CCFG4_DENSE, CCFG4(param->addr.kernelAddr));
	//SET_VCODE_REG(CONV2D_CCFGF_CONV, CCFGF(0,0,0,0, op_num));
	SET_VCODE_REG(DENSE_LOADW_DENSE, LOADW(R0, 0, loadw_len, 0));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "kernel : 0x%04x\r\n", param->addr.kernelAddr);
	iet_print(IET_PRINT_ALG_DBG, "loadw_len: %d, loop: %d\r\n", loadw_len, loop);
	iet_print(IET_PRINT_ALG_DBG, "CCFG0[0x%08x]: 0x%04x\r\n", DENSE_CCFG0_DENSE, GET_VCODE_REG(DENSE_CCFG0_DENSE));
	iet_print(IET_PRINT_ALG_DBG, "CCFG1[0x%08x]: 0x%04x\r\n", DENSE_CCFG1_DENSE, GET_VCODE_REG(DENSE_CCFG1_DENSE));
	iet_print(IET_PRINT_ALG_DBG, "CCFG2[0x%08x]: 0x%04x\r\n", DENSE_CCFG2_DENSE, GET_VCODE_REG(DENSE_CCFG2_DENSE));
	iet_print(IET_PRINT_ALG_DBG, "CCFG3[0x%08x]: 0x%04x\r\n", DENSE_CCFG3_DENSE, GET_VCODE_REG(DENSE_CCFG3_DENSE));
	iet_print(IET_PRINT_ALG_DBG, "CCFG4[0x%08x]: 0x%04x\r\n", DENSE_CCFG4_DENSE, GET_VCODE_REG(DENSE_CCFG4_DENSE));
	//iet_print(IET_PRINT_ALG_DBG, "CCFGF[0x%08x]: 0x%04x\r\n", CONV2D_CCFGF_CONV, GET_VCODE_REG(CONV2D_CCFGF_CONV));
	iet_print(IET_PRINT_ALG_DBG, "LOADW[0x%08x]: 0x%04x\r\n", DENSE_LOADW_DENSE, GET_VCODE_REG(DENSE_LOADW_DENSE));
	iet_print(IET_PRINT_ALG_DBG, "input: 0x%08x, kernel:%04x, bias:0x%04x, convout:0x%04x\r\n",
			param->addr.inputAddr, 0x4000, biasDestAddr, convOutputAddr);

#endif
	/*set EDA/EBA/ECA/EDA*/
	SET_VCODE_REG(DENSE_EAA_H_DENSE, LOADH(R0, param->addr.inputAddr));
	SET_VCODE_REG(DENSE_EAA_L_DENSE, LOADL(R0, param->addr.inputAddr));
	SET_VCODE_REG(DENSE_EBA_H_DENSE, LOADH(R1, 0x4000));
	SET_VCODE_REG(DENSE_EBA_L_DENSE, LOADL(R1, 0x4000));
	SET_VCODE_REG(DENSE_ECA_H_DENSE, LOADH(R2, biasDestAddr));
	SET_VCODE_REG(DENSE_ECA_L_DENSE, LOADL(R2, biasDestAddr));
	SET_VCODE_REG(DENSE_EDA_H_DENSE, LOADH(R3, convOutputAddr));
	SET_VCODE_REG(DENSE_EDA_L_DENSE, LOADL(R3, convOutputAddr));

	/*set op*/
	SET_VCODE_REG(DENSE_DOTC_DENSE, EXEC(DOTC, op_num));

	/*set load2 scale and vector*/
	loadw_len = (kernel_c_align*16 + 511)/512 - 1;
	SET_VCODE_REG(DENSE_CCFG2_SCALE, CCFG2(loadw_len, 1));
	SET_VCODE_REG(DENSE_CCFG3_SCALE, CCFG3(param->addr.scaleAddr, 0));
	SET_VCODE_REG(DENSE_CCFG4_SCALE, CCFG4(param->addr.scaleAddr));
	SET_VCODE_REG(DENSE_LOADW_SCALE, LOADW(R0, 0, loadw_len, 0));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "scale :loadw_len: %d\r\n", loadw_len);
	iet_print(IET_PRINT_ALG_DBG, "CCFG2[0x%08x]: 0x%04x\r\n", DENSE_CCFG2_SCALE, GET_VCODE_REG(DENSE_CCFG2_SCALE));
	iet_print(IET_PRINT_ALG_DBG, "CCFG3[0x%08x]: 0x%04x\r\n", DENSE_CCFG3_SCALE, GET_VCODE_REG(DENSE_CCFG3_SCALE));
	iet_print(IET_PRINT_ALG_DBG, "CCFG4[0x%08x]: 0x%04x\r\n", DENSE_CCFG4_SCALE, GET_VCODE_REG(DENSE_CCFG4_SCALE));
	iet_print(IET_PRINT_ALG_DBG, "LOADW[0x%08x]: 0x%04x\r\n", DENSE_LOADW_SCALE, GET_VCODE_REG(DENSE_LOADW_SCALE));
#endif
	load_dst = param->addr.inputAddr>>14;
	if(param->use_v == 0){
		loadw_len = 0;
	}
	SET_VCODE_REG(DENSE_CCFG2_VECTOR, CCFG2(loadw_len, load_dst));
	SET_VCODE_REG(DENSE_CCFG3_VECTOR, CCFG3(param->addr.vectorAddr, 0));
	SET_VCODE_REG(DENSE_CCFG4_VECTOR, CCFG4(param->addr.vectorAddr));
	SET_VCODE_REG(DENSE_LOADW_VECTOR, LOADW(R0, 0, loadw_len, 0));
#if CONV_VCODE_DEBUG
	iet_print(IET_PRINT_ALG_DBG, "vector :loadw_len: %d, load_dst: %d\r\n", loadw_len, load_dst);
	iet_print(IET_PRINT_ALG_DBG, "CCFG2[0x%08x]: 0x%04x\r\n", DENSE_CCFG2_VECTOR, GET_VCODE_REG(DENSE_CCFG2_VECTOR));
	iet_print(IET_PRINT_ALG_DBG, "CCFG3[0x%08x]: 0x%04x\r\n", DENSE_CCFG3_VECTOR, GET_VCODE_REG(DENSE_CCFG3_VECTOR));
	iet_print(IET_PRINT_ALG_DBG, "CCFG4[0x%08x]: 0x%04x\r\n", DENSE_CCFG4_VECTOR, GET_VCODE_REG(DENSE_CCFG4_VECTOR));
	iet_print(IET_PRINT_ALG_DBG, "LOADW[0x%08x]: 0x%04x\r\n", DENSE_LOADW_VECTOR, GET_VCODE_REG(DENSE_LOADW_VECTOR));
#endif

	/*set mulc config*/
	op_num = (convoutput_c_align+15)/16;
	int16_t relu = param->activation == RELU ? 1 : 0;
	SET_VCODE_REG(DENSE_CCFG0_MULC, CCFG0(op_num, 0, param->truncate[1], 0, 0, 0));
	SET_VCODE_REG(DENSE_CCFG1_MULC, CCFG1(1, 1, 1, 0,relu,0,0,0,0,0,0));

	/*set EDA/EBA/ECA/EDA*/
	SET_VCODE_REG(DENSE_EAA_H_MULC, LOADH(R0, convOutputAddr));
	SET_VCODE_REG(DENSE_EAA_L_MULC, LOADL(R0, convOutputAddr));
	SET_VCODE_REG(DENSE_EBA_H_MULC, LOADH(R1, 0x4000));
	SET_VCODE_REG(DENSE_EBA_L_MULC, LOADL(R1, 0x4000));
	SET_VCODE_REG(DENSE_ECA_H_MULC, LOADH(R2, param->addr.inputAddr));
	SET_VCODE_REG(DENSE_ECA_L_MULC, LOADL(R2, param->addr.inputAddr));
	SET_VCODE_REG(DENSE_EDA_H_MULC, LOADH(R3, param->addr.outputAddr));
	SET_VCODE_REG(DENSE_EDA_L_MULC, LOADL(R3, param->addr.outputAddr));

	/*set mulc ELP*/
    elp = 1;
    SET_VCODE_REG(DENSE_ELP_H_MULC, LOADH(R4, elp));
    SET_VCODE_REG(DENSE_ELP_L_MULC, LOADL(R4, elp));    
    
    /*set EBDI*/
    copy_len = 0;
    SET_VCODE_REG(DENSE_EBDI_H_MULC, LOADH(R4, copy_len));
    SET_VCODE_REG(DENSE_EBDI_L_MULC, LOADL(R4, copy_len));

    /*set op*/
    if(param->use_v)
		SET_VCODE_REG(DENSE_MULC_MULC, EXEC(MULC, op_num));
	else
		SET_VCODE_REG(DENSE_MULC_MULC, EXEC(MUL, op_num));

	VpuStart(start_addr);
#endif	
	return 0;
}
