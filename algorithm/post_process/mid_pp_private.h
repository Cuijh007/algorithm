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
*  @date     2021-08-24
*
*----------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2021/08/24 | 0.1       | Xiaoyu.Ren     | Initial file
*****************************************************************************/
#ifndef _MID_PP_PRIVATE_H_
#define _MID_PP_PRIVATE_H_  

#include "nn_file_struct.h"

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus

#define MID_PP_DEBUG
//#define MID_PP_PRIVATE_DEBUG

#define MIN_SCORE_VALID_FRAME       (9830)      // 0.3*32768
#define CUTOFF_VALUE_VALID_FRAME    (6553)      // 0.2*32768
#define MIN_SCORE_VALID_S2          (26214)     // 0.8*32768
#define EOS_THRESHOLD               (19660)     // 0.6   (26214)     // 0.8 //  (22937)     // 0.7  (19660)     // 0.6
#ifdef MID_PP_DEBUG    
extern int32_t sil_frame_count;
extern int32_t frame_index;
extern int32_t detect_start;
extern int32_t delay;
#endif


typedef enum
{
    SEARCH_END_TYPE_DEFAULT = 0,        // wait until the search method take it as end
    SEARCH_END_TYPE_FORCE,              // force the search to end, will reset the search status
    SEARCH_END_TYPE_WAKEUP,             // for wakeup beam search, return the cmd detected in time, don't reset the status
} SearchEndType;


typedef enum
{
    PP_LOSS_TYPE_MSE = 0,               // Net training with MSE loss
    PP_LOSS_TYPE_CE,                    // Net training with CE loss
    PP_LOSS_TYPE_NUM
} TrainningLossType;


typedef struct MidPpInstance {
    int mode;
    int word_num;
    int with_gru;               // 1: With GRU in model, 0: without GRU
    void *handle;               // handle for default processing routine
    void *handle_ext0;          // handle for extra processing routine, beam handle is set to here always
    int8_t *confirm_cmds;       // pointer to the array for all cmds, 1: the cmd need confirm 0 for not
} MidPpInstance;


void wakeup_beam_search_reset(int model_index);
int32_t wakeup_s1_beam_search(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase,
                             int32_t *cmd_ids, uint32_t *cmd_scores, int32_t cmd_num);
int32_t wakeup_beam_search(int model_index, int16_t *word_scores, int32_t word_num, 
                             int32_t *cmd_ids, uint32_t *cmd_scores, int32_t cmd_num, int32_t frame_num);
int32_t peak_beam_search_start(int model_index, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t cmd_num);
int32_t peak_fst_search_start(int model_index, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores);
int32_t wakeup_search_start(int word_num,  uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t *response_delay);
int32_t wakeup_fst_search_start(int model_index, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores);



#ifdef  __cplusplus
}
#endif // __cplusplus


#endif // _MID_PP_PRIVATE_H_

