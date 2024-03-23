/*****************************************************************************
*  This software is confidential and proprietary and may be used 
*  only as expressly authorized by a licensing agreement from
*                     Intengine.
*            
*              (C) COPYRIGHT 2019 INTENGINE
*                     ALL RIGHTS RESERVED 
*
*  @file     fst_entry.h
*  @brief    wrapper of fst decoder
*  @author   Xiaoyu.Ren
*  @email    xyren@intenginetech.com
*  @version  0.1
*  @date     2021/08/27
*
*----------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2021/08/27 | 0.1       | Xiaoyu.Ren     | Initial file
*****************************************************************************/
#ifndef _FST_ENTRY_H_
#define _FST_ENTRY_H_  
    
#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus


#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "fst_entry_private.h"
#include "mid_common.h"
#include "mid_pp.h"


int32_t fst_dec_init(int model_index, FstInitPara *p_init, MidHandle *mid_pp_inst);
int32_t fst_dec_start(MidHandle handle, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores);


#ifdef  __cplusplus
}
#endif // __cplusplus


#endif // _FST_ENTRY_H_

