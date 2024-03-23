/*****************************************************************************
*  This software is confidential and proprietary and may be used 
*  only as expressly authorized by a licensing agreement from
*                     Intengine.
*            
*              (C) COPYRIGHT 2019 INTENGINE
*                     ALL RIGHTS RESERVED 
*
*  @file     beam_search.c
*  @brief    beam search algorithm
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
*  2020/08/24 | 1.3       | Xiaoyu.Ren     | Added api for usderdefine layer calling
*****************************************************************************/

#include <stdio.h>
#include <string.h>

#include "nn_file_struct.h"

#include "beam_search.h"
#include "peak_search.h"
#include "fst_entry.h"
#include "nn_userdefine.h"
#include "nn_vpu_control.h"

#include "mid_mem.h"
#include "mid_common.h"
#include "mid_pp.h"
#include "mid_pp_private.h"


#define ALIGN_16(x) ((x + 15)&0xFFF0)
#define DPINTF(fmt, ...)    printf(fmt, ##__VA_ARGS__)

#ifdef MID_PP_DEBUG 
extern int32_t frame_index;
#endif

extern MidPpInstance pp_instance[MAX_MODEL_SUPPORTED];

/*
 * API for internal calling
 * do beam search for stage 1 of wakeup userdefine layer
 *
 * parameters:
 * word_scores: score list of each word of one frame
 * word_num: word number of each frame in word_scores array
 * cmd_ids: list of valid command id found
 * cmd_scores: scores of each command, 0 for invalid command
 * cmd_num: expected top n cmd number from the caller
 * 
 */
#define MAX_WAKEUP_OUT_SIZE         (64)
extern int frame_index_to_print ;
int32_t wakeup_s1_beam_search(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase,
                             int32_t *cmd_ids, uint32_t *cmd_scores, int32_t cmd_num)
{
    int ret;
    int16_t __attribute__ ((aligned(4))) word_scores[MAX_WAKEUP_OUT_SIZE];
    LayerParam_t *pLayers = &pNet->pNetInfo->Layers[0];
    LayerParam_t *input = &pLayers[param->inputLayerId[1]];    // softmax layer    
    const int32_t *inputAddr = (const int32_t *)((char*)pBase + param->addr.inputAddr*32);
    int out_size = input->outputShape.H * input->outputShape.W * input->outputShape.C;
    if(input->saveFlag==1)
    {
        if(pNet->pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
        {
            inputAddr = (int32_t *)((char*)VPU_PSRAM_BASE + input->addr.saveResultAddr*64);
        }
        else
        {
            inputAddr = (int32_t *)((char*)pBase + input->addr.saveResultAddr*32);
        }    
    }
    // read the data from vmem to sram for bach1c/bach2c to access by beam_search in int16_t
    // can be removed for General C version or chopin series
    int16_t *p_word_scores = &word_scores[0];
    for(int i=0; i<=out_size/2; i++)
    {
        int32_t value = inputAddr[i];
        *p_word_scores++ = (int16_t)value&0xFFFF;
        *p_word_scores++ = (int16_t)((value>>16)&0xFFFF);
    }

// #define MID_PP_PRIVATE_DEBUG
#ifdef MID_PP_PRIVATE_DEBUG
    iet_print(IET_PRINT_ALG_DBG, "M[%d]F%3d],", model_index, frame_index_to_print);
    int16_t * p_score = word_scores;
    for(int i=0; i<out_size; i++)
    {
        iet_print(IET_PRINT_ALG_DBG, "%6d,", p_score[i]);
    }
    iet_print(IET_PRINT_ALG_DBG, "\r\n");
#endif

    ret = beam_search_start(pp_instance[model_index].handle_ext0, (uint16_t*)word_scores, cmd_ids, cmd_scores, cmd_num, SEARCH_END_TYPE_WAKEUP);

    return ret;
}

void wakeup_beam_search_reset(int model_index)
{
    beam_search_reset(pp_instance[model_index].handle_ext0);
}


/*
 * API for internal calling
 * process buffer frames of K002 model with beam search, to do beam search with the buffer frames for stage 2
 *
 * parameters:
 * word_scores: score list of each word of one frame
 * word_num: word number of each frame in word_scores array
 * cmd_ids: list of valid command id found
 * cmd_scores: scores of each command, 0 for invalid command
 * cmd_num: expected top n cmd number from the caller
 * frame_num: frame number of the input data
 * 
 */
int32_t wakeup_beam_search(int model_index, int16_t *word_scores, int32_t word_num, 
                             int32_t *cmd_ids, uint32_t *cmd_scores, int32_t cmd_num, int32_t frame_num)
{
    int ret;

#ifdef MID_PP_DEBUG //MID_PP_PRIVATE_DEBUG
    {
        int print_number = word_num > 32 ? 32 : word_num;
        iet_print(IET_PRINT_ALG_DBG, "M[%d]F%3d],", model_index, -1);
        for(int i=0; i<print_number; i++)
        {
            iet_print(IET_PRINT_ALG_DBG, "%6d,", i);
    
        }
        iet_print(IET_PRINT_ALG_DBG, "\r\n");
    }
    for(int i=0; i<frame_num; i++)
    {
        int print_number = word_num > 32 ? 32 : word_num;
        iet_print(IET_PRINT_ALG_DBG, "M[%d]F%3d],", model_index, i);
        int16_t * p_score = word_scores + i * ALIGN_16(word_num);
        for(int i=0; i<print_number; i++)
        {
            iet_print(IET_PRINT_ALG_DBG, "%6d,", p_score[i]);
    
        }
        iet_print(IET_PRINT_ALG_DBG, "\r\n");
    }
#endif

    for(int i=0; i<frame_num -1; i++)
    {
        ret = beam_search_start(pp_instance[model_index].handle_ext0, (uint16_t*)word_scores, cmd_ids, cmd_scores, cmd_num, SEARCH_END_TYPE_DEFAULT);
        if(ret == PP_RESULT_VALID_CMD)
        {
            return PP_RESULT_VALID_CMD;
            
        }
        word_scores += ALIGN_16(word_num);  // input score is aligned in 16 (channel)
    }

    ret = beam_search_start(pp_instance[model_index].handle_ext0, (uint16_t*)word_scores, cmd_ids, cmd_scores, cmd_num, SEARCH_END_TYPE_FORCE);

    return ret;    
}

/*
 * API for internal calling
 * do peak seach for GRU out and beam search for AM out
 *
 * parameters:
 * word_scores: out of GRU and am in concated, start with GRU out and aliged to 16
 * cmd_ids: list of valid command id found
 * cmd_scores: scores of each command, 0 for invalid command
 * cmd_num: expected top n cmd number from the caller
 * frame_num: frame number of the input data
 */
int32_t peak_beam_search_start(int model_index, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t cmd_num)
{
#define DELAY_FRAME_NUM     20
#define MAX_CMD_NUM_BEAM    5

    static int32_t double_confirm_frame_count = 0;
    static int32_t valid_cmd_id_peak = 0, valid_cmd_id_beam[MAX_CMD_NUM_BEAM] ={0};
    static uint32_t valid_cmd_scores_peak = 0, valid_cmd_scores_beam[MAX_CMD_NUM_BEAM] = {0};
    static int32_t cmd_valid_beam = 0, cmd_valid_peak = 0;
    
    int32_t peak_ret, beam_ret, reponse_delay;
    int32_t cmd_id_peak=0, cmd_id_beam[MAX_CMD_NUM_BEAM];
    uint32_t cmd_score_peak=0, cmd_score_beam[MAX_CMD_NUM_BEAM];
    
    int32_t end_type = SEARCH_END_TYPE_DEFAULT;
    peak_ret = peak_search_max_score(pp_instance[model_index].handle, word_scores, &cmd_id_peak, &cmd_score_peak, &reponse_delay);
    if(peak_ret == PP_RESULT_VALID_CMD)
    {
#ifdef MID_PP_DEBUG        
        if(pp_instance[model_index].confirm_cmds)
        {
            iet_print(IET_PRINT_ALG_DBG, "peak: f%d %d %d confirm: %d\r\n", frame_index, cmd_id_peak, cmd_score_peak, 
                pp_instance[model_index].confirm_cmds[cmd_id_peak]);
        }
        else
        {
            iet_print(IET_PRINT_ALG_DBG, "peak: f%d %d %d confirm: 0\r\n", frame_index, cmd_id_peak, cmd_score_peak);
        }
#endif        
        // don't need double confirm
        if(pp_instance[model_index].confirm_cmds == NULL ||
            (pp_instance[model_index].confirm_cmds && pp_instance[model_index].confirm_cmds[cmd_id_peak] == 0))
        {
            *cmd_ids = cmd_id_peak;
            *cmd_scores = cmd_score_peak;
            return PP_RESULT_VALID_CMD;            
        }

        if(cmd_valid_peak == 0 && cmd_valid_beam == 0)
        {
            double_confirm_frame_count = 1;
        }
        cmd_valid_peak = 1;
        valid_cmd_id_peak = cmd_id_peak;
        valid_cmd_scores_peak = cmd_score_peak;
        end_type = SEARCH_END_TYPE_FORCE;
    }
    
    beam_ret = beam_search_start(pp_instance[model_index].handle_ext0, word_scores, 
                                &cmd_id_beam[0], &cmd_score_beam[0], 
                                MAX_CMD_NUM_BEAM, end_type);
    if(beam_ret == PP_RESULT_VALID_CMD)
    {
#ifdef MID_PP_DEBUG        
        iet_print(IET_PRINT_ALG_DBG, "beam: f%d %d %d\r\n", frame_index, cmd_id_beam[0], cmd_score_beam[0]);
#endif         
        if(cmd_valid_peak == 0 && cmd_valid_beam == 0)
        {
            double_confirm_frame_count = 1;
        }
        cmd_valid_beam = 1;
        for(int i=0; i<MAX_CMD_NUM_BEAM; i++)
        {
            valid_cmd_id_beam[i] = cmd_id_beam[i];
            valid_cmd_scores_beam[i] = cmd_score_beam[i];
        }       
    }

    // beam result should be gotten before GRU
    if(cmd_valid_peak && cmd_valid_beam)
    {
        // reset search
        cmd_valid_beam = 0;
        cmd_valid_peak = 0;
        double_confirm_frame_count = 0;
        
        for(int i=0; i<MAX_CMD_NUM_BEAM; i++)
        {
            if(valid_cmd_id_beam[i] == valid_cmd_id_peak)
            {
                *cmd_ids = valid_cmd_id_peak;
                *cmd_scores = valid_cmd_scores_peak;
                return PP_RESULT_VALID_CMD;
            }
        }
    }
    else
    {
        // found the cmd by one of the two ways
        if(double_confirm_frame_count)
        {                   
            // found the cmd by one of the two ways
            double_confirm_frame_count++;
            if(double_confirm_frame_count > DELAY_FRAME_NUM)
            {
                // double confirm timeout, reset
                cmd_valid_beam = 0;
                cmd_valid_peak = 0;
                double_confirm_frame_count = 0;
            }
        }
    }
    return PP_RESULT_CHECKING_CMD;

}


int32_t peak_fst_search_start(int model_index, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores)
{
#define PEAK_FST_DELAY_FRAME_NUM 30

    static int32_t double_confirm_frame_count = 0;
    static int32_t cmd_valid_peak = 0;
    static int32_t cmd_valid_fst = 0;
    static int32_t valid_cmd_id_peak = 0;
    static int32_t valid_cmd_id_fst = 0;
    static uint32_t valid_cmd_score_peak = 0;
    static uint32_t valid_cmd_score_fst = 0;
    
    int32_t peak_ret, fst_ret, reponse_delay;
    int32_t cmd_id_peak, cmd_id_fst;
    uint32_t cmd_score_peak, cmd_score_fst;
 
    peak_ret = peak_search_max_score(pp_instance[model_index].handle, word_scores, &cmd_id_peak, &cmd_score_peak, &reponse_delay);
    if(peak_ret == PP_RESULT_VALID_CMD)
    {
#ifdef MID_PP_DEBUG        
        if(pp_instance[model_index].confirm_cmds)
        {
            iet_print(IET_PRINT_ALG_DBG, "peak: f%d %d %d confirm: %d\r\n", frame_index, cmd_id_peak, cmd_score_peak, 
                pp_instance[model_index].confirm_cmds[cmd_id_peak]);
        }
        else
        {
            iet_print(IET_PRINT_ALG_DBG, "peak: f%d %d %d\r\n", frame_index, cmd_id_peak, cmd_score_peak);
        }
#endif        
        if(pp_instance[model_index].confirm_cmds == NULL ||
            (pp_instance[model_index].confirm_cmds && pp_instance[model_index].confirm_cmds[cmd_id_peak] == 0))
        {
            *cmd_ids = cmd_id_peak;
            *cmd_scores = cmd_score_peak;
            return PP_RESULT_VALID_CMD;            
        }
        
        if(cmd_valid_peak == 0 && cmd_valid_fst == 0)
        {
            double_confirm_frame_count = 1;
        }
        cmd_valid_peak = 1;
        valid_cmd_id_peak = cmd_id_peak;
        valid_cmd_score_peak = cmd_score_peak;        
    }  

    fst_ret = fst_dec_start(pp_instance[model_index].handle_ext0, word_scores,  &cmd_id_fst, &cmd_score_fst);
    if(fst_ret == PP_RESULT_VALID_CMD)
    {
#ifdef MID_PP_DEBUG        
        iet_print(IET_PRINT_ALG_DBG, "fst : f%d %d %d\r\n", frame_index, cmd_id_fst, cmd_score_fst);
#endif         
        if(cmd_valid_peak == 0 && cmd_valid_fst == 0)
        {
            double_confirm_frame_count = 1;
        }
        cmd_valid_fst = 1;
        valid_cmd_id_fst = cmd_id_fst;
        valid_cmd_score_fst = cmd_score_fst;
    }

    // fst result should be gotten before GRU
    if(cmd_valid_peak && cmd_valid_fst)
    {
        // reset search
        cmd_valid_fst = 0;
        cmd_valid_peak = 0;
        double_confirm_frame_count = 0;
        if(valid_cmd_id_peak == valid_cmd_id_fst)
        {
            *cmd_ids = valid_cmd_id_peak;
            *cmd_scores = valid_cmd_score_peak;
            return PP_RESULT_VALID_CMD;
        }
    }
    else
    {
        // found the cmd by one of the two ways
        if(double_confirm_frame_count)
        {
            double_confirm_frame_count++;
            if(double_confirm_frame_count > PEAK_FST_DELAY_FRAME_NUM)
            {
                // double confirm timeout, reset
                cmd_valid_fst = 0;
                cmd_valid_peak = 0;
                double_confirm_frame_count = 0;
            }
        }
    }
    
    return PP_RESULT_CHECKING_CMD;

}

/**************************************
* process out of wakeup layer, bypass the result only
* param[in]:
*    handle: handle of the pp instance
*    word_scores: the out of the net, please refere to comments of wakeup_set_output() for details
* param[out]:
*    cmd_ids: id of the cmd gotten
*    cmd_scores: score of the cmd
**************************************/
int32_t wakeup_search_start(int word_num,  uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t *response_delay)
{
#if 0    
    for(int i = 1; i < word_num; i++)
    {
        if(word_scores[i] > 0)
        {
            *cmd_scores = word_scores[i];
            *cmd_ids = i;
            return PP_RESULT_VALID_CMD;
        }
    }
#else
    if(word_scores[0])
    {
        *cmd_ids = word_scores[0];
        *cmd_scores = word_scores[1];
        *response_delay = word_scores[2];
        return PP_RESULT_VALID_CMD;        
    }
#endif
    return PP_RESULT_CHECKING_CMD;

}

int32_t wakeup_fst_search_start(int model_index, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores)
{
#define WAKEUP_FST_DELAY_FRAME_NUM 30

    static int32_t double_confirm_frame_count = 0;
    static int32_t cmd_valid_peak = 0;
    static int32_t cmd_valid_fst = 0;
    static int32_t valid_cmd_id_peak = 0;
    static int32_t valid_cmd_id_fst = 0;
    static uint32_t valid_cmd_score_peak = 0;
    static uint32_t valid_cmd_score_fst = 0;
    
    int32_t peak_ret, fst_ret;
    int32_t cmd_id_peak, cmd_id_fst;
    uint32_t cmd_score_peak, cmd_score_fst;
    int32_t response_delay;
 
    peak_ret = wakeup_search_start(pp_instance[model_index].word_num, word_scores, &cmd_id_peak, &cmd_score_peak, &response_delay);
    if(peak_ret == PP_RESULT_VALID_CMD)
    {
        
        if(cmd_valid_peak == 0 && cmd_valid_fst == 0)
        {
            double_confirm_frame_count = 1;
        }
        cmd_valid_peak = 1;
        valid_cmd_id_peak = cmd_id_peak;
        valid_cmd_score_peak = cmd_score_peak;
    }  

    fst_ret = fst_dec_start(pp_instance[model_index].handle_ext0, word_scores,  &cmd_id_fst, &cmd_score_fst);
    if(fst_ret == PP_RESULT_VALID_CMD)
    {
        if(cmd_valid_peak == 0 && cmd_valid_fst == 0)
        {
            double_confirm_frame_count = 1;
        }
        cmd_valid_fst = 1;
        valid_cmd_id_fst = cmd_id_fst;
        valid_cmd_score_fst = cmd_score_fst;
    }

    // fst result should be gotten before GRU
    if(cmd_valid_peak && cmd_valid_fst)
    {
        // reset search
        cmd_valid_fst = 0;
        cmd_valid_peak = 0;
        double_confirm_frame_count = 0;
#if 1
        if(valid_cmd_id_peak == valid_cmd_id_fst)
        {
            *cmd_ids = valid_cmd_id_peak;
            *cmd_scores = valid_cmd_score_peak;
            return PP_RESULT_VALID_CMD;
        }
#else
        // to test fst
        //if(valid_cmd_id_peak == valid_cmd_id_fst)
        {
            *cmd_ids = valid_cmd_id_fst;
            *cmd_scores = valid_cmd_score_fst;
            return PP_RESULT_VALID_CMD;
        }
#endif
    }
    else
    {
        // found the cmd by one of the two ways
        if(double_confirm_frame_count)
        {
            double_confirm_frame_count++;
            if(double_confirm_frame_count > WAKEUP_FST_DELAY_FRAME_NUM)
            {
                // double confirm timeout, reset
                cmd_valid_fst = 0;
                cmd_valid_peak = 0;
                double_confirm_frame_count = 0;
            }
        }
    }
    
    return PP_RESULT_CHECKING_CMD;

}

