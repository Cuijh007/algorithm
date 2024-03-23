#include "nn_saveresult.h"
#include "nn_vpu_control.h"
#include "nn_rnndenseadd.h"



int nn_rnndenseadd(const LayerParam_t *param, void* players, void *pBase)
{
#ifndef ALGORITHM_RELEASE
	int16_t tmp_result[1024] = {0};
	LayerParam_t *layers = (LayerParam_t *)players;

	//设置输入X input数据
	int16_t inputLayerId = param->inputLayerId[0];
	int16_t InpautLayerSize = ALIGN_16(param->inputShape.C - param->outputShape.C);
	int16_t ExtenInputC = (InpautLayerSize+15)/16 * 16;
	LayerParam_t CellParam;
	CellParam.outputShape.H = 1;
	CellParam.outputShape.W = 1;
	CellParam.outputShape.C = InpautLayerSize;
	CellParam.addr.outputAddr = layers[inputLayerId].addr.saveResultAddr;
	CellParam.addr.saveResultAddr = param->addr.inputAddr;
	nn_saveresult(&CellParam, pBase, 1);
		
	//x运算
	LayerParam_t CellParam1 = {
		.inputShape = {1, 1, InpautLayerSize},
		.outputShape = param->outputShape,
		.kernelShape = param->kernelShape,
		.strides = {1,1},
		.activation = 0,
		.use_b = 1,
		.use_s = 1,
		.use_b8 = 1,
		.use_v = 0,
		.model_q = 0,
		.truncate = {param->truncate[0], param->truncate[1], 0, 0},
		.addr = param->addr
	};
	CellParam1.addr.scaleAddr = CellParam1.addr.scaleAddr;
	nn_dense(&CellParam1);

	//保存中间结果
	int ExtenOutput = (param->outputShape.C + 15)/16 * 16;
	uint32_t* output_addr = (param->addr.outputAddr)*32 + pBase;
	//iet_print(IET_PRINT_ALG_DBG, "result\r\n");
	for(int index=0; index<ExtenOutput/2; index++)
	{
		int tmp = output_addr[index];
		tmp_result[2*index] = (int16_t)(tmp&0xFFFF);
		tmp_result[2*index+1] = (int16_t)((tmp>>16)&0xFFFF);
		//iet_print(IET_PRINT_ALG_DBG, "%d,%d,", tmp_result[2*index], tmp_result[2*index+1]);
	}
	//iet_print(IET_PRINT_ALG_DBG, "\r\n");
	
	//设置输入State数据	
	inputLayerId = param->inputLayerId[1];
	CellParam.outputShape = param->outputShape;
	CellParam.addr.outputAddr = layers[inputLayerId].addr.saveResultAddr;
	CellParam.addr.saveResultAddr = param->addr.inputAddr;
	nn_saveresult(&CellParam, pBase, 1);

	//计算权重偏移
	CellParam1.inputShape = param->outputShape;
	CellParam1.use_v = 0;
	CellParam1.use_s = 1;
	CellParam1.use_b = 1;
	CellParam1.addr.kernelAddr = param->addr.kernelAddr + (ExtenInputC*ExtenOutput*8 + 511)/512;
	CellParam1.addr.scaleAddr = param->addr.scaleAddr + (ExtenOutput*16 + 511)/512;
	CellParam1.addr.biasAddr = param->addr.vectorAddr;
	CellParam1.truncate[0] = param->truncate[2];
	CellParam1.truncate[1] = param->truncate[4];

	nn_dense(&CellParam1);
	for(int index=0; index<ExtenOutput/2; index++)
	{
		int tmp = output_addr[index];
		int16_t lowbit_data = SAT_INT16T(tmp_result[2*index] + (int16_t)(tmp&0x0000FFFF));
		int16_t highbit_data = SAT_INT16T(tmp_result[2*index+1] + (int16_t)((tmp>>16)&0xFFFF));
		output_addr[index] =(uint32_t)((lowbit_data&0x0000FFFF) | ((highbit_data<<16)&0xFFFF0000));
	}
#endif
	return 0;
}


int nn_rnnstate(const LayerParam_t *param, void *players, void* pBase)
{
#ifndef ALGORITHM_RELEASE
	int16_t inputLayerId = param->inputLayerId[0];
	LayerParam_t *layers = (LayerParam_t*)players;
	const uint32_t *input_addr = (const uint32_t*)(layers[inputLayerId].addr.saveResultAddr*32 + pBase);
	uint32_t *output_addr = param->addr.outputAddr*32 + pBase;
	int32_t outputSize = param->outputShape.H * param->outputShape.W * ALIGN_16(param->outputShape.C);
	int truncate = param->truncate[2] - param->truncate[3];

	for(int index=0; index<outputSize/2; index++)
	{
		uint32_t tmp_data = input_addr[index]; 
		int16_t tmp_a = ((int16_t)(tmp_data&0x0000FFFF) >> truncate);
		int16_t tmp_b = ((int16_t)((tmp_data>>16)&0x0000FFFF) >> truncate);
		output_addr[index] = (tmp_a&0x0000FFFF)|((tmp_b&0x0000FFFF)<<16);
	}
#endif
	return 0;
}

int nn_initstate(const LayerParam_t *param, void* players, void *pBase) {

	InitStateSetting_t *initstate = (InitStateSetting_t *)param->userDefine;
    int init_layer_id = initstate->init_layer_id;
	LayerParam_t *layers = players;
	int32_t *dstAddr = (const int32_t*)(layers[init_layer_id].addr.saveResultAddr * 64 + VPU_PSRAM_BASE);   
	const int32_t *input_addr = (const int32_t*)(param->addr.inputAddr*32 + pBase);
	int32_t *outputAddr = (int32_t*)(param->addr.outputAddr*32 + pBase);
	MatrixShape_t shape = param->outputShape;

	int8_t truncate = param->truncate[0];

	int32_t num = shape.H * shape.W * ALIGN_16(shape.C);

	if(truncate < 0)
	{
		for(int i=0; i<num/2; i++)
		{

            int16_t inputHigh = (int16_t)( (input_addr[i]>>16)&0xFFFF );
            int16_t inputLow = (int16_t)(input_addr[i]&0xFFFF);

            int16_t outputHigh = SAT_INT16T(inputHigh*(1<<(-truncate)));
            int16_t outputlow = SAT_INT16T(inputLow*(1<<(-truncate)));
			outputAddr[i] = (int32_t)outputlow&0x0000FFFF | ((int32_t)outputHigh<<16)&0xFFFF0000;

            dstAddr[i] = outputAddr[i];
		}

	}
	else
	{
		for(int i=0; i<num; i++)
		{
			outputAddr[i] = (input_addr[i] >> truncate);
		}
        int32_t *outputAddr_32 = (int32_t*)(param->addr.outputAddr*32 + pBase);
        for(int i=0; i<num/2; i++)
		{
			dstAddr[i] = outputAddr_32[i];
		}
	}
	return 0;
}

int nn_gruspecialtrement(const LayerParam_t *param, void* players, void *pBase)
{
#ifndef ALGORITHM_RELEASE
	//iet_print(IET_PRINT_ALG_DBG, "nn_gruspecialtrement\r\n");
	LayerParam_t *layers = players;
	int outputsize = param->outputShape.H * param->outputShape.W * ((param->outputShape.C+15)/16) *16;
	int16_t* inputid = param->inputLayerId;
	const int* zt = (layers[inputid[0]].addr.saveResultAddr * 32) + pBase;      // Zt
	const int* ht = (param->addr.inputAddr*32+pBase);//(layers[inputid[1]].addr.saveResultAddr * 32) + pBase;      // Ht
	const int* status = (layers[inputid[2]].addr.saveResultAddr * 32) + pBase;      // Status

	int* outputaddr = (param->addr.outputAddr)*32 + pBase;
    
	for(int index=0; index<outputsize/2; index++)
	{
		int zt_l = (int16_t)(zt[index]&0x0000FFFF);
		int zt_h = (int16_t)((zt[index]>>16)&0x0000FFFF);
		int ht_l = (int16_t)(ht[index]&0x0000FFFF);
		int ht_h = (int16_t)((ht[index]>>16)&0x0000FFFF);
		int status_l = (int16_t)(status[index]&0x0000FFFF);
		int status_h = (int16_t)((status[index]>>16)&0x0000FFFF);
	
		int32_t tmp = ((zt_l*status_l)>>(param->truncate[0]));
		tmp += (((32767 - zt_l)*ht_l)>>(param->truncate[1]));
		int16_t lowdata = SAT_INT16T(tmp);
		int16_t highdata = SAT_INT16T(((zt_h*status_h)>>(param->truncate[0])) + (((32767 - zt_h)*ht_h)>>(param->truncate[1])));
		outputaddr[index] = (int32_t)lowdata&0x0000FFFF | ((int32_t)highdata<<16)&0xFFFF0000 ;
	}
#endif
    return 0;
}

int nn_GruResetStatus(Net_t *pNet, int model_index, void *pBase) 
{

	NetInfo_t *pNetInfo = pNet->pNetInfo;
    int32_t startId = pNetInfo->NetLayersStartId[model_index];
    int32_t endId = pNetInfo->NetLayersEndId[model_index];

    int gru_index = 0;
    for(int i=startId; i<=endId; i++)
    {
        LayerParam_t *curLayer = &pNetInfo->Layers[i];
        if(curLayer->type == GRUSpecialTreatment)
        {                    
            int32_t *p_data = pNetInfo->GruStatusOffset[model_index][gru_index] == 0 ? 0 : (int32_t*)(pNetInfo->GruStatusOffset[model_index][gru_index] + (char*)pNet->pPrivateData);
            int32_t ExtenOutputC = ALIGN_16(curLayer->outputShape.C);
            int32_t outputsize = curLayer->outputShape.H * curLayer->outputShape.W * ExtenOutputC/2;
            int32_t *saveAddr = (int32_t*)(pBase + (curLayer->addr.saveResultAddr*32));
			if(pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
			{
				saveAddr = (int32_t *)((char*)VPU_PSRAM_BASE + curLayer->addr.saveResultAddr*64);
			}			
            for(int index=0; index<outputsize; index++)
            {
                 if(p_data)
                     saveAddr[index] = *p_data++;
                 else
                     saveAddr[index] = 0;				
            }
            gru_index++;
        }
    }
	
	return 0;
   
}


