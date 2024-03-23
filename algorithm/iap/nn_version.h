#ifndef __VERSION_H__
#define __VERSION_H__

#include <stdint.h>

/************************************************
*Function: get_lib_compile_date
*Description: Get Vpu Algorithm Lib Compile Date
*Input Param:
*	NULL
*Output Param:
*	char* 		   : Vpu Algorithm Lib Compile Date
*
*************************************************/
char* get_lib_compile_date();

/************************************************
*Function: get_lib_version
*Description: Get Vpu Algorithm Lib Version
*Input Param:
*	NULL
*Output Param:
*	char* 		   : Vpu Algorithm Lib Version
*
*************************************************/
char* get_lib_version();


/************************************************
*Function: get_bin_version
*Description: Get weight bin info
*Input Param:
*	NULL
*Output Param:
*	char* 		   : Weight bin info
*
*************************************************/
char* get_bin_version(void *handle);


#endif

