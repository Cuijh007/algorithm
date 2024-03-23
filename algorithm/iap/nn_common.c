#include "nn_common.h"

void setRegistData(unsigned int addr, short data, short startbit, short len)
{
	if(data > ((1<<len)-1))
	{
		iet_print(IET_PRINT_ERROR, "SET REG ERROR: %d, 0x%04x\r\n", data, addr);
		exit(0);
		//data = ((1<<len)-1);
	}
	unsigned int registerdata = *((volatile unsigned int*)(addr));
	//set need change data to zeros
	unsigned int resetdata = 0xFFFFFFFF - (((1<<len) -1) << startbit);
	//set data
	*((volatile unsigned int*)(addr)) =  (int)(((registerdata & resetdata) | ((unsigned int)data << startbit)));
}

/****************************************************************
*note:
*	bug泛滥，我已瘫痪！
*	when i wrote this, only god and i understand what i was doing
*   Now only god konws
*   Please read this code and i hope you'll know what the following code does
*   good luck to you!
*
*****************************************************************/
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
					 const uint8_t 		 keep_C)
{
    //TODO: check if this function is needed

}

