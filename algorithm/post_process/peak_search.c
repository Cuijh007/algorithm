/*****************************************************************************
*  This software is confidential and proprietary and may be used 
*  only as expressly authorized by a licensing agreement from
*                     Intengine.
*            
*              (C) COPYRIGHT 2021 INTENGINE
*                     ALL RIGHTS RESERVED 
*
*  @file     peak_search.c
*  @brief    peak search algorithm
*  @author   Hui.zhang, Xiaoyu.Ren
*  @email    xyren@intenginetech.com
*  @version  1.1
*  @date     2021-05-18
*
*----------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2019/04/07 | 1.0       | Hui.zhang      | Initial file
*  2019/12/28 | 2.0       | Hui.zhang      | Improve method
*  2020/08/29 | 2.1       | Xiaoyu.Ren     | Improve algo to avoid fake cmd
*  2021/05/18 | 2.2       | Xiaoyu.Ren     | Add algo for CE loss
*****************************************************************************/
#include "peak_search.h"
#include "mid_common.h"
#include "mid_mem.h"


#define CMD_WORD_MIN_VALUE              (2000)
#define MIN_PEAK_WAIT_IN_MS             (160)       // wait 0.16s
#define MAX_EOS_CONTINUE_FRAME          (4)         // continue eos frame needed, 4 for VAD, 6 for AM

#define ALIGN_16(x)                     ((x + 15)&0xFFF0)


static PeakSearchInstance peakSearchInst[2];

/**************************************
* peak search init for TF with MSE(mean square error) loss
*
*
**************************************/
int32_t peak_search_init(int model_index, PeakSearchInitPara *p_init, MidHandle *mid_pp_inst)
{
    
    *mid_pp_inst = (MidHandle)&peakSearchInst[model_index];

    memset(&peakSearchInst[model_index], 0, sizeof(PeakSearchInstance));
    peakSearchInst[model_index].word_num = p_init->word_num;
    peakSearchInst[model_index].score_num = p_init->score_num;
    peakSearchInst[model_index].out_size = p_init->out_size;
    peakSearchInst[model_index].with_extra_data = 0;
    peakSearchInst[model_index].extra_data_size = 0;
    if(p_init->out_size > ALIGN_16(p_init->score_num))
    {        
        peakSearchInst[model_index].with_extra_data = 1;
        peakSearchInst[model_index].extra_data_size = p_init->out_size - ALIGN_16(p_init->score_num);
    }

    peakSearchInst[model_index].word_thresholds = p_init->word_thresholds;
    if(p_init->p_private_data)
    {
        PostProcessDataHeader *p_header = (PostProcessDataHeader *)p_init->p_private_data;
        if(p_header->command_delay_offset)
        {
            peakSearchInst[model_index].p_command_delay = p_init->p_private_data + p_header->command_delay_offset;
            if(p_header->command_delay_num != p_init->word_num)
            {
                iet_print(IET_PRINT_ERROR, "warning: net[%d] %s command number[%d] doesn't match with the command delay number [%d]\r\n", 
                    model_index, __FUNCTION__, p_header->command_delay_num,  p_init->word_num);
            }
        }
        peakSearchInst[model_index].extra_label = p_header->extra_label;
    }
    
    iet_print(IET_PRINT_ALG_DBG, "Peak: net[%d] word_num: %d score_num: %d, threshold: %p exra_label: %d\r\n", model_index,  
                    peakSearchInst[model_index].word_num, peakSearchInst[model_index].score_num,
                    peakSearchInst[model_index].word_thresholds,  peakSearchInst[model_index].extra_label
                    );

    return 0;
}

/**************************************
* take the last valid cmd as result
* word_scores: [0, 32767]
*
**************************************/
int32_t peak_search_last_cmd(MidHandle handle, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t *response_delay)
{
    PeakSearchInstance *p_inst = (PeakSearchInstance*)handle;
    
    //find max cmd_scores
    int max_cmd_score = 0;
    int max_all_score = 0;
    int max_cmd_index = 0;

    for(int i=1; i<p_inst->word_num; i++)
    {
        if(word_scores[i] > max_cmd_score)
        {
            max_cmd_score = word_scores[i];
            max_cmd_index = i;
        }
    }

    max_all_score = max_cmd_score;
    for(int i=p_inst->word_num; i<p_inst->score_num; i++)
    {
        if(word_scores[i] > max_all_score)
        {
            max_all_score = word_scores[i];
        }
        
    }    
    // printf("GU%3d:  %4d %6d; ", frame_index_to_print, max_cmd_index, max_cmd_score);

    /*
     * if extra data is proveded together with GRU out, use extra data to check eos,
     * - AM data is provided as extra data
     * - VAD data from GRU out is provided as extra data
     * the process is same for both case: take as EOS if max score is from <sil>
     */
    if(p_inst->with_extra_data)
    {
        int32_t extra_data_offset = ALIGN_16(p_inst->score_num);
        uint16_t *p_data = word_scores + extra_data_offset;
        uint32_t extra_max_id = 0;
        uint32_t extra_max_score = 0;
        for(int i=0; i<p_inst->extra_data_size; i++)
        {
            if(*p_data > extra_max_score)
            {
                extra_max_score = *p_data;
                extra_max_id = i;
            }
            p_data++;       
        }
        // printf("AM:  %4d %6d\n", extra_max_id, extra_max_score);

        if(extra_max_id == 0)
        {
            p_inst->eos_frame_count++;
        }
        else
        {
            p_inst->eos_frame_count = 0;
        }

    }

    // for ctc temporaril
    //word_scores[0] = word_scores[p_inst->word_num];

#ifdef MID_PP_DEBUG
    if(p_inst->with_extra_data 
        && p_inst->eos_frame_count >= MAX_EOS_CONTINUE_FRAME
        && max_all_score < CUTOFF_VALUE_VALID_FRAME
        )
    {
        sil_frame_count++;
    }
    else if(max_all_score < CUTOFF_VALUE_VALID_FRAME)
    {
        sil_frame_count++;
    }
    else
    {
        if(detect_start == 0)
            detect_start = frame_index;
    }
#endif
    p_inst->response_delay++;
    // Find the last command
    if(max_cmd_score > p_inst->word_thresholds[max_cmd_index])
    {   
        if(max_cmd_index == p_inst->active_cmd_id)
        {
            if(max_cmd_score > p_inst->active_cmd_score)
            {
                p_inst->active_cmd_score = max_cmd_score;
                 p_inst->response_delay = 0;
            }
        }
        else
        {
            p_inst->active_cmd_id = max_cmd_index;
            p_inst->active_cmd_score = max_cmd_score;
            p_inst->response_delay = 0;
        }
    }

    // cmd end
    if(p_inst->active_cmd_id)
    {
        int is_cmd_end = 0;
        if(p_inst->extra_label == EXTRA_LABEL_TYPE_EOS)
        {
            if(word_scores[p_inst->word_num] > EOS_THRESHOLD)
            {
                is_cmd_end = 1;
            }
        }
        else if(p_inst->with_extra_data)
        {
            if(p_inst->eos_frame_count >= MAX_EOS_CONTINUE_FRAME 
                && max_all_score < CUTOFF_VALUE_VALID_FRAME)
            {
                is_cmd_end = 1;
            }
        }
        else
        {
            if(max_all_score < CUTOFF_VALUE_VALID_FRAME)
            {
                is_cmd_end = 1;
            }

        }
        if(is_cmd_end)
        {
            int delay_threhold = 0;

            p_inst->delay_frames++;
            if(p_inst->p_command_delay)
            {
                int valid_cmd_id = p_inst->active_cmd_id;
                delay_threhold = p_inst->p_command_delay[valid_cmd_id];
            }
            if(p_inst->delay_frames > delay_threhold)
            {
                *cmd_ids = p_inst->active_cmd_id;
                *cmd_scores = p_inst->active_cmd_score;
                *response_delay = p_inst->response_delay;

        #ifdef MID_PP_DEBUG
                delay = frame_index - detect_start;
        #endif

                p_inst->active_cmd_id = 0;
                p_inst->active_cmd_score = 0;   
                p_inst->delay_frames = 0;
                return PP_RESULT_VALID_CMD;                                      
            }
        }
    }
    else
    {
        *cmd_ids = 0;
        *cmd_scores = 0;        
        return PP_RESULT_CHECKING_CMD;
    }

    return PP_RESULT_CHECKING_CMD;

}


/**************************************
* first: find max cmd_scores for model with CE loss
* second:  if maxscore > 0.5 : start record and find max, else: pass
* word_scores: [0, 32767]
*
**************************************/
int32_t peak_search_max_score(MidHandle handle, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t *response_delay)
{
    PeakSearchInstance *p_inst = (PeakSearchInstance*)handle;
    
    //find max cmd_scores
    int max_cmd_score = 0;
    int max_all_score = 0;
    int max_cmd_index = 0;

    for(int i=1; i<p_inst->word_num; i++)
    {
        if(word_scores[i] > max_cmd_score)
        {
            max_cmd_score = word_scores[i];
            max_cmd_index = i;
        }
    }

    /*
     * if extra data is proveded together with GRU out, use extra data to check eos,
     * - AM data is provided as extra data
     * - VAD data from GRU out is provided as extra data
     * the process is same for both case: take as EOS if max score is from <sil>
     */
    if(p_inst->with_extra_data)
    {
        int32_t extra_data_offset = ALIGN_16(p_inst->score_num);
        uint16_t *p_data = word_scores + extra_data_offset;
        uint32_t extra_max_id = 0;
        uint32_t extra_max_score = 0;
        for(int i=0; i<p_inst->extra_data_size; i++)
        {
            if(*p_data > extra_max_score)
            {
                extra_max_score = *p_data;
                extra_max_id = i;
            }
            p_data++;         
        }
        if(extra_max_id == 0)
        {
            p_inst->eos_frame_count++;
        }
        else
        {
            p_inst->eos_frame_count = 0;
        }

    }
    else
    {
        max_all_score = max_cmd_score;
        for(int i=p_inst->word_num; i<p_inst->score_num; i++)
        {
            if(word_scores[i] > max_all_score)
            {
                max_all_score = word_scores[i];
            } 
        }
    }

    // for ctc temporaril
    //word_scores[0] = word_scores[p_inst->word_num];

#ifdef MID_PP_DEBUG
    if(p_inst->with_extra_data && p_inst->eos_frame_count >= MAX_EOS_CONTINUE_FRAME)
    {
        sil_frame_count++;
    }
    else if(max_all_score < CUTOFF_VALUE_VALID_FRAME)
    {
        sil_frame_count++;
    }
    else
    {
        if(detect_start == 0)
            detect_start = frame_index;
    }
#endif    
    p_inst->response_delay++;
    // find the max score and the corresponding id
    if(max_cmd_score >= p_inst->active_cmd_score)
    {
        p_inst->response_delay = 0;
        
        p_inst->active_cmd_score = max_cmd_score;
        p_inst->active_cmd_id = max_cmd_index;
    }

    // cmd end and possilbe cmd exists
    if(p_inst->active_cmd_id)
    {
        int is_cmd_end = 0;
        if(p_inst->extra_label == EXTRA_LABEL_TYPE_EOS)
        {
            if(word_scores[p_inst->word_num] > EOS_THRESHOLD)
            {
                is_cmd_end = 1;
            }
        }
        else if(p_inst->with_extra_data)
        {
            if(p_inst->eos_frame_count >= MAX_EOS_CONTINUE_FRAME 
                && max_all_score < CUTOFF_VALUE_VALID_FRAME)
            {
                is_cmd_end = 1;
            }
        }        
        else
        {
            if(max_all_score < CUTOFF_VALUE_VALID_FRAME)
            {
                is_cmd_end = 1;
            }
        }

        if(is_cmd_end)
        {
            int valid_cmd_id = p_inst->active_cmd_id;
            int delay_threhold = 0;

            p_inst->delay_frames++;
            if(p_inst->p_command_delay)
            {
                delay_threhold = p_inst->p_command_delay[valid_cmd_id];
            }
            if(p_inst->delay_frames >= delay_threhold)
            {
                if(p_inst->active_cmd_score >= p_inst->word_thresholds[valid_cmd_id])
                {            
                    *cmd_ids = valid_cmd_id;
                    *cmd_scores = p_inst->active_cmd_score;
                    *response_delay = p_inst->response_delay;

                    p_inst->active_cmd_id = 0;
                    p_inst->active_cmd_score = 0;
                    p_inst->delay_frames = 0;

        #ifdef MID_PP_DEBUG
                    delay = frame_index - detect_start;
        #endif
                    return PP_RESULT_VALID_CMD;
                }
                else
                {
                    p_inst->active_cmd_id = 0;
                    p_inst->active_cmd_score = 0;
                    p_inst->delay_frames = 0;
                    return PP_RESULT_CHECKING_CMD;
                }
            }
        }

    }

    return PP_RESULT_CHECKING_CMD;

}



/**************************************
* check the word by the threshold only, don't care the cut off value, for miniNLP
* word_scores: [0, 32767]
*
**************************************/
int32_t peak_search_threshold_only(MidHandle handle, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t *response_delay)
{
    PeakSearchInstance *p_inst = (PeakSearchInstance*)handle;
    
    //find max cmd_scores
    int max_cmd_score = 0;
    int max_all_score = 0;
    int max_cmd_index = 0;

    for(int i=1; i<p_inst->word_num; i++)
    {
        if(word_scores[i] > max_cmd_score)
        {
            max_cmd_score = word_scores[i];
            max_cmd_index = i;
        }
    }
    max_all_score = max_cmd_score;
    for(int i=p_inst->word_num; i<p_inst->score_num; i++)
    {
        if(word_scores[i] > max_all_score)
        {
            max_all_score = word_scores[i];
        }
        
    }

    // for ctc temporaril
    //word_scores[0] = word_scores[p_inst->word_num];

#ifdef MID_PP_DEBUG
    if(max_all_score < CUTOFF_VALUE_VALID_FRAME)
    {
        sil_frame_count++;
    }
    else
    {
        if(detect_start == 0)
            detect_start = frame_index;
    }
#endif

    p_inst->response_delay++;
    // Find the valie command, output the current one if a new one is found
    uint16_t thres = p_inst->word_thresholds[max_cmd_index];
    if(max_cmd_score > thres)
    {
        if(max_cmd_index == p_inst->active_cmd_id)
        {
            if(max_cmd_score > p_inst->active_cmd_score)
            {
                p_inst->active_cmd_score = max_cmd_score;
                p_inst->response_delay = 0;
            }
        }
        else if(p_inst->active_cmd_id)
        {
            *cmd_ids = p_inst->active_cmd_id;
            *cmd_scores = p_inst->active_cmd_score;
            *response_delay = p_inst->response_delay;

            p_inst->active_cmd_id = max_cmd_index;
            p_inst->active_cmd_score = max_cmd_score;
            p_inst->response_delay = 0;
            //iet_printf_msg("switch: cmd %d %d [%d]\r\n", *cmd_ids, *cmd_scores, thres);
            return PP_RESULT_VALID_CMD;
        }
        else
        {
            p_inst->active_cmd_id = max_cmd_index;
            p_inst->active_cmd_score = max_cmd_score;
            p_inst->response_delay = 0;           
        }
    }

    // cmd end
    if(p_inst->active_cmd_id)
    {
        int is_cmd_end = 0;
        if(p_inst->extra_label == EXTRA_LABEL_TYPE_EOS)
        {
            if(word_scores[p_inst->word_num] > EOS_THRESHOLD)
            {
                is_cmd_end = 1;
            }
        }
        else
        {
            if(max_all_score < CUTOFF_VALUE_VALID_FRAME)
            {
                is_cmd_end = 1;
            }

        }
        if(is_cmd_end)
        {
            *cmd_ids = p_inst->active_cmd_id;
            *cmd_scores = p_inst->active_cmd_score;
            *response_delay = p_inst->response_delay;

            p_inst->active_cmd_id = 0;
            p_inst->active_cmd_score = 0;
            //iet_printf_msg("curoff: cmd %d %d [%d]\r\n", *cmd_ids, *cmd_scores, thres);
            return PP_RESULT_VALID_CMD;
        }
    }
    else
    {
        *cmd_ids = 0;
        *cmd_scores = 0;        
        return PP_RESULT_CHECKING_CMD;
    }

    return PP_RESULT_CHECKING_CMD;

}

