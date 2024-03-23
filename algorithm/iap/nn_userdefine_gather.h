#ifndef __USERDEFINE_GATHER_H__
#define __USERDEFINE_GATHER_H__

#include "nn_file_struct.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct UserDefineGatherSetting_t {
	int16_t dynamic; 
	uint16_t vcode_line_num;
}UserDefineGatherSetting_t;

int nn_userdefine_gather(LayerParam_t *param, void* players, void *pBase);

#ifdef __cplusplus
}
#endif
#endif
