#ifndef __NET_H__
#define __NET_H__

#include <stdint.h>


void DebugLayerData(const Net_t *pNet, int layer_id, int frame_index, int print_in, int print_out, Bool print_hex);
void set_debug_layer(int model_index, int layer_id, int frame_index, int vcode_addr);
void set_npu_stop_layer(int model_index, int layer_id, int vcode_addr);
void set_npu_stop_layer_by_vcode_mem(int model_index, int layer_id, int vcode_addr);
extern int frame_index_to_print;
extern int stop_model_index;
extern int stop_layer_id;
extern int stop_frame_index; // to stop the specified frame, the specified layer
extern int stop_vcode_addr;

/************************************************
*Function: NetInit
*Description: Initialize the private datas of the net to run
*Input Param:
*	*net           : vpu handle , Return value of LoadNet
*   model_index    : model index
*Output Param:
*	int 		   : if sucess retrun Neural NetWork Output Data Size ; else return -1
*
*************************************************/
int NetInit(const Net_t *pNet, int model_index);


/************************************************
*Function: NetForward
*Description: Compute Neural Network
*Input Param:
*	*net           : vpu handle , Return value of LoadNet
*   model_index    : model index
*   start_id       : start layer ID to run
*   end_id         : end layer ID(included) to run
*	*input_buffer  : Neural Network Input Data
*	*output_buffer : Neural Network Output Data
*Output Param:
*	int 		   : if sucess retrun Neural NetWork Output Data Size ; else return -1
*
*************************************************/
int NetForward(const Net_t *pNet, int model_index, int start_id, int end_id, const int16_t *input_buffer, int16_t *output_buffer);
#endif

