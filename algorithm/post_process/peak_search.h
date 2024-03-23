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
*****************************************************************************/
#ifndef _PEAK_SEARCH_H_
#define _PEAK_SEARCH_H_  
    
#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus


#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "peak_search_private.h"
#include "mid_common.h"
#include "mid_pp_private.h"
#include "mid_pp.h"




int32_t peak_search_init(int model_index, PeakSearchInitPara *p_init, MidHandle *mid_pp_inst);
int32_t peak_search_max_score(MidHandle handle, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t *response_delay);
int32_t peak_search_last_cmd(MidHandle handle, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t *response_delay);
int32_t peak_search_threshold_only(MidHandle handle, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t *response_delay);


#ifdef  __cplusplus
}
#endif // __cplusplus


#endif // _PEAK_SEARCH_H_

