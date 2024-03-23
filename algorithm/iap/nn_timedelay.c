#include "nn_timedelay.h"
#include "nn_vpu_control.h"

extern int32_t vcode_offset;
int nn_timedelay_vpu(const LayerParam_t *param)
{
	TimeDelayPrivateData_t *p_private_data = (TimeDelayPrivateData_t *)param->userDefine;
	uint32_t vpucode_addr = (uint32_t)(VPU_CODE_ADDR+((param->vpuPara.vpu_end_addr - vcode_offset)*sizeof(uint32_t)));

	if(param->timesType == INTERVAL_MODE)
	{
		p_private_data->g_stride_now++;
		if(p_private_data->g_stride_now == param->strides[0]){
			p_private_data->g_stride_now = 0;
			// Set R12 = 0 to skip the running of the current layer for this frame,
			// used for A101&D101&D102
			SET_VCODE_REG(vpucode_addr, LOADL(R12, 0));
		}else{
			SET_VCODE_REG(vpucode_addr, LOADL(R12, 1));
		}
	}

	if(param->strides[0] != 1 && param->timesType == NORMAL_MODE){
		p_private_data->g_stride_now++;
		if(p_private_data->g_stride_now == param->strides[0]){
			p_private_data->g_stride_now = 0;
			SET_VCODE_REG(vpucode_addr, LOADL(R11, 1));			
		}else{
			// Set R11 = 0 to run the EXIT cmd after buffering the current frame
			// used for A102&D101&D102
			SET_VCODE_REG(vpucode_addr, LOADL(R11, 0));
			return 1;
		}
	}

	if(param->timesType == EXTRACT_MODE) {
		// do nothing, support param->strides[0]=1 only now
	}

	return 0;	
}

int nn_timedelay(const LayerParam_t *param, void *pBase)
{
#ifndef ALGORITHM_RELEASE
	//iet_print(IET_PRINT_ALG_DBG, "this is timedelay\r\n");
	MatrixShape_t *inputShape = &param->inputShape;
	MatrixShape_t *outputShape = &param->outputShape;
	int16_t timeDelaySize = param->timesize;
	int16_t ExtenOutputC = ALIGN_16(inputShape->C );
	int32_t oneDataSize = (inputShape->W * inputShape->H * ExtenOutputC*16 + 255)/256;	
	int8_t totalTimeSize = param->timesize;//0 - param->times[0] + 1;
	uint16_t start_addr = 0x1c9;
	TimeDelayPrivateData_t *p_private_data = (TimeDelayPrivateData_t *)param->userDefine;

	/*copy timedelay to output*/
	int16_t op_num = ExtenOutputC/16;
	int16_t elp = inputShape->W * inputShape->H*(timeDelaySize-1);
	int32_t beginAddr = param->addr.saveResultAddr + oneDataSize;
	SET_VCODE_REG(TIMED_ELP_H_COPY0, LOADH(R4, elp));
	SET_VCODE_REG(TIMED_ELP_L_COPY0, LOADL(R4, elp));
	SET_VCODE_REG(TIMED_EADI_H_COPY0, LOADH(R4, op_num));
	SET_VCODE_REG(TIMED_EADI_L_COPY0, LOADL(R4, op_num));
	SET_VCODE_REG(TIMED_EAA_H_COPY0, LOADH(R0, beginAddr));
	SET_VCODE_REG(TIMED_EAA_L_COPY0, LOADL(R0, beginAddr));
	SET_VCODE_REG(TIMED_EDA_H_COPY0, LOADH(R1, param->addr.outputAddr));
	SET_VCODE_REG(TIMED_EDA_L_COPY0, LOADL(R1, param->addr.outputAddr));
	SET_VCODE_REG(TIMED_SCR_COPY0, EXEC(SCR, op_num));
#if 0
	iet_print(IET_PRINT_ALG_DBG, "elp :%d, op_num : %d, input : 0x%04x, output : 0x%04x\r\n", elp, op_num, beginAddr, param->addr.outputAddr);
#endif
	/*copy input to output*/
	beginAddr = param->addr.outputAddr + op_num*elp;
	elp = inputShape->W * inputShape->H;
	//op_num = oneDataSize;
	SET_VCODE_REG(TIMED_ELP_H_COPY1, LOADH(R4, elp));
	SET_VCODE_REG(TIMED_ELP_L_COPY1, LOADL(R4, elp));
	SET_VCODE_REG(TIMED_EADI_H_COPY1, LOADH(R4, op_num));
	SET_VCODE_REG(TIMED_EADI_L_COPY1, LOADL(R4, op_num));
	SET_VCODE_REG(TIMED_EAA_H_COPY1, LOADH(R0, param->addr.inputAddr));
	SET_VCODE_REG(TIMED_EAA_L_COPY1, LOADL(R0, param->addr.inputAddr));
	SET_VCODE_REG(TIMED_EDA_H_COPY1, LOADH(R1, beginAddr));
	SET_VCODE_REG(TIMED_EDA_L_COPY1, LOADL(R1, beginAddr));
	SET_VCODE_REG(TIMED_SCR_COPY1, EXEC(SCR, op_num));
#if 0
	iet_print(IET_PRINT_ALG_DBG, "elp :%d, op_num : %d, input : 0x%04x, output : 0x%04x\r\n", elp, op_num, param->addr.inputAddr, beginAddr);
#endif
	/*copy output to timedelay*/
	elp = outputShape->W * outputShape->H;
	SET_VCODE_REG(TIMED_ELP_H_COPY2, LOADH(R4, elp));
	SET_VCODE_REG(TIMED_ELP_L_COPY2, LOADL(R4, elp));
	SET_VCODE_REG(TIMED_EADI_H_COPY2, LOADH(R4, op_num));
	SET_VCODE_REG(TIMED_EADI_L_COPY2, LOADL(R4, op_num));
	SET_VCODE_REG(TIMED_EAA_H_COPY2, LOADH(R0, param->addr.outputAddr));
	SET_VCODE_REG(TIMED_EAA_L_COPY2, LOADL(R0, param->addr.outputAddr));
	SET_VCODE_REG(TIMED_EDA_H_COPY2, LOADH(R1, param->addr.saveResultAddr));
	SET_VCODE_REG(TIMED_EDA_L_COPY2, LOADL(R1, param->addr.saveResultAddr));
	SET_VCODE_REG(TIMED_SCR_COPY2, EXEC(SCR, op_num));
#if 0
	iet_print(IET_PRINT_ALG_DBG, "elp :%d, op_num : %d, input : 0x%04x, output : 0x%04x\r\n", elp, op_num, param->addr.outputAddr, param->addr.timeDelayAddr);
#endif

	VpuStart(start_addr);

	if(param->strides[0] != 1){
		p_private_data->g_stride_now++;
		if(p_private_data->g_stride_now == param->strides[0]){
			p_private_data->g_stride_now = 0;
			return 0;
		}else{			
			return 1;
		}
	}
#endif	
	return 0;
}

int nn_timedelay_init(Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase) 
{
	// for EXTRACT_MODE, userDefine is used to store the configuration
	if(param->timesType == INTERVAL_MODE || param->timesType == NORMAL_MODE)
	{
		TimeDelayPrivateData_t *p_private_data = (TimeDelayPrivateData_t *)param->userDefine;
		p_private_data->g_stride_now = 0;
	}

    int32_t *time_delay = (int32_t *)(pBase + param->addr.saveResultAddr*32);
	if(pNet->pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
	{
		time_delay = (int32_t *)((char*)VPU_PSRAM_BASE + param->addr.saveResultAddr*64);
	}
    int timesBufferSize =  param->outputShape.H * param->outputShape.W *
                          ALIGN_16(param->outputShape.C)/2;
    for(int index=0; index<timesBufferSize; index++)
    {
        time_delay[index] = 0;
    }

	return 0;
}

