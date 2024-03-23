#include "nn_bn.h"

#if 1
int nn_bn(const LayerParam_t *param, void *pBase){
#ifndef ALGORITHM_RELEASE
	unsigned short start_addr = 0x202;
	int loadw_len = 0;
	int copy_len = 0;
	int op_num = 0;
	int elp = 0;
	int load_dst;
	
	int ExOutputC = (param->inputShape.C + 15)/16 * 16;
	int8_t axis = param->axis;
	//iet_print(IET_PRINT_ALG_DBG, "axix : %d\r\n", axis);
	if(axis == -1 || axis == 3){
		loadw_len = (ExOutputC*16 + 511)/512-1;
	}else if(axis == 2){
		loadw_len = (param->inputShape.W*ExOutputC*16 + 511)/512-1;
	}

	/*set copy scale  vector*/
	SET_VCODE_REG(BN_CCFG2_SCALE, CCFG2(loadw_len, 1));
	SET_VCODE_REG(BN_CCFG3_SCALE, CCFG3(param->addr.scaleAddr, 0));
	SET_VCODE_REG(BN_CCFG4_SCALE, CCFG4(param->addr.scaleAddr));
	SET_VCODE_REG(BN_LOADW_SCALE, LOADW(0, 0, loadw_len, 0));

	load_dst = param->addr.biasDestAddr>>14;
	//iet_print(IET_PRINT_ALG_DBG, "load_dst: %d\r\n", load_dst);
	SET_VCODE_REG(BN_CCFG2_VECTOR, CCFG2(loadw_len, load_dst));
	SET_VCODE_REG(BN_CCFG3_VECTOR, CCFG3(param->addr.vectorAddr, 0));
	SET_VCODE_REG(BN_CCFG4_VECTOR, CCFG4(param->addr.vectorAddr));
	SET_VCODE_REG(BN_LOADW_VECTOR, LOADW(0, 0, loadw_len, 0));
	
	/*set bn ELP EAI EBI ECI EDI*/
    elp = param->inputShape.W;
	copy_len = (ExOutputC*16+255)/256;
    SET_VCODE_REG(BN_MULC_ELP_H, LOADH(R15, elp));
    SET_VCODE_REG(BN_MULC_ELP_L, LOADL(R15, elp));
	SET_VCODE_REG(BN_MULC_EAI_H, LOADH(R15, copy_len));
    SET_VCODE_REG(BN_MULC_EAI_L, LOADL(R15, copy_len));
	if(axis == -1 || axis == 3){
		SET_VCODE_REG(BN_MULC_EBI_H, LOADH(R15, 0));
		SET_VCODE_REG(BN_MULC_EBI_L, LOADL(R15, 0));    
		SET_VCODE_REG(BN_MULC_ECI_H, LOADH(R15, 0));
		SET_VCODE_REG(BN_MULC_ECI_L, LOADL(R15, 0));    
	}else if(axis == 2){
		SET_VCODE_REG(BN_MULC_EBI_H, LOADH(R15, copy_len));
		SET_VCODE_REG(BN_MULC_EBI_L, LOADL(R15, copy_len));    
		SET_VCODE_REG(BN_MULC_ECI_H, LOADH(R15, copy_len));
		SET_VCODE_REG(BN_MULC_ECI_L, LOADL(R15, copy_len));    
	}
	SET_VCODE_REG(BN_MULC_EDI_H, LOADH(R15, copy_len));
    SET_VCODE_REG(BN_MULC_EDI_L, LOADL(R15, copy_len));   

	SET_VCODE_REG(BN_EAA_H_MULC, LOADH(R0, param->addr.inputAddr));
    SET_VCODE_REG(BN_EAA_L_MULC, LOADL(R0, param->addr.inputAddr)); 
	SET_VCODE_REG(BN_EBA_H_MULC, LOADH(R1, 0x4000));
    SET_VCODE_REG(BN_EBA_L_MULC, LOADL(R1, 0x4000)); 
	SET_VCODE_REG(BN_ECA_H_MULC, LOADH(R2, param->addr.biasDestAddr));
    SET_VCODE_REG(BN_ECA_L_MULC, LOADL(R2, param->addr.biasDestAddr)); 
	SET_VCODE_REG(BN_EDA_H_MULC, LOADH(R3, param->addr.outputAddr));
    SET_VCODE_REG(BN_EDA_L_MULC, LOADL(R3, param->addr.outputAddr)); 

    /*set BN config*/
	op_num = copy_len;
	SET_VCODE_REG(BN_MULC_CCFG0, CCFG0(op_num, 0, param->truncate[1], 0, 0, 0));
	SET_VCODE_REG(BN_MULC_CCFG1, CCFG1(1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0));
	SET_VCODE_REG(BN_MULC_NUM, EXEC(MULC, op_num));

	VpuStart(start_addr);
#endif
	return 0;
}
#else
int nn_bn(const LayerParam_t *param, void *pBase)
{
#ifndef ALGORITHM_RELEASE
	const MatrixShape_t *inputShape  = &param->inputShape;
	const MatrixShape_t *outputShape = &param->outputShape;

	uint32_t *inputAddr 		 = (uint32_t *)(pBase + param->addr.inputAddr*32);
	uint32_t *scaleAddr 		 = (uint32_t *)(pBase + param->addr.scaleAddr*32);
	uint32_t *vectorAddr 	 	 = (uint32_t *)(pBase + param->addr.vectorAddr*32);
	uint32_t *outputAddr		 = (uint32_t *)(pBase + param->addr.outputAddr*32);
	
	const uint8_t *truncate = param->truncate;
	int ExOutputC = (inputShape->C + 15)/16 * 16;
	int16_t axis = param->axis;
	
	if(axis == -1 || axis == 3){
		for(int R12 = 0; R12 < outputShape->H*outputShape->W; R12++){
			for(int R13 = 0; R13 < ExOutputC; R13+=2){
				int index = (R12*ExOutputC + R13)/2;
				if(R13 >= inputShape->C){
					outputAddr[index] = 0;
					continue;
				}
				uint32_t value = inputAddr[index];
				int16_t low_value = (int16_t)(value&0x0000FFFF);
				int16_t high_value = (int16_t)((value>>16)&0x0000FFFF);
				int16_t scale_l = (int16_t)(scaleAddr[R13/2]&0x0000FFFF);
				int16_t scale_h = (int16_t)((scaleAddr[R13/2]>>16)&0x0000FFFF);
				int16_t vector_l = (int16_t)(vectorAddr[R13/2]&0x0000FFFF);
				int16_t vector_h = (int16_t)((vectorAddr[R13/2]>>16)&0x0000FFFF);
				
				int32_t result_h = ((int32_t)(high_value * scale_h) >> truncate[1]) + vector_h;
				int32_t result_l = ((int32_t)(low_value * scale_l) >> truncate[1]) + vector_l;
				if((R13 + 1) == outputShape->C){
					int16_t tmp_result_l = (int16_t)SAT_INT16T(result_l);
					outputAddr[index] = (int32_t)(tmp_result_l&0x0000FFFF);
				}else{
					int16_t tmp_result_h = (int16_t)SAT_INT16T(result_h);
					int16_t tmp_result_l = (int16_t)SAT_INT16T(result_l);					
					outputAddr[index] = (int32_t)(tmp_result_l&0x0000FFFF)|((int32_t)(tmp_result_h&0x0000FFFF)<<16);
				}
			}
		}
	}else if(axis == 2){
		for(int R12 = 0; R12 < outputShape->H; R12++){
			for(int R13 = 0; R13 < outputShape->W; R13++){
				int16_t scale_l = (int16_t)(scaleAddr[R13/2]&0x0000FFFF);
				int16_t scale_h = (int16_t)((scaleAddr[R13/2]>>16)&0x0000FFFF);
				int16_t vector_l = (int16_t)(vectorAddr[R13/2]&0x0000FFFF);
				int16_t vector_h = (int16_t)((vectorAddr[R13/2]>>16)&0x0000FFFF);
				int16_t scale = (R13%2) == 0 ? scale_l : scale_h;
				int16_t vector = (R13%2) == 0 ? vector_l : vector_h;

				int offset = R12*outputShape->W*ExOutputC + R13*ExOutputC;
				for(int R14 = 0; R14 < ExOutputC; R14+=2){
					int index = (offset + R14)/2;
					if(R14 >= inputShape->C){
						outputAddr[index] = 0;
						continue;
					}
					uint32_t value = inputAddr[index];
					int16_t low_value = (int16_t)(value&0x0000FFFF);
					int16_t high_value = (int16_t)((value>>16)&0x0000FFFF);					

					int32_t result_h = ((int32_t)(high_value * scale) >> truncate[1]) + vector;
					int32_t result_l = ((int32_t)(low_value * scale) >> truncate[1]) + vector;
					if((R14 + 1) == outputShape->C){
						int16_t tmp_result_l = (int16_t)SAT_INT16T(result_l);
						outputAddr[index] = (uint16_t)(tmp_result_l&0x0000FFFF);
					}else{
						int16_t tmp_result_h = (int16_t)SAT_INT16T(result_h);
						int16_t tmp_result_l = (int16_t)SAT_INT16T(result_l);					
						outputAddr[index] = (int32_t)(tmp_result_l&0x0000FFFF)|((int32_t)(tmp_result_h&0x0000FFFF)<<16);
					}					
				}
			}
		}
	}
#endif	
	return 0;
}
#endif