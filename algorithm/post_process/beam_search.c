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
*  2020/10/13 | 1.3       | Xiaoyu.Ren     | version for whole cmmand/split phrase/split word
*****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "beam_common.h"
#include "beam_search.h"

#include "mid_mem.h"
#include "mid_common.h"
#include "mid_pp.h"
#include "mid_pp_private.h"


#define BS_MAX(a, b)      ((a) > (b) ? (a) : (b))  
#define BS_MIN(a, b)      ((a) < (b) ? (a) : (b))  


BeamWordNode root_node ={0, 1, 1, 1};
BeamWordNode zero_node ={0, 0, 0, 0};    // its data shoul be used only

void beam_search_reset(MidHandle handle)
{    
    int32_t i;
    BeamSearchInstance *p_inst = (BeamSearchInstance*)handle;
    BeamWordNode *p_node = p_inst->cmd_tree_nodes;

    for(i = 0; i< p_inst->cmd_tree_size; i++ )
    {
        p_node->chance = 1;
        p_node++;        
    }
    memset(p_inst->p_mem_pool, 0, p_inst->mem_size);

    p_inst->valid_frame_count = 0;
    p_inst->continue_sil_frame_count = 0;
    p_inst->valid_candidate_count = 0;
    p_inst->valid_frame_start = 0;


    // for debug
#ifdef MID_PP_DEBUG       
    detect_start = 0;
#endif
    
}


/*
 * Sort the word_scores as per score from the big one to the small one, exclude the silent(index 0) and keep it as it is
 * Store the sorted cmd index to parameter "sorted_index", the size of "sorted_index" must be not less than MAX_WORD_NUM
 *
 * parameters:
 * word_scores: scores of npu word_scores in word index order, doesn't include the silent(index=0)
 * sum_len: valid scores number
 * sort_len: top n wanted (n = sort_len)
 * sorted_index: the sorted word index, only top n valid
 * 
 */
void sort_word_scores(MidHandle handle, uint16_t *word_scores, uint8_t *sorted_index, int32_t sum_len, int32_t sort_len)
{
    int32_t i, j;
    uint16_t tmp_score;
    uint8_t tmp_index;
    int32_t isChanged;
    int32_t valid_sort_len = sort_len;
    BeamSearchInstance *p_inst = (BeamSearchInstance*)handle;
    uint16_t *p_tmp_scores = p_inst->tmp_word_scores;

 
    if(sort_len > sum_len)
    {
        valid_sort_len = sum_len-1;
    }

    // don't change the orignal input, it's useful for beam search
    for(i=0; i<sum_len; i++)
    {
        p_tmp_scores[i] = word_scores[i];
        sorted_index[i] = i;                  
    }
   
    // don't touch the index 0, it's for silent
    for(i=1; i<valid_sort_len+1; i++)
    {
        isChanged = 0;
        for(j=sum_len-1; j>i; j--)
        {
            if(p_tmp_scores[j] > p_tmp_scores[j-1])
            {
                tmp_score = p_tmp_scores[j-1];
                p_tmp_scores[j-1] = p_tmp_scores[j];
                p_tmp_scores[j] = tmp_score;

                tmp_index = sorted_index[j-1];
                sorted_index[j-1] = sorted_index[j];
                sorted_index[j] = tmp_index; 

                isChanged = 1;
            }            
        }
        if(isChanged == 0)
        {
            break;
        }
    }

    
}

/*
 * Sort candidates by scores in place, from the biggest one to the small one
 *
 * parameters:
 * nodes: candidate nodes
 * sum_len: valid candidate number
 * sort_len: top n wanted (n = sort_len), MUST make sure that sum_len>=sort_len
 * 
 */
void sort_candidates(RunningNode* nodes, int32_t sum_len, int32_t sort_len)
{
    int32_t i, j;
    RunningNode tmp;
    int32_t ischanged = 0;

    int32_t valid_len = sum_len < sort_len ? sum_len : sort_len;
       
    for(i=0; i<valid_len; i++)
    {
        ischanged = 0;
        for(j=sum_len-1; j>i; j--)
        {
            if(nodes[j].score > nodes[j-1].score)
            {
                tmp = nodes[j-1];
                nodes[j-1] = nodes[j];
                nodes[j] = tmp;
                ischanged = 1;
            }            
        }
        
        if(ischanged == 0)
            break;
    }    
    
}


/*
 * Get the beam search result from the candidates
 *
 * parameters:
 * index: pointer to store the cmd indexs of gotten by beam search
 * score: pointer to store the the scores related to the index
 * num: top n wanted 
 *
 * Return value: the real valid results
 * 
 */
int32_t get_beam_result(MidHandle handle, int32_t *cmd_ids, uint32_t *cmd_score, int32_t num)
{
    int32_t i;
    int32_t *p_index = cmd_ids;
    uint32_t *p_score = cmd_score;
    int32_t count = 0;
    RunningNode * p_running;
    BeamWordNode *p_current;
    BeamSearchInstance *p_inst = (BeamSearchInstance*)handle;
    int cmd_end  = 0;

    if(p_inst->valid_candidate_count == 0)
    {
        return 0;
    }


    if(p_inst->work_mode == BS_MODE_COMMAND)
    {
        // select the cmd with max score from the first two candidatess
        // provide one result only now for command mode
        uint32_t max_score = 0;
        uint32_t result_index = 0;
        int search_count = p_inst->valid_candidate_count > 3 ? 3 : p_inst->valid_candidate_count;
        p_running = p_inst->candidate_tops;
        for(i=0; i<search_count; i++)
        {
            p_current = p_running->p_current;
            if(p_running->current_node_score >= p_inst->cmd_thresholds[p_current->data])
            {
                if(p_running->current_node_score > max_score)
                {
                    result_index = p_current->first_child;
                    max_score = p_running->current_node_score;
                }
            }
            p_running++;
        }

        if(max_score)
        {
            *p_index++ = result_index;
            *p_score++ = max_score;
            count = 1;            
        }
    }
    else if(p_inst->work_mode == BS_MODE_PHRASE)
    {
        p_running = p_inst->candidate_tops;
        p_current = p_running->p_current;
        if(/*p_running->cur_layer == -1 ||*/ p_running->cmd_end == BS_CMD_END_TRUE)
        {
            int cmd_found = 0;
            if(p_running->max_score < p_running->current_node_score)
                p_running->max_score = p_running->current_node_score;
            if(p_running->min_score > p_running->current_node_score)
                p_running->min_score = p_running->current_node_score;             
            if(p_inst->cmd_thresholds)
            {
                // compare the max score of all nodes with the whold cmd threshold
                // other ways can be used also here, such min_score, sum score
                // update min_score here if it's used              
                if(p_running->max_score >= p_inst->cmd_thresholds[p_current->first_child])
                {
                    cmd_found = 1;
                }
            }
            else
            {
                // compare the score of the last node with its threshold
                // other kept node's score should be not less than its threshold
                // when come here
                if(p_running->current_node_score >= p_inst->word_thresholds[p_current->data])
                    cmd_found = 1;
            }
            
            if(cmd_found)
            {
#ifndef  __x86_64               
                // for split word case only
                // send sub words ids to app, each subword hold a place of cmd
                // score set to the cmd score for each subword, change this if necessary
                // 'num' should be lesss than MAX_SUBWORD_NUM_IN_CMD
                for(int j=0; j<num; j++)
                {
                    *p_index++ = p_running->path[j];                 
                    if(p_running->path[j] == 0)
                    {
                        break;
                    }
                    *p_score++ = p_running->max_score;                    
                    count++;
                }

#else           
#ifdef MID_PP_DEBUG_1
                iet_print(IET_PRINT_ALG_DBG, "BS Result: %d = ", p_current->first_child);
                for(int j=0; j<MAX_SUBWORD_NUM_IN_CMD; j++)
                {
                    if(p_running->path[j])
                    {
                        iet_print(IET_PRINT_ALG_DBG, "%d +", p_running->path[j]); 
                    }
                }
                iet_print(IET_PRINT_ALG_DBG, "0\r\n");             
#endif     
                *p_index++ = p_current->first_child;
                *p_score++ = p_running->max_score;
                p_running->cmd_end = BS_CMD_END_REPORTED;
                count++;
#endif
            }
        }    
    }
    else
    {
        p_running = p_inst->candidate_tops;
        for(i=0; i<p_inst->valid_candidate_count; i++)
        {
            p_current = p_running->p_current;
            if(p_running->cmd_end ||
                (p_running->cur_layer == -1 && p_running->continue_frame_count >= PHRASE_NODE_SWITCH_MIN_FRAME))
            {
                *p_index++ = p_current->first_child;
                *p_score++ = p_running->current_node_score;
                count++;
                if(count >= num)
                    break;

             }
      
            p_running++;
        }  
    }
    
    if(count<num)
    {
        for(i=count; i<num; i++)
        {
            cmd_ids[i] = 0;
            cmd_score[i] = 0;
        }
    }

#ifdef MID_PP_DEBUG
    if(cmd_ids[0])
    {
        delay = p_inst->valid_frame_count - detect_start;
    }
#endif
    return count;
    
}

/*
 * Get the beam search result from the candidates
 *
 * parameters:
 * index: pointer to store the cmd indexs of gotten by beam search
 * score: pointer to store the the scores related to the index
 * num: top n wanted 
 *
 * Return value: the real valid results
 * 
 */
int32_t is_cmd_ready(MidHandle handle)
{
    int32_t i;
    int32_t valid_count;
    RunningNode * p_running;
    BeamSearchInstance *p_inst = (BeamSearchInstance*)handle;

    if(p_inst->valid_candidate_count == 0)
    {
        return 0;
    }

    valid_count = p_inst->valid_candidate_count;
  
    if(p_inst->work_mode == BS_MODE_COMMAND /*|| p_inst->work_mode == BS_MODE_PHRASE*/)
        valid_count = 1;        

    p_running = p_inst->candidate_tops;
    for(i=0; i<valid_count; i++)
    {   
        // For command mode, all command in queue can be taken as end alreay, 'cmd_end' doesn't flag it exactly
        if(p_running->cmd_end == BS_CMD_END_TRUE ||
            p_inst->work_mode == BS_MODE_COMMAND /*||
            (p_running->cur_layer == -1 && p_running->continue_frame_count >= PHRASE_NODE_SWITCH_MIN_FRAME)*/ )
        {
            return 1;
        }
        
       p_running++;
    }  

    return 0;
    
}


/*
 * Get the real valid candidate count
 * a reported candidate is still in queue for searching implementation reason,
 * but it's invalid one here
 * parameters:
 *
 * Return value: the real valid candidate number
 * 
 */
int32_t valid_candidate_count(MidHandle handle)
{
    int32_t valid_count;
    RunningNode * p_running;
    BeamSearchInstance *p_inst = (BeamSearchInstance*)handle;

    if(p_inst->valid_candidate_count == 0)
    {
        return 0;
    }

    valid_count = p_inst->valid_candidate_count;
    p_running = p_inst->candidate_tops;
    for(int i=0; i<p_inst->valid_candidate_count; i++)
    {   
        // For command mode, all command in queue can be taken as end alreay, 'cmd_end' doesn't flag it exactly
        if(p_running->cmd_end == BS_CMD_END_REPORTED)
        {
            valid_count--;
        }
        
       p_running++;
    }  

    return valid_count;
    
}


/*
 * process the input frame
 *
 * parameters:
 * word_scores: score list of each word of one frame
 * num: valid score number
 * 
 * return value: 1 - valid command found, 0 - need to check more frame
 *
 */
int32_t beam_search_process(MidHandle handle, uint16_t *word_scores)
{
    BeamSearchInstance *p_inst = (BeamSearchInstance*)handle;

    int32_t i, j, k;
    int32_t word_index;
    BeamWordNode *p_parent = NULL;
    BeamWordNode *p_current = NULL;
    BeamWordNode *p_child = NULL;
    BeamWordNode *p_child_layer = NULL;

    RunningNode *p_running;
    RunningNode *p_candidate;
        
    int32_t all_index = 0;
    int32_t cur_index = 0;

#ifdef MID_PP_DEBUG
    int32_t x;
    if(frame_index == 96)
    {
        // iet_print(IET_PRINT_ALG_DBG, "start debug fake voice\n");
    }
#endif    


    // set silent scores to zero for special node '0'
    word_scores[0] = 0;

    all_index = 0;
    p_running = p_inst->candidate_tops;
    p_candidate = p_inst->candidate_all;
    for(i=0; i<p_inst->valid_candidate_count; i++)
    {
        p_parent = p_running->p_parent;
        p_current = p_running->p_current;
        
        if(p_current->child_num == 0)           
        {
            if(!p_running->cmd_end)
            {
                int32_t current_score ;
                int32_t real_word_index = p_current->data;

                // for special '0' node, use its parent to check the end instead of itself since it's always zero.
                if(p_current->data == 0)
                {
                    real_word_index = p_parent->data;
                    
                }
                current_score = word_scores[real_word_index];
                
                if(current_score == 0)
                {
                   p_candidate->continue_zero_count = p_running->continue_zero_count + 1;
                   if(p_candidate->continue_zero_count >= p_inst->continue_zero_cmd_end)
                   {   
                        int drop = 0;
                       // timeout to allow build new cmd, reset the related node
                       // keep the status untill the command is released, so don't release the leaf node.
                       // p_current->chance = 1; 
                       p_parent->chance = 1;
                       p_running->cmd_end = BS_CMD_END_TRUE;
                       if(p_inst->work_mode == BS_MODE_COMMAND)
                       {
                            // if(0): Don't drop any to keep behavior with that of peak search when application ajdusts threshold
                            // Enable this will help accuracy a little for some cases, but very small
                            //if(0)//p_running->current_node_score < p_inst->word_thresholds[real_word_index])
                            if(p_running->current_node_score < p_inst->word_thresholds_minus[real_word_index])
                            {
                                drop =1;
                            }
                        
                       }
                       else if (p_inst->work_mode == BS_MODE_PHRASE)
                       {  
                            if(p_running->continue_frame_count < p_inst->word_last_time[real_word_index] ||
                                p_running->current_node_score < p_inst->word_thresholds_minus[real_word_index] ||
                               (p_running->current_node_score < p_inst->word_thresholds[real_word_index] && 
                                p_running->frame_over_thr < p_running->min_frames_over_thr ) 
                                )                        
                            {
                                drop = 1;
                            }
                       }
                       else //if(p_inst->work_mode == BS_MODE_WORD)
                       {
                            if((p_running->current_node_score < MIN_SCORE_VALID_FRAME && p_running->frame_over_thr < p_running->min_frames_over_thr*3/2)  ||
                                p_running->frame_over_thr < p_running->min_frames_over_thr*4/5)
                            {
                                drop = 1;
                            }
                        
                       }
                 
                       if(drop)
                       {
                            // drop the invalid command
                            // for  special node '0', don't reset its change flag to avoid that it's added to the waitlist again after drop
                            if(p_current->data)
                            {
                                p_current->chance = 1;
                            }
                            p_running++;
                            continue;
                       }
                   }                 
                   p_candidate->continue_frame_count = p_running->continue_frame_count;
                   p_candidate->frame_over_thr = p_running->frame_over_thr;
                }
                else
                {   
                    p_candidate->continue_zero_count = 0;
                    p_candidate->continue_frame_count = p_running->continue_frame_count + 1;
                    if(current_score >= p_inst->word_thresholds_adj[real_word_index])
                    {                        
                        p_candidate->frame_over_thr = p_running->frame_over_thr + 1;
                    }
                    else
                    {
                        p_candidate->frame_over_thr = p_running->frame_over_thr;
                    }
                }

                p_candidate->current_node_score = current_score > p_running->current_node_score ? current_score : p_running->current_node_score;
                // p_candidate->max_score = current_score > p_running->max_score ? current_score : p_running->max_score;

                p_candidate->p_parent = p_parent;
                p_candidate->p_current = p_current;
                p_candidate->cur_layer = -1;
                p_candidate->score  = p_running->score + word_scores[p_parent->data] + word_scores[p_current->data];
                p_candidate->child_to_find = 0;
                p_candidate->cmd_end = p_running->cmd_end;
                p_candidate->min_frames_over_thr = p_running->min_frames_over_thr;
                //candidate_all[all_index].allow_overlap = p_running->allow_overlap;
            }
            else
            {
                *p_candidate = *p_running;
            }
            
            // to trace teh path of the subword
#ifdef ENABLE_SUBWORD_PATH           
            p_candidate->index = p_current->data;
            for(x=0; x<p_inst->max_cmd_length; x++)
            {
                p_candidate->path[x] = p_running->path[x];
            }
#endif            

            all_index++;
            if(all_index > p_inst->max_candidate_num)
            {
                goto beam_end;
            }  
            p_candidate++;
        }
        else
        {
            // find its new child
            // overlap between words is allowed for word_splitted case, not for phrase_splitted case
            int32_t cur_layer = p_running->cur_layer;
            int32_t switch_node = 0;
            if(p_inst->work_mode == BS_MODE_WORD)
            {                
                // The continue frame condition is to make sure that the node has enough valid frames(don't consider the score threshold)
                // The node without enough valid frame will be dropped by the node keeping check
                if( p_running->continue_frame_count >=  (p_inst->word_last_time[p_current->data]>>2) &&
                    p_running->current_node_score >= p_inst->min_score_node_switch)
                {
                    switch_node = 1;
                }

            }
            else if(p_inst->work_mode == BS_MODE_PHRASE)
            {
                // The continue frame condition is to make sure that the node has enough valid frames(don't consider the score threshold)
                // The node without enough valid frame will be dropped by the node keeping check
                // and make sure the current node score is valid one before switching to a new node
                if( p_running->continue_frame_count >= PHRASE_NODE_SWITCH_MIN_FRAME &&
                    p_running->current_node_score >= p_inst->min_score_node_switch)
                {
                    switch_node = 1;
                }
                
            }

            if(switch_node)
            {
                p_child_layer = p_inst->cmd_tree_nodes + p_inst->layer_offset[cur_layer+1];

                // add new children as candidates or else keep it as it is.
                p_child = p_child_layer + p_current->first_child;
                for(j=0; j<p_current->child_num; j++)
                {
                    if(p_child->chance == 0)
                    {
                        // The child has found it parent - the current node in this running node
                        // and work as a running mode also
                        p_child++;
                        continue;                    
                    }

                    // a new child was found
                    // for special node '0', switch after the end of the current node
                    if((word_scores[p_child->data] && word_scores[p_child->data] >= word_scores[p_current->data])|| 
                        (p_child->data == 0 && word_scores[p_current->data] == 0))
                    {   
                        int32_t allow_overlap = 1;
                        
                        // restore the state of the parent, allow it to build new cmd
                        p_parent->chance = 1;                        
                        p_child->chance = 0;
                        if(p_running->allow_overlap)
                        {
                            p_candidate->p_parent = p_current;
                        }
                        else
                        {
                            p_candidate->p_parent = &zero_node;
                        }
                       
                        p_candidate->p_current = p_child;
                        p_candidate->cur_layer = p_running->cur_layer + 1;
                        p_candidate->child_to_find = p_child->child_num;
                        p_candidate->cmd_end = BS_CMD_END_NONE;
                        
                        if(p_child->data == 0)
                        {
                            // for special node '0', keep its parent's data
                            // the parent data will be used by result check
                            p_candidate->p_parent = p_current;
                            p_candidate->score  = p_running->score ;
                            p_candidate->current_node_score = p_running->current_node_score;
                            p_candidate->continue_frame_count = p_running->continue_frame_count;
                            p_candidate->frame_over_thr = p_running->frame_over_thr;
                            p_candidate->min_frames_over_thr  = p_running->min_frames_over_thr;
                            p_candidate->continue_zero_count = p_running->continue_zero_count+1; 
                        }
                        else
                        {
                            int32_t current_score = word_scores[p_child->data];
                            p_candidate->score  = p_running->score + word_scores[p_candidate->p_parent->data] + current_score;
                            p_candidate->max_score = p_running->current_node_score > p_running->max_score ? p_running->current_node_score : p_running->max_score;
                            p_candidate->min_score = p_running->current_node_score < p_running->min_score ? p_running->current_node_score : p_running->min_score;                            
                            p_candidate->current_node_score = current_score;
                            p_candidate->continue_zero_count = 0; 
                            p_candidate->continue_frame_count = 1;
                            if(current_score >= p_inst->word_thresholds_adj[p_child->data])
                            {                        
                                p_candidate->frame_over_thr = p_running->frame_over_thr+1;
                            }
                            else
                            {
                                p_candidate->frame_over_thr = p_running->frame_over_thr;
                            }
                            p_candidate->min_frames_over_thr = p_running->min_frames_over_thr + p_inst->word_last_time[p_child->data];
                        }

                        p_running->child_to_find -= 1;


                       
                        // mark this node as the last node of the command if it hasn't child 
                        // so the search can end anytime if its score matches the setting threshold
                        // for special node '0', its child_num is zero also, and cannot confirm whether it's expected command,
                        // need more frame to compare with its sibling
                        if(p_child->child_num == 0)
                        {
                            p_candidate->cur_layer = -1;
                            p_candidate->child_to_find  = 0;
                        }                                                    
                        else // reset its state for new node
                        {
                            BeamWordNode *p_grandson_layer;
                            BeamWordNode *p_grandson;
                                                        
                            p_grandson_layer = p_inst->cmd_tree_nodes + p_inst->layer_offset[p_candidate->cur_layer+1];
                            p_grandson = p_grandson_layer + p_child->first_child;                            
                            for(k=0; k<p_child->child_num; k++)
                            {
                                if(p_child->data == p_grandson->data)
                                {
                                    allow_overlap = 0;
                                }
                                p_grandson->chance = 1;
                                p_grandson++;                                
                            }
                        }
                        p_candidate->allow_overlap = allow_overlap;

                        // to trace teh path of the subword
#ifdef ENABLE_SUBWORD_PATH                       
                        p_candidate->index = p_candidate->p_current->data;
                        for(x=0; x<p_inst->max_cmd_length; x++)
                        {
                            p_candidate->path[x] = p_running->path[x];
                        }
                        p_candidate->path[p_running->cur_layer + 1] = p_child->data;
#endif                        

                                           
                        all_index++;
                        if(all_index > p_inst->max_candidate_num)
                        {
                            goto beam_end;
                        }
                        p_candidate++;

                    }
                    p_child++;                    
                } 

            }

            // keep the node if not all child are found
            if(p_running->child_to_find )
            {                
                int32_t keep_node = 1;
                int32_t current_score;
                int32_t real_word_index = p_current->data;

                // for special '0' node, use its parent to check the end instead of itself since it's always zero.
                if(p_current->data == 0)
                {
                    real_word_index = p_parent->data;
                    
                }
                current_score = word_scores[real_word_index];
                
                if(current_score == 0)                
                {
                    int drop = 0;
                    p_candidate->continue_zero_count = p_running->continue_zero_count + 1;
                    if(p_inst->work_mode == BS_MODE_WORD)
                    {
                        if(p_candidate->continue_zero_count >= p_inst->continue_zero_node_end ||    // Timeout to find child node
                            (p_candidate->continue_zero_count >= (p_inst->continue_zero_node_end>>2) && 
                            (p_running->continue_frame_count < (p_inst->word_last_time[real_word_index]>>1) ||
                            p_running->current_node_score < p_inst->min_score_node_keep)
                        ))
                        {
                            drop =1;
                        }

                    }
                    else
                    {
                        if(p_candidate->continue_zero_count >= p_inst->continue_zero_node_end ||    // Timeout to find child node
                            (p_candidate->continue_zero_count >= (p_inst->continue_zero_node_end>>2) && 
                            (p_running->continue_frame_count < (p_inst->word_last_time[real_word_index]>>1) ||
                            p_running->current_node_score < p_inst->min_score_node_keep))
                            )
                        {
                            drop =1;
                        }                        
                    }

                    if(drop)
                    {
                        p_current->chance = 1;
                        p_parent->chance = 1;
                        keep_node = 0;
                    }
                    p_candidate->continue_frame_count = p_running->continue_frame_count;
                    p_candidate->frame_over_thr = p_running->frame_over_thr;
                }
                else
                {
                    p_candidate->continue_zero_count = 0;
                    p_candidate->continue_frame_count = p_running->continue_frame_count + 1;
                    if(current_score >= p_inst->word_thresholds_adj[real_word_index])
                    {
                        p_candidate->frame_over_thr = p_running->frame_over_thr + 1;
                    }
                    else
                    {
                        p_candidate->frame_over_thr = p_running->frame_over_thr;
                    }
                }

                if(keep_node)
                {
                    p_candidate->p_parent = p_parent;
                    p_candidate->p_current = p_current;
                    p_candidate->cur_layer = p_running->cur_layer;
                    p_candidate->score  = p_running->score + word_scores[p_parent->data] + word_scores[p_current->data];
                    p_candidate->child_to_find = p_running->child_to_find;                
                    p_candidate->allow_overlap = p_running->allow_overlap;                    
                    p_candidate->cmd_end = p_running->cmd_end;
                    p_candidate->current_node_score = current_score > p_running->current_node_score ? current_score : p_running->current_node_score;
                    p_candidate->min_frames_over_thr = p_running->min_frames_over_thr;
                    
                    
                    // to trace teh path of the subword
#ifdef ENABLE_SUBWORD_PATH                   
                    p_candidate->index = p_candidate->p_current->data;
                    for(x=0; x<p_inst->max_cmd_length; x++)
                    {
                        p_candidate->path[x] = p_running->path[x];
                    }                    
                    p_candidate->path[p_candidate->cur_layer] = p_current->data;
#endif
                    
                    all_index++;
                    if(all_index > p_inst->max_candidate_num)
                    {
                        goto beam_end;
                    }
                    p_candidate++;
                }
            }
            else
            {
                // drop the running node if all child have been found.
            }
           
        }
        p_running++;
    }


    if(word_scores[p_inst->sorted_index[1]])        // skip the frame if the word score is zero, not include silent
    {
        // skip the silent index
        cur_index = all_index;
        for(i=1; i<p_inst->beam_size; i++)
        {
            // find the node index            
            word_index = p_inst->sorted_index[i];
            if(word_scores[word_index] != 0)
            {
                RunningNode *p_tmp_node;
                BeamWordNode *p_current;
                int32_t new_node = 1;
                p_tmp_node = p_inst->candidate_all;
                for(j=0; j<cur_index; j++)
                {
                    p_current = p_tmp_node->p_current;
                    if(p_current->data == word_index && 
                        (p_tmp_node->cur_layer == 0|| p_inst->work_mode == BS_MODE_COMMAND)) // don't care cur_layer for BS_MODE_COMMAND
                    {
                        new_node = 0;
                        break;                    
                    }
                    p_tmp_node++;
                    
                }

                if(new_node)
                {
                    int32_t allow_overlap = 1;
                    for(j=0; j<p_inst->layer_node_num[0]; j++)
                    {
                        p_current = &(p_inst->cmd_tree_nodes[j]);
                        if(p_current->data == word_index && p_current->chance == 1)
                        {
                            p_current->chance = 0;               // set flag to IN-USING state
                            p_candidate->p_parent = &root_node;
                            p_candidate->p_current = p_current;
                            p_candidate->cur_layer = 0;
                            p_candidate->score = word_scores[word_index];
                            p_candidate->current_node_score = word_scores[word_index];
                            p_candidate->max_score = word_scores[word_index];
                            p_candidate->min_score = 32767;
                            p_candidate->child_to_find = p_current->child_num;
                            p_candidate->continue_zero_count = 0;
                            p_candidate->continue_frame_count = 1;
                            if(word_scores[word_index] >= p_inst->word_thresholds_adj[word_index])
                            {
                                p_candidate->frame_over_thr = 1;
                            }
                            else
                            {
                                p_candidate->frame_over_thr = 0;
                            }

                            p_candidate->min_frames_over_thr = p_inst->word_last_time[word_index];
                            
                            p_child_layer = p_inst->cmd_tree_nodes + p_inst->layer_offset[1];
                            p_child = p_child_layer + p_current->first_child;                            
                            for(k=0; k<p_current->child_num; k++)
                            {
                                if(p_current->data == p_child->data)
                                {
                                    // For word split, don't allow overlap between words when they are same
                                    // or else the word score will be double incorrectly. 
                                    // for example: ding shi shi fen zhong
                                    // model cannot split two "shi" well, maybe out as one word
                                    allow_overlap = 0;
                                }
                                p_child->chance = 1;

#if 0
                                // Not a valid child if the child has occurs for some frames
                                // Use bigger threshold instead of 0 to make sure that the child already exists for some frames
                                // and exclude of special child '0'
                                // Noise of the child will introduced error decision if 0 is used.
                                if(word_scores[p_child->data] > PHRASE_MIN_SCORE_CHILD_CHECK)
                                    p_child->chance = 0;
#endif                                

                                p_child++;                                
                            }
                            p_candidate->allow_overlap = allow_overlap;                            
                            p_candidate->cmd_end = BS_CMD_END_NONE;
                    
                            // to trace teh path of the subword
#ifdef ENABLE_SUBWORD_PATH
                            p_candidate->index = p_current->data;
                            for(x=0; x<p_inst->max_cmd_length; x++)
                            {
                                p_candidate->path[x] = 0;
                            }
                            p_candidate->path[0] = p_current->data;
#endif                    
                            
                            all_index++;
                            if(all_index > p_inst->max_candidate_num)
                            {
                                goto beam_end;
                            }
                            p_candidate++;
                            break;
                        }
                    }
                }
            }
            else
            {
                break;
            }
        }
    }
 

beam_end:    
              
    sort_candidates(p_inst->candidate_all, all_index, p_inst->beam_size);

    p_inst->valid_candidate_count = all_index<p_inst->beam_size ? all_index : p_inst->beam_size;
    for(i=0; i<p_inst->valid_candidate_count; i++)
    {
        p_inst->candidate_tops[i] = p_inst->candidate_all[i];
    }

    // release the node to re-entyr since the cmd has been removed from the candidates
    for(i=p_inst->valid_candidate_count; i<all_index; i++)
    {
        p_inst->candidate_all[i].p_parent->chance = 1;
        p_inst->candidate_all[i].p_current->chance = 1;
    }

    if(p_inst->valid_candidate_count)
    {
        p_inst->valid_frame_start = 1;

        // for debug
#ifdef MID_PP_DEBUG        
        if(detect_start == 0)
            detect_start = p_inst->valid_frame_count;
#endif        

        switch(p_inst->work_mode)
        {
            case BS_MODE_COMMAND:
            case BS_MODE_PHRASE:
            case BS_MODE_WORD:
                p_running = p_inst->candidate_tops;
                if(p_running->cmd_end == BS_CMD_END_TRUE)
                {
                    if(p_inst->valid_candidate_count == 1 ||
                        (p_inst->valid_candidate_count != 1 && (p_running[0].score >>4) > (p_running[1].score)))
                    {
                        return 1;
                    }
                }
                break;

        }    
    }

    return 0;

    
}

/*
 * API to application
 * initialization of beam search
 *
 * parameters:
 * work_mde:  pleaser refer to definition BeamSearchMode
 * frame_offset: interval of frame in ms to beam search
 * p_mem_size: pointer to the variable to hold memory size, used for the upper layer to malloc memory for bs
 *                the memory MUST aligned with data bus width, here 4 bytes.
 * return:  -1 intialization fail, 0 for sucessfull
 */
int32_t beam_search_init(int model_index, BeamSearchInitPara *p_init, MidHandle* mid_pp_inst)
{

    int32_t mem_size;
    int8_t *p_mem = NULL;
    uint8_t *p_private_data = NULL;
    BeamInfo *p_beam_info = NULL;
    BeamSearchInstance *p_inst = NULL;
    
    p_private_data = p_init->p_private_data;
    p_beam_info = (BeamInfo *)p_private_data;
    p_inst = mid_mem_malloc(sizeof(BeamSearchInstance));
    if(p_inst == NULL)
    {
        iet_print(IET_PRINT_ERROR, "%s %d\r\n", __FUNCTION__, __LINE__);
        return MID_PP_ERR;
    }

    if(p_beam_info->work_mode >= BS_MODE_NUM)
    {
        iet_print(IET_PRINT_ERROR, "%s %d\r\n", __FUNCTION__, __LINE__);
        return MID_PP_ERR;
    }

    p_inst->work_mode = p_beam_info->work_mode;
    p_inst->time_out_in_frame_count = TIME_OUT_MS/p_init->frame_offset;

    p_inst->loss_type = p_init->loss_type;
    p_inst->data_offset = p_init->data_offset;
    p_inst->word_num = p_beam_info->word_num;
    p_inst->in_data_size = p_init->in_data_size;
    p_inst->word_thresholds = p_init->word_thresholds;
    p_inst->cmd_thresholds = p_init->cmd_thresholds;
    p_inst->max_cmd_length = p_beam_info->max_cmd_length;
    p_inst->cmd_tree_size = p_beam_info->cmd_tree_size;

    if(p_inst->work_mode == BS_MODE_COMMAND)
    {
        if(p_inst->cmd_thresholds)
            p_inst->word_thresholds = p_inst->cmd_thresholds;
        if(p_inst->word_thresholds)
            p_inst->cmd_thresholds = p_inst->word_thresholds;
    }
    else if(p_inst->work_mode == BS_MODE_WORD)
    {
        // Don't use threshold set by app for this
        p_inst->word_thresholds = NULL;
    }

    if(p_inst->word_thresholds)
    {
        uint16_t *p_mem = mid_mem_malloc(p_inst->word_num*2*sizeof(uint16_t));
        if(p_mem == NULL)
        {
            iet_print(IET_PRINT_ERROR, "%s %d\r\n", __FUNCTION__, __LINE__);
            return MID_PP_ERR;        
        }
        p_inst->word_thresholds_adj = p_mem;
        p_inst->word_thresholds_minus = p_mem + p_inst->word_num;
    }
    else
    {
        // this should happen only when beam as assitant tool and beam work as
        // phrase or word mode
        uint16_t *p_mem = mid_mem_malloc(p_inst->word_num*3*sizeof(uint16_t));
        if(p_mem == NULL)
        {
            iet_print(IET_PRINT_ERROR, "%s %d\r\n", __FUNCTION__, __LINE__);
            return MID_PP_ERR;        
        }
        p_inst->word_thresholds_adj = p_mem;
        p_inst->word_thresholds_minus = p_mem + p_inst->word_num;
        p_inst->word_thresholds = p_mem + 2*p_inst->word_num;

        for(int i=0; i<p_inst->word_num; i++)
        {
            p_inst->word_thresholds[i] = p_inst->work_mode == BS_MODE_PHRASE ? PHRASE_MIN_SCORE_VALID_FRAME : WORD_MIN_SCORE_VALID_FRAME;
        } 
    }
    
    iet_print(IET_PRINT_ALG_DBG, "Net%d:bs%d\r\n", model_index, p_inst->work_mode);

    switch (p_inst->work_mode)
    {
        case BS_MODE_COMMAND:
            // the size must be big enough for cases that too much cmds start with same word, such as "设置..."
            p_inst->beam_size = BS_MIN(p_beam_info->cmd_num*4/5, MAX_BEAM_SIZE_CMD);
            p_inst->beam_size = BS_MAX(p_inst->beam_size, MIN_BEAM_SIZE_CMD);
            p_inst->continue_zero_cmd_end = CMD_CONTINUE_ZERO_CMD_END;
            p_inst->continue_zero_node_end = CMD_CONTINUE_ZERO_NODE_END;
            p_inst->max_candidate_num = BS_MAX(p_beam_info->cmd_num, p_inst->beam_size);
            p_inst->max_candidate_num = BS_MIN(p_inst->max_candidate_num, MAX_CANDIDATE_NUM);
            p_inst->node_switch_threshold = CMD_NODE_SWITCH_MIN_FRAME;           // useless for whole command search
            p_inst->silent_count_cmd_end = CMD_SILENT_COUNT_CMD_END;
            p_inst->word_min_value = CMD_WORD_MIN_VALUE;
            p_inst->cmd_end_sil_threshold = CMD_CMD_END_SIL_THRESHOLD;            
            for(int i=0; i<p_inst->word_num; i++)
            {
                p_inst->word_thresholds_adj[i] = (p_inst->word_thresholds[i]>>1) > CMD_MIN_SCORE_VALID_FRAME ? (p_inst->word_thresholds[i]>>1) : CMD_MIN_SCORE_VALID_FRAME;
                p_inst->word_thresholds_minus[i] = p_inst->word_thresholds[i] - CMD_WORD_SCORE_OFFSET;
            }            
            break;
        case BS_MODE_PHRASE:
            p_inst->beam_size = BS_MIN(p_beam_info->cmd_num/3, MAX_BEAM_SIZE_PHRASE);
            p_inst->beam_size = BS_MAX(p_inst->beam_size, MIN_BEAM_SIZE_CMD);        
            p_inst->continue_zero_cmd_end = PHRASE_CONTINUE_ZERO_CMD_END;
            p_inst->continue_zero_node_end = PHRASE_CONTINUE_ZERO_NODE_END;
            p_inst->max_candidate_num = BS_MAX(p_beam_info->cmd_num/2, p_inst->beam_size);
            p_inst->max_candidate_num = BS_MIN(p_inst->max_candidate_num, MAX_CANDIDATE_NUM);
            p_inst->node_switch_threshold = PHRASE_NODE_SWITCH_MIN_FRAME;
            p_inst->silent_count_cmd_end = PHRASE_SILENT_COUNT_CMD_END;
            p_inst->word_min_value = PHRASE_WORD_MIN_VALUE;
            p_inst->cmd_end_sil_threshold = PHRASE_CMD_END_SIL_THRESHOLD;
            p_inst->min_score_node_keep = PHRASE_MIN_SCORE_NODE_KEEP;
            p_inst->min_score_node_switch = PHRASE_MIN_SCORE_NODE_SWITCH;
            for(int i=0; i<p_inst->word_num; i++)
            {
                p_inst->word_thresholds_adj[i] = (p_inst->word_thresholds[i]>>1) > PHRASE_MIN_SCORE_VALID_FRAME ? (p_inst->word_thresholds[i]>>1) : PHRASE_MIN_SCORE_VALID_FRAME;
                p_inst->word_thresholds_minus[i] = p_inst->word_thresholds[i] - PHRASE_WORD_SCORE_OFFSET;
            }            
            break;            
        case BS_MODE_WORD:
            p_inst->beam_size = BS_MIN(p_beam_info->cmd_num/3, MAX_BEAM_SIZE_WORD);
            p_inst->beam_size = BS_MAX(p_inst->beam_size, MIN_BEAM_SIZE_CMD);        
            p_inst->continue_zero_cmd_end = WORD_CONTINUE_ZERO_CMD_END;
            p_inst->continue_zero_node_end = WORD_CONTINUE_ZERO_NODE_END;
            p_inst->max_candidate_num = BS_MAX(p_beam_info->cmd_num/2, p_inst->beam_size);
            p_inst->max_candidate_num = BS_MIN(p_inst->max_candidate_num, MAX_CANDIDATE_NUM);
            p_inst->node_switch_threshold = WORD_NODE_SWITCH_MIN_FRAME;
            p_inst->silent_count_cmd_end = WORD_SILENT_COUNT_CMD_END;
            p_inst->word_min_value = WORD_WORD_MIN_VALUE;
            p_inst->cmd_end_sil_threshold = WORD_CMD_END_SIL_THRESHOLD;
            p_inst->min_score_node_keep = WORD_MIN_SCORE_NODE_KEEP;
            p_inst->min_score_node_switch = WORD_MIN_SCORE_NODE_SWITCH;
            
            for(int i=0; i<p_inst->word_num; i++)
            {
                //p_inst->word_thresholds_adj[i] = (p_inst->word_thresholds[i]>>1) > WORD_MIN_SCORE_VALID_FRAME ? (p_inst->word_thresholds[i]>>1) : WORD_MIN_SCORE_VALID_FRAME;
                p_inst->word_thresholds_adj[i] =  WORD_MIN_SCORE_VALID_FRAME;
                p_inst->word_thresholds_minus[i] = p_inst->word_thresholds[i] - WORD_WORD_SCORE_OFFSET;
            }            
            break;
        default:
            break;
    }    

           
    mem_size = sizeof(RunningNode)*(p_inst->beam_size + p_inst->max_candidate_num);
    mem_size = (mem_size + MID_PP_ALIGN - 1) & (~(MID_PP_ALIGN - 1));
    mem_size += p_inst->word_num * sizeof(uint16_t);
    mem_size = (mem_size + MID_PP_ALIGN - 1) & (~(MID_PP_ALIGN - 1));
    mem_size += p_inst->word_num * sizeof(uint8_t);
    mem_size = (mem_size + MID_PP_ALIGN - 1) & (~(MID_PP_ALIGN - 1));
    p_inst->mem_size = mem_size;

    p_mem = mid_mem_malloc(mem_size);
    if(p_mem == NULL)
    {
        iet_print(IET_PRINT_ERROR, "%s %d\r\n", __FUNCTION__, __LINE__);
        return MID_PP_ERR;
    }
    
    p_inst->p_mem_pool = p_mem;
    
    p_inst->candidate_all = (RunningNode*)p_mem;
    p_inst->candidate_tops = (RunningNode*)(p_mem+ sizeof(RunningNode)*p_inst->max_candidate_num);
    mem_size = sizeof(RunningNode)*(p_inst->beam_size + p_inst->max_candidate_num);
    mem_size = (mem_size + MID_PP_ALIGN - 1) & (~(MID_PP_ALIGN - 1));
    p_inst->tmp_word_scores = (uint16_t *)(p_mem + mem_size);
    mem_size += p_inst->word_num * sizeof(uint16_t);    
    mem_size = (mem_size + MID_PP_ALIGN - 1) & (~(MID_PP_ALIGN - 1));
    p_inst->sorted_index = (uint8_t *)(p_mem + mem_size);

    p_inst->layer_offset = (uint16_t *)(p_private_data + p_beam_info->layer_offset_offset);
    p_inst->layer_node_num = (uint16_t *)(p_private_data + p_beam_info->layer_node_num_offset);
    p_inst->word_last_time = (uint8_t *)(p_private_data + p_beam_info->word_last_time_offset);
    p_inst->cmd_tree_nodes = (BeamWordNode*)(p_private_data + p_beam_info->cmd_tree_nodes_offset);
   
    if(p_inst->loss_type == PP_LOSS_TYPE_CE)
    {
        for (int i =0; i<p_inst->word_num; i++)
            p_inst->word_last_time[i] = 1;
    }

    *mid_pp_inst = (MidHandle)p_inst;
    beam_search_reset((MidHandle)p_inst);  

    return MID_PP_OK;
}


/*
 * special setting for beam of wakeup layer
 * do after the normal init
 * parameters:
 * handle:  handle of beam search
 * return:  0
 */
int32_t beam_search_setting_wakeup(MidHandle handle)
{
    BeamSearchInstance *p_inst = (BeamSearchInstance*)handle;
    p_inst->continue_zero_cmd_end = PHRASE_CONTINUE_ZERO_CMD_END;
    p_inst->continue_zero_node_end = PHRASE_CONTINUE_ZERO_NODE_END;
    p_inst->node_switch_threshold = PHRASE_NODE_SWITCH_MIN_FRAME;
    // p_inst->silent_count_cmd_end = PHRASE_SILENT_COUNT_CMD_END;
    p_inst->silent_count_cmd_end = 1;
    p_inst->word_min_value = CUTOFF_VALUE_VALID_FRAME;
    p_inst->cmd_end_sil_threshold = CUTOFF_VALUE_VALID_FRAME;
    p_inst->min_score_node_keep = PHRASE_MIN_SCORE_NODE_KEEP;
    p_inst->min_score_node_switch = PHRASE_MIN_SCORE_NODE_SWITCH;
    return 0;   
}


/*
 * API to application
 * process out of one frame from NPU, checking the cmd
 *
 * parameters:
 * word_scores: score list of each word of one frame
 * word_thresholds: threshold list of each word
 * word_len: valid word number related to the word_scores list
 * cmd_ids: list of valid command id
 * cmd_scores: scores of each command, 0 for invalid command
 * cmd_num: expected cmd number from app
 * end_type: force to end the search
 * 
*/

int32_t beam_search_start(MidHandle handle, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t cmd_num, int32_t end_type)
{
    int32_t i;
    int32_t cmd_count = 0;
    int32_t has_cmd;        
    BeamSearchInstance *p_inst = (BeamSearchInstance*)handle;
    uint16_t *p_scores = word_scores + p_inst->data_offset;
    int32_t is_sil_frame = TRUE;

    if(frame_index == 131)
    {
        iet_print(IET_PRINT_ALG_DBG, "start");
    }
#if 0
    iet_printf("BF%3d:", frame_index);
    for(i=0; i<p_inst->word_num; i++)
    {
        if(i==0||i==12 ||i==13||i==39|| i==40 || i==44||i==48||i==49 ||i==50 ||i==51 || i== 66)
        {
            iet_printf("%6d,", p_scores[i]);
            if((i+1)%16==0)
                iet_printf("\n");
        }
    }
    iet_printf("\n");
#endif
    if(p_inst->loss_type != PP_LOSS_TYPE_CE)
    {
        iet_print(IET_PRINT_ERROR, "Err: loss type of BS\r\n");
    }

    // take it as sil frame if all score is less than cut_off value except of <sil>
    // this way works well with <unk>
    // if without <unk>, for words beam search with MSE
    for(int i=1; i<p_inst->in_data_size; i++)
    {
        if(p_scores[i] > CUTOFF_VALUE_VALID_FRAME)
        {
            is_sil_frame = FALSE;
            break;
        }
    }

    // p_inst->word_num is gotten from the beam info, it's <sil>+subword_num, not include other part
    // such as <unk>,  <sil> is not used in BS
    for(i=0; i<p_inst->word_num; i++)
    {
        if(p_scores[i]<p_inst->word_min_value)
        {
            p_scores[i] = 0;
        }

    }

    sort_word_scores(handle, p_scores, p_inst->sorted_index, p_inst->word_num, p_inst->beam_size);

    if(is_sil_frame)
    {
        
#ifdef MID_PP_DEBUG
        sil_frame_count++;  // for debug
#endif
        p_inst->continue_sil_frame_count++;
        if(p_inst->valid_frame_start == 0)
            return PP_RESULT_SILENT;
        // reset BS after N frame sil is detected and there is candidates in queue
        // with wakeup beam, we cannot reset BS once a command is detected since the detected one may be
        // subword of a real cmd
        if(p_inst->continue_sil_frame_count > RESET_DELAY_IN_FRAME && p_inst->valid_candidate_count)
        {
            if(valid_candidate_count(handle) == 0)
            {
                beam_search_reset(handle);
            }
        }
    }
    else
    {
        p_inst->continue_sil_frame_count = 0;
    }


    p_inst->valid_frame_count++;


    has_cmd = beam_search_process(handle, p_scores);
    if(has_cmd == 0)
    {
        int is_ready = is_cmd_ready(handle);
        if(end_type != SEARCH_END_TYPE_WAKEUP)      
            has_cmd = p_inst->continue_sil_frame_count >= p_inst->silent_count_cmd_end && is_ready;
        else
            has_cmd = is_ready;
    }

    if(has_cmd || end_type == SEARCH_END_TYPE_FORCE )
    {
        cmd_count = get_beam_result(handle, cmd_ids, cmd_scores, cmd_num);
        if(end_type != SEARCH_END_TYPE_WAKEUP)
        {
            // wakeup userdefine layer will reset the search as per its decision
            beam_search_reset(handle);
        }
        if(p_inst->continue_sil_frame_count < p_inst->silent_count_cmd_end)
        {
            // valid for SEARCH_END_TYPE_WAKEUP only
            // for this case, the cmd doesn't end in real, just need return the prefix cmd to do double check
            // for example: "小牛客厅灯开灯"， "小牛客厅灯" is a preifx cmd, not real cmd, it need double check by wakeup_stage2
            // and as per <unk>, the cmd doesn't end when "小牛客厅灯" is detected by BS.
            // "小牛客厅灯" is taken as a special cmd in model output, so BS can detects it.
            // Mark "小牛客厅灯" as an interim cmd for wakeup userdefine, not need to report it to app
            int id = *cmd_ids;
            *cmd_ids = (1<<16)|id;
        }
        if(cmd_count > 0)
            return PP_RESULT_VALID_CMD;
        else
            return PP_RESULT_SILENT;
    }
    
    return PP_RESULT_CHECKING_CMD;
    
}


