//#include <stddef.h>
//#include <BACH.h>

#include "iet_common.h"
#include "iet_qspi_flash.h"

#include "nn_file_struct.h"
#include "nn_vpu_control.h"


typedef uint32_t (*WAIT_DONE)(uint32_t timeout_period);
extern WAIT_DONE  npu_finsh;

#define	CLEAR_BIT(x, bit)	(x &= ~(1 << bit))

extern uint32_t iet_systick_read();
void VpuStart(unsigned short startAddr)
{
	// uint32_t start = iet_systick_read();
	// iet_print(IET_PRINT_ALG_DBG, "npu time start  %d\r\n", start);

	uint32_t reset = *(volatile uint32_t*)0x51b00008;
	reset = CLEAR_BIT(reset, 17);
	*(volatile uint32_t *)0x51b00008 = reset;
	*(volatile uint32_t *)0x51b00008 = reset|(1<<17);

	reset = *(volatile uint32_t*)0x51b00004;
	reset = CLEAR_BIT(reset, 9);
	*(volatile uint32_t *)0x51b00004 = reset;
	*(volatile uint32_t *)0x51b00004 = reset|(1<<9);
	
	// the result is same whether dmaen bit is set or not
    *(volatile uint32_t *)VPU_CONTROL_REG = 0x000003FF | ((startAddr&0x1FFF)<<16);
    // *(volatile uint32_t *)VPU_CONTROL_REG = 0x000003FF | ((startAddr&0x1FFF)<<16) | (1<<11);
	
	if (!npu_finsh)
	{
		iet_print(IET_PRINT_ERROR, "npu_finsh NULL!!!!!!\r\n");
	}
	else
	{	
		int result = npu_finsh(500);
		if(result != 0)
		{
			uint32_t errCode =  *(volatile uint32_t *)VPU_CONTROL_REG;
			errCode = (errCode>>VPU_ERRCODE_OFFSET)&0xF;
			iet_printf_err("npu_finsh time out!!!!!! startAddr = %d\r\n", startAddr);
		}
	}
	// uint32_t end = iet_systick_read();
	// iet_print(IET_PRINT_ALG_DBG, "npu time  %d\r\n", end -start);
	// iet_print(IET_PRINT_ALG_DBG, "VPU_CONTROL_REG = %p\r\n", *(volatile uint32_t *)VPU_CONTROL_REG);
	*(int*)VPU_CONTROL_REG = 0x00000000;
	return ;
}

/*load vpu code of the specified layers to VPU_CODE_ADDR*/
void load_vcode_layers(int32_t model_index, int32_t start_id, int32_t end_id)
{
	extern int32_t vcode_offset;
	Net_t *pNet = &g_net;
	LayerParam_t *pLayers = &pNet->pNetInfo->Layers[0];

#if 0        
	uint16_t *p_vpu_code = (uint16_t *)pNet->pVpuCode;
	int vpucode_size = pNet->pNetInfo->VpuCodeSize[0];
	if (model_index == 1)
	{
		p_vpu_code += pNet->pNetInfo->VpuCodeSize[0];
		vpucode_size = pNet->pNetInfo->VpuCodeSize[1];
	}
	for(int i = 0; i < vpucode_size; i++)
	{
		*((int *)VPU_CODE_ADDR + i) = ((int)p_vpu_code[i] & 0x00000FFFF);
	}
#else
	// vcode addr info in netinfo for each model is independent
	// load the part needed since the whole vcode size of a model may be over than 4K
	uint16_t *p_vpu_code = (uint16_t *)pNet->pVpuCode;
	for(int i=0; i<model_index; i++)
	{
		p_vpu_code += pNet->pNetInfo->VpuCodeSize[i];
	}
	uint32_t *p_dst = (uint32_t *)VPU_CODE_ADDR;
	int32_t start_addr = pLayers[start_id].vpuPara.vpu_start_addr;
	int32_t end_addr = pLayers[end_id].vpuPara.vpu_end_addr;
	int32_t vpucode_size = end_addr - start_addr + 1;
	uint16_t *p_src = p_vpu_code + start_addr;
	for(int i = 0; i < vpucode_size; i++)
	{
		// to work for both cases: load from sram or psram, doesn't work for flash
		*p_dst++ = (uint32_t)(*p_src++);
	}
		*p_dst = 0;
	vcode_offset = start_addr;
	if(vpucode_size > MAX_VPU_CODE_SIZE)
	{
		iet_print(IET_PRINT_ERROR, "VCode Size[%d] out of space, M %d %d-%d\r\n", vpucode_size, model_index, start_id, end_id);
	}
	// iet_print(IET_PRINT_ALG_DBG, "Load m%d %d-%d [%d %d], s: %d off %d\r\n", 
	// 	model_index, start_id, end_id, start_addr, end_addr, vpucode_size, vcode_offset);
#endif	
}

void load_vcode(int32_t model_index)
{
	extern int32_t vcode_offset;
    Net_t *pNet = &g_net;
    int vpucode_size = pNet->pNetInfo->VpuCodeSize[model_index];
    uint32_t *p_vpu_code = (uint32_t *)pNet->pVpuCode;
    for(int i=0; i<model_index; i++)
    {
        p_vpu_code += pNet->pNetInfo->VpuCodeSize[i]/2;
    }
    uint32_t *p_dst = (uint32_t *)VPU_CODE_ADDR;
    for(int i = 0; i < vpucode_size/2; i++)
    {
		// to work for both cases: load from sram or flash
		// it will take extra 0.8ms when loading from flash, 721@184MHz
        uint32_t value = p_vpu_code[i];
        *p_dst++ = value & 0x00000FFFF;
        *p_dst++ = (value>>16)& 0x00000FFFF;
    }
	if(vpucode_size > MAX_VPU_CODE_SIZE)
	{
		iet_print(IET_PRINT_ERROR, "VCode Size[%d] out of space, M %d\r\n", vpucode_size, model_index);
	}	
	vcode_offset = 0;
}
