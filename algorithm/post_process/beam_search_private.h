/*****************************************************************************
*  This software is confidential and proprietary and may be used 
*  only as expressly authorized by a licensing agreement from
*                     Intengine.
*            
*              (C) COPYRIGHT 2019 INTENGINE
*                     ALL RIGHTS RESERVED 
*
*  @file     beam_search.h
*  @brief    generate the tree for beam search algorithm
*  @author   Xiaoyu.Ren
*  @email    xyren@intenginetech.com
*  @version  1.1
*  @date     2020-05-29
*
*----------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/04/07 | 0.1       | Xiaoyu.Ren     | Initial file
*  2020/04/28 | 1.0       | Xiaoyu.Ren     | Verfied with model
*  2020/05/29 | 1.1       | Xiaoyu.Ren     | Improve algo for whole word cmd
*  2020/06/03 | 1.2       | Xiaoyu.Ren     | Update interface
*  2020/10/13 | 1.3       | Xiaoyu.Ren     | version for whole cmmand/split phrase/split word
*****************************************************************************/
#ifndef _BEAM_SEARCH_INTERNAL_H_
#define _BEAM_SEARCH_INTERNAL_H_  
    
#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "beam_common.h"
#include "beam_search.h"
#include "mid_pp.h"
#include "mid_pp_private.h"

#define ENABLE_SUBWORD_PATH                 // enable this if want to send subword ids to app, for board only

#define MAX_SUBWORD_NUM_IN_CMD          7   //
#define RESET_DELAY_IN_FRAME            15  // reset BS after continue N sil frames is detected


#define TIME_OUT_MS                     10000       // time out of beam search in ms
#define MID_PP_ALIGN                    4
#define MAX_BEAM_SIZE_CMD               100
#define MAX_BEAM_SIZE_WORD              50
#define MAX_BEAM_SIZE_PHRASE            50

#define MIN_BEAM_SIZE_CMD               10
#define MIN_BEAM_SIZE_WORD              30
#define MIN_BEAM_SIZE_PHRASE            30

#define MAX_CANDIDATE_NUM               200

#define SILENT_FRAME_THRESHOLD          28000

// for command mode
#define CMD_SILENT_FRAME_THRESHOLD      24000
#define CMD_CONTINUE_ZERO_CMD_END       3
#define CMD_CONTINUE_ZERO_NODE_END      3          // Useless for whole command search
#define CMD_NODE_SWITCH_MIN_FRAME       0           // Useless for whole command search    
#define CMD_SILENT_COUNT_CMD_END        1           // this value should be greater than CMD_CONTINUE_ZERO_CMD_END
#define CMD_WORD_MIN_VALUE              6553
#define CMD_CMD_END_SIL_THRESHOLD       CMD_WORD_MIN_VALUE  // this value should be not greater than CMD_WORD_MIN_VALUE
#define CMD_MIN_SCORE_VALID_FRAME       MIN_SCORE_VALID_FRAME       // bigger value for strict term
#define CMD_WORD_SCORE_OFFSET           1000        // bigger value for EASY term

// for phrase mode
#define PHRASE_SILENT_FRAME_THRESHOLD   28000
#define PHRASE_CONTINUE_ZERO_CMD_END    1
#define PHRASE_CONTINUE_ZERO_NODE_END   30          // Considering that the interval of words caused by the speaker, this doesnt' affect response latency
#define PHRASE_NODE_SWITCH_MIN_FRAME    1           // continue frame
#define PHRASE_SILENT_COUNT_CMD_END     4           // this value should be greater than PHRASE_CONTINUE_ZERO_CMD_END
// increasing this value will help reduce FAR, 
// for MSE loss when bigger one such as 5000 is used, PHRASE_SILENT_FRAME_THRESHOLD should be use together to check sil frame.
// for CE loss, the sum of this two value should be 1.0
#define PHRASE_WORD_MIN_VALUE           (32768-PHRASE_SILENT_FRAME_THRESHOLD)
#define PHRASE_CMD_END_SIL_THRESHOLD    PHRASE_WORD_MIN_VALUE
#define PHRASE_MIN_SCORE_NODE_KEEP      6553       // 0.30*32767, bigger value for strict term
#define PHRASE_MIN_SCORE_NODE_SWITCH    5000        // 0.15*32767, bigger value for strict term
#define PHRASE_MIN_SCORE_CHILD_CHECK    6553       // 0.30*32767, bigger value for strict term
#define PHRASE_MIN_SCORE_VALID_FRAME    6553       // bigger value for strict term
#define PHRASE_WORD_SCORE_OFFSET        1000        // bigger value for EASY term

// for word mode
#define WORD_SILENT_FRAME_THRESHOLD     28000
#define WORD_CONTINUE_ZERO_CMD_END      2
#define WORD_CONTINUE_ZERO_NODE_END     20
#define WORD_NODE_SWITCH_MIN_FRAME      1
#define WORD_SILENT_COUNT_CMD_END       4
#define WORD_WORD_MIN_VALUE             2000    
#define WORD_CMD_END_SIL_THRESHOLD      WORD_WORD_MIN_VALUE
#define WORD_MIN_SCORE_NODE_KEEP        2000        // 0.30*32767, bigger value for strict term
#define WORD_MIN_SCORE_NODE_SWITCH      2000        // 0.15*32767, bigger value for strict term
#define WORD_MIN_SCORE_CHILD_CHECK      2000        // 0.30*32767, bigger value for strict term
#define WORD_MIN_SCORE_VALID_FRAME      2000        // bigger value for strict term
#define WORD_WORD_SCORE_OFFSET          1000        // bigger value for EASY term

typedef enum {
    BS_CMD_END_NONE,                    // cmd doesn't end
    BS_CMD_END_TRUE,                    // cmd end detected, but not reported to the caller of BS
    BS_CMD_END_REPORTED,                // the cmd already reported to the caller, though it's still in the cmd list
    BS_CMD_END_NUM
} CommandEndFlag;

typedef struct {
    BeamWordNode *p_parent;             // pointer to the parent node
    BeamWordNode *p_current;            // pointer to the current node
    uint32_t score;                     // the accumlulation score untill the current node
    uint16_t current_node_score;        // max word score of the current node
    uint16_t max_score;                 // max score of all nodes
    uint16_t min_score;                 // min socre of all nodes
    int8_t cur_layer;                   // the layer position of the node, -1 for end of the cmd
    uint8_t child_to_find;              // remaining child number that may occurs
    uint16_t continue_frame_count;      // count of the continue frame of the current node(word, phrase)
    uint16_t frame_over_thr;            // count of the continue frame of the current node(word, phrase) with specified threshold
    uint16_t min_frames_over_thr;       // mininum number of the frames over than specifed threshold
    uint16_t continue_zero_count;       // cotinuous zero frame of this node
    uint8_t  allow_overlap;             // 1: allow overlap of parent and child, 0 for not
    uint8_t  cmd_end;                   // 1: end of this cmd
    uint8_t index;                      // for debug only
#ifdef __x86_64    
    uint16_t path[MAX_SUBWORD_NUM_IN_CMD];   // to trace subword ids for debug, dbg cannot display well with uint8_t
#else
    uint8_t path[MAX_SUBWORD_NUM_IN_CMD];   // to trace subword ids 
#endif    
} RunningNode;


typedef struct BeamSearchInstance {
    uint16_t silent_count_cmd_end;      // take the command as end once the continue silent frame count is over than this value
    uint16_t time_out_in_frame_count;   // time out in ms
    uint16_t node_switch_threshold;     // the min frame count of one node
    uint16_t continue_zero_cmd_end;     // take the cmd as end  once continue zero frame count is over than this value
    uint16_t continue_zero_node_end;    // take the node as end  once continue zero frame count is over than this value
    uint16_t max_candidate_num;         // max candidate num
    uint16_t beam_size;                 // beam size
    uint16_t word_min_value;            // minimum value of valid word
    uint16_t min_score_node_keep;       // mininum score to keep this node                           // PHRASE_MIN_SCORE_NODE_KEEP
    uint16_t min_score_node_switch;     // mininum score this node to switch next node               // PHRASE_MIN_SCORE_NODE_KEEP
    
    RunningNode *candidate_all;         // candidates nodes with the current score
    RunningNode *candidate_tops;
    uint16_t *tmp_word_scores;          // temporary memory for sortint word scores
    uint8_t *sorted_index;              // temporary memory for sorting the input from npu word_scores

    int8_t *p_mem_pool;                 // memory address for candidates.
    uint32_t mem_size;                  // memory size

    uint16_t *cmd_thresholds;           // thresholds for whole cmd
    uint16_t *word_thresholds;          // // thresholds for each word(maybe word, phrase,cmd) of GRU out
    uint16_t *word_thresholds_adj;      // pointer to adjust word(phrase/cmd) thresholds to check continue valid frame
    uint16_t *word_thresholds_minus;    // pointer to adjust word(phrase/cmd) thresholds to last node
    uint16_t word_num;                  // word(phrase) number of the model, include <sil>
    uint16_t in_data_size;              // input data size of the beam size in sample, not less than word_num, <sil>...<wordN>[unk]

    uint16_t continue_sil_frame_count;  // continues silent or invalid frame 
    uint16_t valid_frame_start;         // 1: valid frame detected, 0 for else
    uint16_t valid_frame_count;         // frame count since the fist valid frame
    uint16_t valid_candidate_count;     // valid candidates in candidate_tops queue
    
    BeamWordNode *cmd_tree_nodes;       // pointer to nodes tree, placed in order by layer
    uint8_t  *word_last_time;           // pointer to last time of each splitting unit (word/phrase0)
    uint16_t *layer_node_num;           // pointer to node number of tree layers
    uint16_t *layer_offset;             // pointer to offset of each tree layer, start from layer 1 (layer 0 for fake root node , useless now )
    uint16_t cmd_tree_size;             // nodes number of commd tree
    uint16_t max_cmd_length;            // max word(phrase) number to form the comman

    uint16_t work_mode;
    uint16_t cmd_end_sil_threshold;     // threshold to take the cmd as end
    uint16_t data_offset;               // start offset of the valid data from the input data ('word_scores'), in data number    
    uint16_t loss_type;                 // Net trainning loss type
} BeamSearchInstance;


typedef struct BeamSearchInitPara {
    uint16_t frame_offset;              // frame offset(interval) in ms of each input frame
    uint16_t data_offset;               // start offset of the valid data from the input data ('word_scores'), in data number
    uint16_t loss_type;                 // Net trainning loss type
    uint16_t in_data_size;              // input data size of the beam size in sample
    uint16_t *word_thresholds;          // thresholds for each word(maybe word, phrase,cmd) of GRU out
    uint16_t *cmd_thresholds;           // thresholds for whole cmd
    uint8_t *p_private_data;            // private data of this post process
} BeamSearchInitPara;


#ifdef MID_PP_DEBUG
typedef struct BeamSearchCfg {
    int32_t silent_count_cmd_end;       // take the command as end once the continue silent frame count is over than this value
    int32_t time_out_in_frame_count;    // time out in ms
    int32_t node_switch_threshold;      // the min frame count of one node
    int32_t continue_zero_cmd_end;      // take the cmd as end  once continue zero frame count is over than this value
    int32_t continue_zero_node_end;     // take the node as end  once continue zero frame count is over than this value
    int32_t max_candidate_num;          // max candidate num
    int32_t beam_size;                  // beam size
    int32_t word_min_value;             // minimum value of valid word
} BeamSearchCfg;
#endif


#ifdef  __cplusplus
}
#endif // __cplusplus


#endif // _BEAM_SEARCH_INTERNAL_H_

