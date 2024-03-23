#ifndef __SAVERESULT_H_
#define __SAVERESULT_H_

#include "nn_file_struct.h"

//for saveresult
#define COPY_SAVE_NUM_H			0xA020870C
#define COPY_SAVE_NUM_L 		0xA0208710
#define COPY_SAVE_INPUTSTEP_H	0xA0208718
#define COPY_SAVE_INPUTSTEP_L	0xA020871C
#define COPY_SAVE_OUTPUTSTEP_H	0xA0208724
#define COPY_SAVE_OUTPUTSTEP_L	0xA0208728
#define COPY_SAVE_CCFG0			0xA0208730
#define COPY_SAVE_CCFG1			0xA0208734
#define COPY_SAVE_INPUTADDR_H	0xA020873C
#define COPY_SAVE_INPUTADDR_L	0xA0208740
#define COPY_SAVE_OUTPUTADDR_H	0xA0208748
#define COPY_SAVE_OUTPUTADDR_L 	0xA020874C
#define COPY_SAVE_ENABLE		0xA0208758
#define COPY_SAVE_SCR_NUM		0xA0208760

void nn_saveresult(const LayerParam_t *param, void *pBase, int16_t save_flag);
void nn_copydata(const uint16_t inputAddrA, 
					  const uint16_t inputAddrB,
					  const uint16_t inputAddrC,
					  const uint16_t inputAddrD, 
					  const uint8_t *strides, 
					  const MatrixShape_t *inputShape, 
					  const MatrixShape_t *outputShape,
					  const int16_t EE);
void nn_copywidthdata(const uint16_t inputAddr,
						  const uint16_t outputAddr,
						  const int32_t len,
						  uint16_t save_flag);

#endif
