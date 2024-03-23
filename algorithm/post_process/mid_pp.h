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
#ifndef _MID_PP_H_
#define _MID_PP_H_  
    
#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus



#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "iet_common.h"


#define MIN_WORD_LAST_TIME_IN_MS      128     // minimum last time in ms of a Chinese character, normally 200ms for a word



typedef void* MidHandle;

typedef enum 
{
    PP_RESULT_CHECKING_CMD = 0,         // no valid cmd/silent found
    PP_RESULT_VALID_CMD,                // valid cmd found
    PP_RESULT_SILENT,                   // silent
    PP_RESULT_NUM
} MidPpResultType;

typedef enum 
{
    PP_MODE_BEAM_SEARCH = 0,            // do beam only, the threshold provided is for whole cmd
    PP_MODE_PEAK_SEARCH,                // do peak only
    PP_MODE_FST,                        // do fst only
    PP_MODE_PEAK_BEAM,                  // do peak search as main way, beam search as assitant one
    PP_MODE_PEAK_FST,                   // do peak search as main way, fst as assitant one
    PP_MODE_PEAK_ARGMAX,                // do peak search as main way, argMax as assitant one
    PP_MODE_BEAM_BEAM,                  // beam search is used for both ways
    PP_MODE_BEAM_FST,                   // do beam search as main way, fst as assitant one
    PP_MODE_BEAM_ARGMAX,                // do beam search as main way, argMax as assitant one
    PP_MODE_DEFAULT,                    // algo use the default way as per model type
    PP_MODE_WAKEUP_BEAM,                // wakeup userdfine and beam search for am out
    PP_MODE_WAKEUP_ARGMAX,              // wakeup userdfine and argMax for am out
    PP_MODE_WAKEUP,                     // by pass input if post process is not necessary
    PP_MODE_WAKEUP_FST,                 // do fst with wakeup together
    PP_MODE_BEAM_SEARCH_WORD_THRESHOLD, // do beam, the threshold provided is for word/phrase
    PP_MODE_LAST_CMD,                   // select the last cmd as result after silence is detected
    PP_MODE_INTENT_SLOT,                // NLP model Intent slot post process mode
    pp_MODE_THRESHOLD_ONLY,             // check the word's threshold only, don't care its'cut value
    PP_MODE_NUM
} MidPpMode;

typedef struct MidPpInitPara
{
    uint16_t mode;                      // please check MidPpMode for details
    uint16_t character_last_time_ms;    // minimum last time of a Chinese character, deprecated
    uint16_t *word_thresholds_0;        // thresholds group 0 for each word/phrase/command, must be valid value
    uint16_t *word_thresholds_1;        // thresholds group 1 for each word/phrase/command, can be null
    //uint8_t  *p_private_data;           // private data of this post process
} MidPpInitPara;


/**
 * \defgroup    MID_PP_CONTROL_CMD     PP Control Commands
 * \ingroup MID_PP_CONTROL
 * \brief   Definitions for PP Control Command
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - MID_PP_OK,        Control device successfully
 *   - MID_PP_ERR,       Error happens when calling PP APIs
 *   - MID_PP_PAR,       Parameter is not valid for current control command
 * @{
 */

/**
 * Get PP control module version
 * - Param type : char *
 * - Param usage : store result of PP control version
 * - Return value explanation :
 */
#define MID_PP_CMD_BIN_VERSION         (0)

/**
 * Set PP which cmds need to confirm by different
 * - Param type : NULL
 * - Param usage: NULL
 * - Return value explanation :
 */
#define MID_PP_CMD_DOUBLE_CONFIRM      (1)

/**
 * Get the sound pressure level(in dB) of the current cmd
 * - Param type : NULL
 * - Param usage: pointer the varible to hold the sound pressure
 * - Return value explanation : sound pressure level of L&R channel when the cmd is recognized
 */
#define MID_PP_CMD_GET_SPL              (2)


/**
 * Get the response delay of the command
 * - Param type : NULL
 * - Param usage: pointer the varible to hold the delay in ms
 * - Return value explanation : the response delay of the cmd in ms
 */
#define MID_PP_CMD_GET_RESPONSE_DELAY   (3)


/**
 * Get the sound max rms level(in dB) of the current cmd
 * - Param type : NULL
 * - Param usage: pointer the varible to hold the sound pressure
 * - Return value explanation : max rms level of L&R channel when the cmd is recognized
 */
#define MID_PP_CMD_GET_RMS              (4)


/*
 * API to application
 * initialization of post process
 *
 * parameters:
 * p_init:  post process initialization parameters
 *
 * return:  -1 intialization fail, 0 for sucessfull
 */
int32_t mid_pp_init(int model_index, MidPpInitPara *p_init);

/*
 * API to application
 * App to control the beam search algo, set memory used etc.
 *
 * parameters:
 * ctrl_cmd: config type
 * param: parmeters of this type of configuration
 * 
*/
int32_t mid_pp_control(int model_index, int32_t ctrl_cmd, void *param);

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
 * end_type: set to 0 if no special requirment
 * 
*/
int32_t mid_pp_start(int model_index, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores, int32_t cmd_num, int32_t end_type);


#ifdef  __cplusplus
}
#endif // __cplusplus


#endif // _MID_PP_H_

