/*****************************************************************************
*  
*  Generated automatically for beam search, please don't touch it 
*  Common header file for cmd_tree.bin 
*  
*****************************************************************************/
#ifndef _BEAM_COMMON_H_
#define _BEAM_COMMON_H_


#include <stdint.h>




typedef enum {
     BS_MODE_COMMAND = 0,               // whole command search
     BS_MODE_PHRASE,                    // phrase search
     BS_MODE_WORD,                      // word search
     BS_MODE_NUM
} BeamSearchMode;


typedef struct {
     uint8_t data;                      // word or phrase index
     uint8_t chance;                    // probability of this value in all sibling
     uint16_t first_child;               // index of the first child in next layer
     uint16_t child_num;                 // number of the child, childrens are placed together
                                        // first_child is cmd id of this branch if child num is zero
} BeamWordNode;


typedef struct BeamInfo {
     uint16_t work_mode;                // please refer to BeamSearchMode for details
     uint16_t cmd_tree_size;            // nodes number of commd tree
     uint16_t word_num;                 // word(phrase) number of the model
     uint16_t cmd_num;                  // command number
     uint16_t max_cmd_length;           // max word(phrase) number to form the command
     uint16_t cmd_tree_nodes_offset;    // offset of cmd_tree_nodes[] to the first addreess of private data, in byte
     uint16_t word_last_time_offset;    // offset of word_last_time[] to the first addreess of private data, in byte
     uint16_t layer_node_num_offset;    // offset of layer_node_num[] to the first addreess of private data, in byte
     uint16_t layer_offset_offset;      // offset of layer_offset[] to  the first addreess of private data, in byte
} BeamInfo;


#endif // _BEAM_COMMON_H_