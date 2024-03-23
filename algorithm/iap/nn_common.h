#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include "nn_file_struct.h"
#include "nn_vpu_control.h"

typedef union Reslut{
    int32_t a;
    int16_t b[2];
    int8_t  c[4];    
}type_un;

typedef struct LayerId4Skip_s
{
	int16_t start_id;
	int16_t end_id;
}LayerId4Skip_t;

void setRegistData(unsigned int addr, short data, short startbit, short len);

void nn_common(  const uint16_t 	 inputAddrA, 
					 const uint16_t 	 inputAddrB,
					 const uint16_t 	 inputAddrC,
					 const uint16_t 	 inputAddrD,
					 const uint8_t  	 *strides,
					 const MatrixShape_t *inputShape,
					 const MatrixShape_t *outputShape,
					 const int16_t 		 EE,
					 const int16_t 		 truncate,
					 const uint16_t 	 ccfg1_data,
					 const uint8_t		 operateCode,
					 const uint8_t 		 keep_A,
					 const uint8_t 		 keep_B, 
					 const uint8_t 		 keep_C);


#endif
