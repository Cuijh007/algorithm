#include "nn_file_struct.h"

char* get_lib_compile_date()
{
    static char DateTime[64] = {0};
    sprintf(DateTime, "Build@%s %s", __DATE__, __TIME__);
	return DateTime; 
}


/*************************************************
*version:0.0.1
*Description : first version
*
*version:0.0.2
*Description : add fbank overflow truncate protect
*
*version:2.0.0
*Description: 修改GRU量化方式，增加State第一次修改。
*             优化激活函数
*
*
*version:2.0.1
*Description: 增加命令词识别之后清空所有层缓存数据
*       	  修改后处理，在找到最大值之后的第30帧强制输出
*
*
*version:2.1.0
*Description: 增加TDNN timeDelay层
*
*
*version:3.0.0
*Description: 支持H*W >= 1024
* 			  对卷积和全连接运算拆分。copybias -> conv/dense -> mult scale add vector
*
**********************************************/
char* get_lib_version()
{
	return (__LIB_VERSION__);
}


char* get_bin_version(void *handle)
{
	Net_t *pNet = (Net_t *)handle;
	return (pNet->pNetInfo->Version);
}