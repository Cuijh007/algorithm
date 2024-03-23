#include <string.h>
#include "nn_userdefine_gather.h"
#include "nn_net.h"
#include "mid_common.h"
#include "nn_vpu_control.h"

int16_t gather_offset_index;  // update dynamically with each for-loop.
int16_t eos_offset_index;     // eos position of each sentence.

int nn_userdefine_gather(LayerParam_t *param, void* players, void *pBase){
    LayerParam_t *layers = (LayerParam_t *)players;

    UserDefineGatherSetting_t *ud_gather = (UserDefineGatherSetting_t *)param->userDefine;
    
    int16_t input_block = param->addr.inputAddr / 0x4000;
    //此时无SCR vcode将input -> output
    if(param->truncate[0] == 0)
        input_block = param->addr.outputAddr / 0x4000;
    int16_t psram_offset = param->outputShape.C / 32;
    int32_t load_addr;


    if(ud_gather->dynamic == 1){
        load_addr = layers[param->inputLayerId[0]].addr.saveResultAddr + gather_offset_index*psram_offset;
    }else{
        load_addr = layers[param->inputLayerId[0]].addr.saveResultAddr + eos_offset_index*psram_offset;
    }

    uint32_t vpucode_addr = (uint32_t)(VPU_CODE_ADDR+(ud_gather->vcode_line_num*sizeof(uint32_t)));
    SET_VCODE_REG(vpucode_addr, CCFG2(psram_offset-1, input_block));
    SET_VCODE_REG(vpucode_addr+4, CCFG3(load_addr, 0));
    SET_VCODE_REG(vpucode_addr+8, CCFG4(load_addr));

    return 0;
}