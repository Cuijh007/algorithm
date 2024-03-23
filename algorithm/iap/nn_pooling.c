#include "nn_pooling.h"
#include "nn_saveresult.h"

static int nn_maxpooling(const LayerParam_t *param, void* pBase)
{  
#ifndef ALGORITHM_RELEASE     
    const uint16_t Aspace = param->addr.inputAddr;
    const uint16_t Bspace = (int16_t)(param->addr.kernelAddr/0x4000)*0x4000;
    uint16_t Cspace = param->addr.biasDestAddr;
    uint16_t Dspace = param->addr.outputAddr;
    MatrixShape_t *inputShape = &param->inputShape;
    MatrixShape_t *outputShape = &param->outputShape;
    uint8_t       onestrides[2] = {1,1};
    uint16_t start_addr = 0x10a;
    //iet_print(IET_PRINT_ALG_DBG, "nn_MaxPooling\r\n");
    int copy_len = 0;
    int op_num = 0;
    int elp = 0;
#if 1
    uint16_t copy_outputAddr, max_outputAddr;
    if((param->poolingSize[0]*param->poolingSize[1]) % 2 == 1){
        copy_outputAddr = 0x4000;
        max_outputAddr = param->addr.outputAddr;
    }else{
        max_outputAddr = 0x4000;
        copy_outputAddr = param->addr.outputAddr;
    }

    /*set copy config*/
    SET_VCODE_REG(MAXP_CCFG0_COPY, CCFG0(0, 0, 0, 0, 0, 0));
    SET_VCODE_REG(MAXP_CCFG1_COPY, CCFG1(1, 1, 1, 0,0,0,0,0,0,0,0));

    /*set ELP*/
    elp = param->outputShape.W;
    SET_VCODE_REG(MAXP_ELP_H_COPY, LOADH(R4, elp));
    SET_VCODE_REG(MAXP_ELP_L_COPY, LOADL(R4, elp));    

    /*set EDI*/
    copy_len = (param->inputShape.C*16 + 255)/256;
    SET_VCODE_REG(MAXP_EDI_H_COPY, LOADH(R4, copy_len));
    SET_VCODE_REG(MAXP_EDI_L_COPY, LOADL(R4, copy_len));
    
    /*set EAI*/
    copy_len *= param->poolingSize[1];
    SET_VCODE_REG(MAXP_EAI_H_COPY, LOADH(R4, copy_len));
    SET_VCODE_REG(MAXP_EAI_L_COPY, LOADL(R4, copy_len));  
    
#if 1
    //iet_print(IET_PRINT_ALG_DBG, "copy_len: %d\r\n", copy_len);
    //iet_print(IET_PRINT_ALG_DBG, "EDI_H[0x%08x]: 0x%04x\r\n", CONV2D_EDI_H_BIAS, GET_VCODE_REG(CONV2D_EDI_H_BIAS));
    //iet_print(IET_PRINT_ALG_DBG, "EDI_L[0x%08x]: 0x%04x\r\n", CONV2D_EDI_L_BIAS, GET_VCODE_REG(CONV2D_EDI_L_BIAS));
#endif
    /*set A addr*/
    SET_VCODE_REG(MAXP_EAA_H_COPY, LOADH(R0, param->addr.inputAddr));
    SET_VCODE_REG(MAXP_EAA_L_COPY, LOADL(R0, param->addr.inputAddr));
    
    /*set D addr*/
    SET_VCODE_REG(MAXP_EDA_H_COPY, LOADH(R1, copy_outputAddr));
    SET_VCODE_REG(MAXP_EDA_L_COPY, LOADL(R1, copy_outputAddr));
#if 1
    //iet_print(IET_PRINT_ALG_DBG, "copy_outputAddr: 0x%08x\r\n", copy_outputAddr);
    //iet_print(IET_PRINT_ALG_DBG, "EDA_H[0x%08x]: 0x%04x\r\n", CONV2D_EDA_H_BIAS, GET_VCODE_REG(CONV2D_EDA_H_BIAS));
    //iet_print(IET_PRINT_ALG_DBG, "EDA_L[0x%08x]: 0x%04x\r\n", CONV2D_EDA_L_BIAS, GET_VCODE_REG(CONV2D_EDA_L_BIAS));
#endif

    /*set output w*/
    SET_VCODE_REG(MAXP_OUTPUT_H_COPY, LOADH(R5, param->outputShape.H));
    SET_VCODE_REG(MAXP_OUTPUT_L_COPY, LOADL(R5, param->outputShape.H));

    /*set SCR op num*/
    op_num = (param->inputShape.C*16 + 255)/256;
    SET_VCODE_REG(MAXP_SCR_COPY, EXEC(SCR, op_num));

    /*set step*/
    int16_t step = (param->inputShape.W*param->inputShape.C*param->poolingSize[0]*16 + 255)/256;
    int16_t step_for = step > 127 ? step/127 : 1;
    int16_t step_for_num = step > 127 ? 127 : step;
    int16_t step_last = step > 127 ? step%127 : 0;
    //iet_print(IET_PRINT_ALG_DBG, "step : %d, step_for: %d, step_for_num: %d, step_last:%d\r\n",
    //    step, step_for, step_for_num, step_last);
    SET_VCODE_REG(MAXP_INPUT_STEP_FOR_H_COPY, LOADH(R6, step_for));
    SET_VCODE_REG(MAXP_INPUT_STEP_FOR_L_COPY, LOADL(R6, step_for));
    SET_VCODE_REG(MAXP_INPUT_STEP_FOR_COPY, INC(R0, step_for_num));
    SET_VCODE_REG(MAXP_INPUT_STEP_FOR_LAST_COPY, INC(R0, step_last));
    step = (param->outputShape.W*param->outputShape.C*16 + 255)/256;
    if(step > 254){
        iet_print(IET_PRINT_ERROR, "maxPooling err step : %d\r\n", step);
    }
    step_for_num = step > 127 ? 127 : step;
    step_last = step > 127 ? step%127 : 0;
    SET_VCODE_REG(MAXP_OUTPUT0_STEP_COPY, INC(R1, step_for_num));
    SET_VCODE_REG(MAXP_OUTPUT1_STEP_COPY, INC(R1, step_last));

    /*set max elp*/
    SET_VCODE_REG(MAXP_ELP_H_POOLING, LOADH(R4, elp));
    SET_VCODE_REG(MAXP_ELP_L_POOLING, LOADL(R4, elp));

    /*set EBI*/
    copy_len = (param->inputShape.C*16 + 255)/256;
    SET_VCODE_REG(MAXP_EBI_H_POOLING, LOADH(R4, copy_len));
    SET_VCODE_REG(MAXP_EBI_L_POOLING, LOADL(R4, copy_len));

    /*set EDI*/
    SET_VCODE_REG(MAXP_EDI_H_POOLING, LOADH(R4, copy_len));
    SET_VCODE_REG(MAXP_EDI_L_POOLING, LOADL(R4, copy_len));
    
    /*set EAI*/
    copy_len *= param->poolingSize[1];
    //iet_print(IET_PRINT_ALG_DBG, "copy_len = %d, poolingSize[0]:%d, poolingSize[1]:%d\r\n",
    //    copy_len, param->poolingSize[0],param->poolingSize[1]);
    SET_VCODE_REG(MAXP_EAI_H_POOLING, LOADH(R4, copy_len));
    SET_VCODE_REG(MAXP_EAI_L_POOLING, LOADL(R4, copy_len));  

    /*set max pooling config*/
    SET_VCODE_REG(MAXP_CCFG0_POOLING, CCFG0(0, 0, 0, 0, 0, 0));
    SET_VCODE_REG(MAXP_CCFG1_POOLING, CCFG1(1, 1, 1, 0,0,0,0,0,0,0,0));

    /*set A B Daddr*/
    SET_VCODE_REG(MAXP_EAA_H_POOLING, LOADH(R0, param->addr.inputAddr));
    SET_VCODE_REG(MAXP_EAA_L_POOLING, LOADL(R0, param->addr.inputAddr));
    SET_VCODE_REG(MAXP_EBA_H_POOLING, LOADH(R1, copy_outputAddr));
    SET_VCODE_REG(MAXP_EBA_L_POOLING, LOADL(R1, copy_outputAddr));
    SET_VCODE_REG(MAXP_EDA_H_POOLING, LOADH(R2, max_outputAddr));
    SET_VCODE_REG(MAXP_EDA_L_POOLING, LOADL(R2, max_outputAddr));

    /*set pool size*/
    SET_VCODE_REG(MAXP_POOL_SIZE_H_H_POOL, LOADH(R5, param->poolingSize[0]));
    SET_VCODE_REG(MAXP_POOL_SIZE_H_L_POOL, LOADL(R5, param->poolingSize[0]));
    SET_VCODE_REG(MAXP_POOL_SIZE_W_H_POOL, LOADH(R6, param->poolingSize[1]));
    SET_VCODE_REG(MAXP_POOL_SIZE_W_L_POOL, LOADL(R6, param->poolingSize[1]));
    SET_VCODE_REG(MAXP_OUT_SIZE_H_H_POOL, LOADH(R7, param->outputShape.H));
    SET_VCODE_REG(MAXP_OUT_SIZE_H_L_POOL, LOADL(R7, param->outputShape.H));

    /*set MAX op num*/
    //op_num = (param->inputShape.C*16 + 255)/256;
    SET_VCODE_REG(MAXP_MAX_POOL, EXEC(VMAX, op_num));
    //iet_print(IET_PRINT_ALG_DBG, "MAXP_MAX_POOL op_num : %d\r\n", op_num);

    /*set step*/
    step = (param->inputShape.W*param->inputShape.C*param->poolingSize[0]*16 + 255)/256;
    step_for = step > 127 ? step/127 : 1;
    step_for_num = step > 127 ? 127 : step;
    step_last = step > 127 ? step%127 : 0;
    SET_VCODE_REG(MAXP_INPUT_STEP_FOR_H_POOL, LOADH(R8, step_for));
    SET_VCODE_REG(MAXP_INPUT_STEP_FOR_L_POOL, LOADL(R8, step_for));
    SET_VCODE_REG(MAXP_INPUT_STEP_FOR_POOL, INC(R10, step_for_num));
    SET_VCODE_REG(MAXP_INPUT_STEP_FOR_LAST_POOL, INC(R10, step_last));
    step = (param->outputShape.W*param->outputShape.C*16 + 255)/256;
    //iet_print(IET_PRINT_ALG_DBG, "input pool w step : %d\r\n", step);
    if(step > 254){
        iet_print(IET_PRINT_ERROR, "maxPooling err step : %d\r\n", step);
    }
    step_for_num = step > 127 ? 127 : step;
    step_last = step > 127 ? step%127 : 0;    
    SET_VCODE_REG(MAXP_OUTPUT0_STEP_POOL, INC(R11, step_for_num));
    SET_VCODE_REG(MAXP_OUTPUT1_STEP_POOL, INC(R11, step_last));
    SET_VCODE_REG(MAXP_OUTPUT2_STEP_POOL, INC(R12, step_for_num));
    SET_VCODE_REG(MAXP_OUTPUT3_STEP_POOL, INC(R12, step_last));    
    SET_VCODE_REG(MAXP_INPUT_POOLW_STEP_POOL, INC(R0, op_num));
    step = (param->inputShape.W*param->inputShape.C*16 + 255)/256 - (param->poolingSize[1]*op_num);
    //iet_print(IET_PRINT_ALG_DBG, "input pool h step : %d\r\n", step);
    step_for_num = step > 127 ? 127 : step;
    step_last = step > 127 ? step%127 : 0;     
    SET_VCODE_REG(MAXP_INPUT_POOLH0_STEP_POOL, INC(R0, step_for_num));
    SET_VCODE_REG(MAXP_INPUT_POOLH1_STEP_POOL, INC(R0, step_last));
#endif
    VpuStart(start_addr);
#endif    
    return 0;
}


static int nn_averagePooling(const LayerParam_t *param, void *pBase)
{
    iet_print(IET_PRINT_ALG_DBG, "nn_averagePooling\r\n");
    return 0;
}

nn_pooling(const LayerParam_t *param, void* pBase)
{
    //iet_print(IET_PRINT_ALG_DBG, "do pooling\r\n");
    int16_t paddingSize = param->padding[0][0] +
                          param->padding[0][1] +
                          param->padding[1][0] +
                          param->padding[1][1];
    if(paddingSize != 0)
    {
        //iet_print(IET_PRINT_ALG_DBG, "pooling need padding\r\n");
        //bach1c_padding(param->addr.inputAddr, param->addr.outputAddr, param->addr.biasAddr, param->addr.biasDestAddr, param->padding, &param->inputShape);
    }

    uint8_t poolingType = param->poolingType;
    int Ret = 0;
    switch (poolingType)
    {
        case NONE_POOL:
            //type is None;
            Ret = 0;
            break;
        case MAX:
            //do maxPooling
            Ret = nn_maxpooling(param, pBase);
            break;
        case 2:
            //do average pooling
            Ret = nn_averagePooling(param, pBase);
            break;
        default:
            //not support
            Ret = -1;
            break;
    };
    //exit(0);
    return Ret;
}