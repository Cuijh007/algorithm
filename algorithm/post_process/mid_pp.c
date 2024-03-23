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

#include "beam_search.h"
#include "peak_search.h"
#include "fst_entry.h"
#include "intent_result.h"
#include "nn_userdefine.h"

#include "mid_mem.h"
#include "mid_common.h"
#include "mid_pp.h"
#include "mid_pp_private.h"
#include "mid_npu.h"

#include "nn_file_struct.h"

#define ALIGN_16(x) ((x + 15)&0xFFF0)

// #define OUT_SUBWORD_ENABLE

// for sound pressure level calculation
#define SPOTTING_TO_VOICE_END_OFFSET        (8)  // related to the delay of the models
#define VALID_FRAME_FOR_SPL_SEARCH          (24) // make suare MAX_FRAME_SPL >= VALID_FRAME_FOR_SPL + SPOTTING_TO_VOICE_END_OFFSET
#define FRAME_NUM_AVG_SPL                   (4)  // select the max n value for avg
#define VALID_FRAME_FOR_MAX_SPL_SEARCH      (15) // search space of the max spl
int16_t spl_fifo[MAX_AUDIO_CH][MAX_FRAME_SPL] = {0};
int32_t spl_wr = 0;

// for debug
#ifdef MID_PP_DEBUG    
int32_t sil_frame_count = 0;
int32_t frame_index = 0;
int32_t detect_start= 0;
int32_t delay = 0;
#endif

extern int gru_s1_max_score[MAX_MODEL_SUPPORTED];
extern uint16_t *p_wakeup_stage1_thresholds[MAX_MODEL_SUPPORTED];
extern uint16_t *p_cmd_thresholds[MAX_MODEL_SUPPORTED];
extern uint16_t *p_wakeup_stage2_thresholds[MAX_MODEL_SUPPORTED];

// get the response delay of the command, 3 cases:
// 1) cmd with peak search
// 2) wakeup
// 3) NLP, result doesn't save in this varailbe since the model index is different between NLP1chip and 2chip
int32_t reponse_delay_in_frame[MAX_MODEL_SUPPORTED];

MidPpInstance pp_instance[MAX_MODEL_SUPPORTED] = {{PP_MODE_BEAM_SEARCH, 0, 0, NULL, NULL, NULL}, {PP_MODE_BEAM_SEARCH, 0, 0, NULL, NULL, NULL}};

void sort_data(int16_t *in,  int32_t data_size, int32_t top_k)
{
    for(int i=0; i<top_k; i++)
    {
        for(int j=0; j<data_size-1-i; j++)
        {
            if(in[j]>in[j+1])
            {
                int16_t tmp = in[j+1];
                in[j+1] = in[j];
                in[j] = tmp;
            }
        }
    }
}

uint32_t get_avg_spl()
{
    int16_t spl_data[MAX_AUDIO_CH][VALID_FRAME_FOR_SPL_SEARCH];
    int32_t spl_rd = spl_wr - SPOTTING_TO_VOICE_END_OFFSET - VALID_FRAME_FOR_SPL_SEARCH;
    if(spl_rd < 0)
        spl_rd += MAX_FRAME_SPL;

    for(int i=0; i<VALID_FRAME_FOR_SPL_SEARCH; i++)
    {
        spl_data[0][i] = spl_fifo[0][spl_rd];
        spl_data[1][i] = spl_fifo[1][spl_rd];
        spl_rd++;
        if(spl_rd == MAX_FRAME_SPL)
        {
            spl_rd -= MAX_FRAME_SPL;
        }
    }
    sort_data(&spl_data[0][0], VALID_FRAME_FOR_SPL_SEARCH, FRAME_NUM_AVG_SPL);
    sort_data(&spl_data[1][0], VALID_FRAME_FOR_SPL_SEARCH, FRAME_NUM_AVG_SPL);

    int32_t avg_spl[MAX_AUDIO_CH] = {0};
    int16_t *p_data_l = &spl_data[0][VALID_FRAME_FOR_SPL_SEARCH - FRAME_NUM_AVG_SPL];
    int16_t *p_data_r = &spl_data[1][VALID_FRAME_FOR_SPL_SEARCH - FRAME_NUM_AVG_SPL];
    for(int i=0; i<FRAME_NUM_AVG_SPL; i++)
    {
        avg_spl[0] += *p_data_l++;
        avg_spl[1] += *p_data_r++;
    }
    avg_spl[0] /= FRAME_NUM_AVG_SPL;
    avg_spl[1] /= FRAME_NUM_AVG_SPL;

    uint32_t value =((avg_spl[1]&0xFFFF)<<16)|(avg_spl[0]&0xFFFF);
    return value;
}

uint32_t get_max_spl()
{
    int32_t spl_rd = spl_wr - SPOTTING_TO_VOICE_END_OFFSET - VALID_FRAME_FOR_MAX_SPL_SEARCH;
    if(spl_rd < 0)
        spl_rd += MAX_FRAME_SPL;

    int32_t max_spl[MAX_AUDIO_CH] = {-32768, -32768};
    for(int i=0; i<VALID_FRAME_FOR_MAX_SPL_SEARCH; i++)
    {
        if(spl_fifo[0][spl_rd] > max_spl[0])
        {
            max_spl[0] = spl_fifo[0][spl_rd];
        }
        if(spl_fifo[1][spl_rd] > max_spl[1])
        {
            max_spl[1] = spl_fifo[1][spl_rd];
        }
        spl_rd++;
        if(spl_rd == MAX_FRAME_SPL)
        {
            spl_rd -= MAX_FRAME_SPL;
        }
    }

    uint32_t value =((max_spl[1]&0xFFFF)<<16)|(max_spl[0]&0xFFFF);
    return value;
}

/*
 * parameters:
 *  word_num: real cmd number, include <sil>
 *  score_num: score number for the cmds, normal: <sil> <cmd...><unk>[am_out], score_num = <sil> ~ <unk>
 *  out_size: the out size of the last layer, out_size = score_num + number of [am_out]
 */
int32_t local_peak_init(int model_index, uint16_t *word_thresholds, 
        int word_num, int score_num, int out_size)
{
    int32_t ret = MID_PP_OK;
    Net_t *p_net = &g_net;
    NetInfo_t *p_net_info = g_p_net_info;

    if(!pp_instance[model_index].with_gru)
    {
        iet_print(IET_PRINT_ERROR, "Net%d:noGRU, PEAK_FST INV\r\n", model_index);
        return MID_PP_ERR;
    }

    PeakSearchInitPara p_init_peak;
    p_init_peak.word_num = word_num;
#ifdef OUT_SUBWORD_ENABLE
    // if out subword, ignore the unk label when doing peak search, report each subword to app
    p_init_peak.score_num = word_num;
#else
    p_init_peak.score_num = score_num;
#endif
    p_init_peak.out_size = out_size;
    p_init_peak.word_thresholds = word_thresholds;
    if(p_net_info->PostDataOffset[model_index])
    {
        p_init_peak.p_private_data = (uint8_t*)p_net->pPrivateData + p_net_info->PostDataOffset[model_index];
    }
    else
    {
        p_init_peak.p_private_data = NULL;        
    }
   
    ret =  peak_search_init(model_index, &p_init_peak, &pp_instance[model_index].handle);    
    if(ret != MID_PP_OK)
    {
        iet_print(IET_PRINT_ERROR, "Net%d:peak fail\r\n", model_index);
    }

    return ret;
}

int32_t local_beam_init(int model_index, uint16_t *cmd_thresholds, uint16_t *word_thresholds, 
        int data_offset, int data_size)
{
    int32_t ret = MID_PP_OK;
    Net_t *p_net = &g_net;
    NetInfo_t *p_net_info = g_p_net_info;

    BeamSearchInitPara p_init_beam;
    if(p_net->pPrivateData == NULL || p_net_info->PostDataOffset[model_index] == 0)
    {
        iet_print(IET_PRINT_ERROR, "Net%d:beam NoD\r\n", model_index);
        return MID_PP_ERR;
    }        
    p_init_beam.p_private_data = (uint8_t*)p_net->pPrivateData + p_net_info->PostDataOffset[model_index];
    p_init_beam.frame_offset = p_net_info->FrameOffset[model_index];
    p_init_beam.word_thresholds = word_thresholds; 
    p_init_beam.cmd_thresholds = cmd_thresholds;
    p_init_beam.data_offset = data_offset;
    p_init_beam.in_data_size = data_size;

    // don't use the laster layer, it may be concat
    // CE? CTC?
    p_init_beam.loss_type = PP_LOSS_TYPE_CE;

    ret = beam_search_init(model_index, &p_init_beam, &pp_instance[model_index].handle_ext0);
    if(ret != MID_PP_OK)
    {
        iet_print(IET_PRINT_ERROR, "Net%d:beam fail\r\n", model_index);
    }

    return ret;
}


int32_t local_fst_init(int model_index, uint16_t *word_thresholds, int word_num, int data_offset, int pdf_num)
{
    int32_t ret = MID_PP_OK;
    Net_t *p_net = &g_net;
    NetInfo_t *p_net_info = g_p_net_info;
    FstInitPara p_init_fst;
    if(p_net->pPrivateData == NULL || p_net_info->PostDataOffset[model_index] == 0)
    {
        iet_print(IET_PRINT_ERROR, "Net%d:fst NoD\r\n", model_index);
        return MID_PP_ERR;
    }

    if(pdf_num < 1)
    {
        iet_print(IET_PRINT_ERROR, "Net%d:pdfNum=0\r\n", model_index);
        return MID_PP_ERR;        
    }
    p_init_fst.p_private_data = (uint8_t*)p_net->pPrivateData + p_net_info->PostDataOffset[model_index];
    p_init_fst.word_thresholds = word_thresholds;
    p_init_fst.word_num = word_num;
    p_init_fst.data_offset = data_offset;
    p_init_fst.pdf_num = pdf_num; 
    ret = fst_dec_init(model_index, &p_init_fst, &pp_instance[model_index].handle_ext0);
    return ret;
}

int32_t local_intent_init(int model_index, uint16_t *word_thresholds)
{
    int32_t ret = MID_PP_OK;
    Net_t *p_net = &g_net;
    NetInfo_t *p_net_info = g_p_net_info;

    IntentResultInitPara p_init_intent;
    p_init_intent.intent_thresholds = word_thresholds;
    if(p_net->pPrivateData == NULL || p_net_info->PostDataOffset[model_index] == 0)
    {
        iet_print(IET_PRINT_ERROR, "Net%d:intent NoD\r\n", model_index);
        return MID_PP_ERR;
    }
    p_init_intent.p_private_data = (uint8_t*)p_net->pPrivateData + p_net_info->PostDataOffset[model_index];
    nlp_intent_result_init(&p_init_intent, &pp_instance[model_index].handle);
    return ret;
}


/*
 * API to application
 * initialization of post process
 *
 * parameters:
 * p_init:  post process initialization parameters
 *
 * return:  -1 intialization fail, 0 for sucessfull
 */
int32_t mid_pp_init(int model_index, MidPpInitPara *p_init)
{     
    int start_id, end_id;
    Net_t *p_net = &g_net;
    NetInfo_t *p_net_info = g_p_net_info;
    LayerParam_t *p_layer;
    int word_num;           // the real cmd number, includes sil
    int gru_out_size = 0;
    int out_size;
    int bs_in_size = 0;

    if(model_index >= MAX_MODEL_SUPPORTED)
    {
        iet_print(IET_PRINT_ERROR, "pp%d:out[0, %d)\r\n", model_index, MAX_MODEL_SUPPORTED);
        return MID_PP_PAR;        
    }

    // PP_MODE_PEAK_SEARCH_TORCH for torch mode only
    if(p_init->mode != PP_MODE_DEFAULT)
        pp_instance[model_index].mode = p_init->mode;
    else
        pp_instance[model_index].mode = p_net_info->PostProcessMode[model_index];      

    // get word number of this model
    // Here "word" maybe a command, a phrase or a character as per the model.
    // It's the out size of the main output of net, such as GRU with softmax, 
    // may be same as that of the out size of the output layer.
    // since for some net, the output of the last layer may include other datas,
    // such as AM out one.
    start_id = p_net_info->NetLayersStartId[model_index];
    end_id = p_net_info->NetLayersEndId[model_index];
    p_layer = &p_net_info->Layers[end_id];
    out_size = p_layer->outputShape.H *
               p_layer->outputShape.W *
               p_layer->outputShape.C;

    if(p_net_info->CommandNumber[model_index])
    {
        word_num = p_net_info->CommandNumber[model_index];
    }
    else
    {
        word_num = out_size;
    }
    
    if(pp_instance[model_index].mode == PP_MODE_BEAM_SEARCH ||
        pp_instance[model_index].mode == PP_MODE_BEAM_SEARCH_WORD_THRESHOLD)
    {
        // beam doesn't use this parameter, cmd number and phrase number
        // is hold in its private data
        // but we need it when debug input of beam
        pp_instance[model_index].word_num = out_size;
    }
    else
    {
        pp_instance[model_index].word_num = word_num;
    }
    
    /*init parameters of user defined wakeup layer related*/
    for(int index = start_id; index <= end_id; index++)
    {
        p_layer = &p_net_info->Layers[index];
        if(p_layer->type == UserDefineWakeup)
        {               
            WakeupModelSetting_t *p_model_setting = (WakeupModelSetting_t *)&p_layer->userDefine;
            WakeupExtraSetting_t *p_extra_setting =(WakeupExtraSetting_t *)(((uint8_t*)p_net->pPrivateData)+p_layer->extraDataOffset);
            // the checking here to compatible the bin file generated before(include) packweight commit c464c6.
            if(pp_instance[model_index].mode == PP_MODE_PEAK_FST)
            {
                pp_instance[model_index].mode = PP_MODE_WAKEUP_FST;
            }
            else if(pp_instance[model_index].mode == PP_MODE_PEAK_SEARCH)
            {
                 pp_instance[model_index].mode = PP_MODE_WAKEUP;        
            }
            else
            {
                // shouldn't come here
            }
            p_cmd_thresholds[model_index] = p_init->word_thresholds_0;
            if(p_extra_setting->wakeup_number)
            {
                p_wakeup_stage1_thresholds[model_index] = p_init->word_thresholds_1;
                p_wakeup_stage2_thresholds[model_index] = p_init->word_thresholds_0;
            }
            else
            {
                p_wakeup_stage1_thresholds[model_index] = p_init->word_thresholds_0;
                p_wakeup_stage2_thresholds[model_index] = p_init->word_thresholds_0;
            }

            // for debug
            // {
            //     iet_print(IET_PRINT_ALG_DBG, "N%d Threshold:\r\n", model_index);
            //     for(int i=0; i<p_extra_setting->wakeup_number; i++)
            //     {
            //         iet_print(IET_PRINT_LOG, "%6d, %6d\r\n", p_init->word_thresholds_0[i], p_init->word_thresholds_1[i]);
            //     }

            // }

            break;
        }
        // if(p_layer->type == GRUSpecialTreatment)
        //     pp_instance[model_index].with_gru = 1;
    }
    // all net has gru if pp is needed except of FST
    if(pp_instance[model_index].mode != PP_MODE_FST)
    {
        pp_instance[model_index].with_gru = 1;
    }
    if(pp_instance[model_index].with_gru)
    {
        gru_out_size = out_size;
        bs_in_size = out_size;
        
        // special for net using both GRU and FST, the last layer is concate layer
        // GRU out and AM last out are concatenated together
        // both GRU and AM last out are in 1x1xC format
        // if GRU out is not aligned to 16, a copy layer is added after it to align with 16
        // so the size is always aligned to 16 before concatenation.
        if(p_layer->type == Concatenate)
        {
            int in_layer_id0 = p_layer->inputLayerId[0];
            LayerParam_t *p_layer_in = &p_net_info->Layers[in_layer_id0];
            gru_out_size = p_layer_in->outputShape.H * p_layer_in->outputShape.W * p_layer_in->outputShape.C;
            bs_in_size = out_size - gru_out_size;
        }
    }

    iet_print(IET_PRINT_APP_KEY, "Net%d:PPMode %d\r\n", 
        model_index, pp_instance[model_index].mode);

    int ret = 0;
    int pdf_num = out_size - gru_out_size;
    switch (pp_instance[model_index].mode)
    {        
#if (!defined NLP_1CHIP) && (!defined NLP_2CHIP)
        
        case PP_MODE_BEAM_SEARCH:
            // use last layer out as input, for GRU or AM with phone out
            // threshold from app is for the whold cmd
            ret = local_beam_init(model_index, p_init->word_thresholds_0, NULL, 0, bs_in_size);        
            break;
        case PP_MODE_BEAM_SEARCH_WORD_THRESHOLD:
            // use last layer out as input, for GRU or AM with phone out
            // threshold from app is for the word of GRU out
            ret = local_beam_init(model_index, NULL, p_init->word_thresholds_0, 0, bs_in_size);        
            break;            
        case PP_MODE_PEAK_BEAM:
            // two kinds of processing ways are used
            // the main out of net is used by the first process way
            // NOTE: If two processing ways are used by PP, the data are concatenated together to the output of last out layer
            //       and starts with the main part data such as softmax of GRU and  it's aligned to C channel size(16 now)
            //       the whole output of the last layer is aligned to C channel also.
            // peak and beam input data are concated together, peak in first        
            ret = local_peak_init(model_index, p_init->word_thresholds_0, word_num, gru_out_size, out_size);
            if(ret == MID_PP_OK)
            {
                ret = local_beam_init(model_index, NULL, NULL, gru_out_size, bs_in_size);
            }
            break;
        case PP_MODE_FST:
            // for fst work only, two cases to process:
            // 1) The net has GRU, the last output includes <GRU out data> <fst out data> in order,
            //    both aligned to 16 to run copy operation by NPU
            // 2) The net hasn't GRU, the last output includes <fst out data> only, gru_out_size is zero
            ret = local_fst_init(model_index, p_init->word_thresholds_0, word_num, gru_out_size, out_size - gru_out_size);
            break;            
        case PP_MODE_WAKEUP_FST:
            // for PP_MODE_WAKEUP_FST, use wakeup_search_start(),
            // it doesn't need instance to keep private data        
            ret = local_fst_init(model_index, NULL, word_num, gru_out_size, pdf_num);
            break;
        case PP_MODE_PEAK_FST:
            // data is orginized simlar as PP_MODE_PEAK_BEAM
            ret = local_peak_init(model_index, p_init->word_thresholds_0, word_num, gru_out_size, out_size);
            if(ret == MID_PP_OK)
            {
                ret = local_fst_init(model_index, NULL, word_num, gru_out_size, pdf_num);
            }
            break;
#endif
        case PP_MODE_PEAK_SEARCH:
        case PP_MODE_LAST_CMD:
        case pp_MODE_THRESHOLD_ONLY:
            // normally for GRU out
            ret = local_peak_init(model_index, p_init->word_thresholds_0, word_num, gru_out_size, out_size);
            break;
        case PP_MODE_WAKEUP_BEAM:
            // for PP_MODE_WAKEUP_BEAM mode, beach search is called by userdefine of wakeup
            // still need to init beam search for it.
            ret = local_beam_init(model_index, NULL, NULL, 0, bs_in_size);
            if(ret == MID_PP_OK)
            {
                // special setting for wakeup
                ret = beam_search_setting_wakeup(pp_instance[model_index].handle_ext0);
            }
            break;
        case PP_MODE_WAKEUP:
            ret = MID_PP_OK;
            break;
#if (defined NLP_1CHIP) || (defined NLP_2CHIP)              
        case PP_MODE_INTENT_SLOT:
            ret = local_intent_init(model_index, p_init->word_thresholds_0);
            break;
#endif            
        default:
            iet_print(IET_PRINT_ERROR, "pp%d:UNS %d\r\n", model_index, pp_instance[model_index].mode);
            ret = MID_PP_ERR;
            break;
    }

    return ret;
}


/*
 * API to application
 * App to control the beam search algo, set memory used etc.
 *
 * parameters:
 * ctrl_cmd: config type
 * param: parmeters of this type of configuration
 * 
*/
int32_t mid_pp_control(int model_index, int32_t ctrl_cmd, void *param)
{
    Net_t *p_net = &g_net;
    NetInfo_t *pNetInfo = p_net->pNetInfo;
    switch(ctrl_cmd)
    {
        case MID_PP_CMD_DOUBLE_CONFIRM:
            pp_instance[model_index].confirm_cmds = (int8_t *)param;
            iet_print(IET_PRINT_ALG_DBG, "%s M%d double confirm: %p\r\n", __FUNCTION__, model_index, param);
            break;
        case MID_PP_CMD_GET_SPL:
            *((uint32_t*)param) = get_avg_spl();
            break;
        case MID_PP_CMD_GET_RMS:
            *((uint32_t*)param) = get_max_spl();
            break;
        case MID_PP_CMD_GET_RESPONSE_DELAY:
            if(pNetInfo->TrainType[model_index] == NLP_MODEL || pNetInfo->TrainType[model_index] == NLP_2CHIP_MODEL)
            {
                // 5: asr model delay; 4: delay caused by view of conv
                extern int32_t nlp_reponse_delay_in_frame;
                *((uint32_t*)param) = (nlp_reponse_delay_in_frame+5+4)*32;
            }
            else
            {
            // 6:float model delay: pre 9 and post 6; 3: delay caused by view of conv
            *((uint32_t*)param) = (reponse_delay_in_frame[model_index]+6+3)*32;
            }
            break;
        default:
            break;
    }
    return MID_PP_OK;
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
int32_t mid_pp_start(int model_index, uint16_t *word_scores, int32_t *cmd_ids, 
                    uint32_t *cmd_scores, int32_t cmd_num, int32_t end_type)
{
    int32_t ret = PP_RESULT_CHECKING_CMD;
    int32_t response_delay;

    //uint32_t starttime = osKernelGetTickCount();
#if 0
    // if(model_index == 1)
    {
        int print_header = 0;
        for(int i=1; i<pp_instance[model_index].word_num +1; i++)
        {
            if(word_scores[i]>1000)
            {
                print_header = 1;
            }
        }

        if(print_header)
        {
            iet_print(IET_PRINT_ALG_DBG, "M[%d]F%3d],", model_index, frame_index_to_print);
        }
        for(int i=1; i<pp_instance[model_index].word_num +1; i++)
        // for(int i=0; i<48; i++)
        {
            // if(i==0||i== 32 ||i == 10 )
            if(word_scores[i]>1000 || i==0)
            {
                iet_print(IET_PRINT_ALG_DBG, "%2d %6d,", i, word_scores[i]);
            }
    
        }
        if(print_header)
        {
            iet_printf_msg("\r\n");
        }
    }

#endif

    switch (pp_instance[model_index].mode)
    {
#if (!defined NLP_1CHIP) && (!defined NLP_2CHIP)       
        case PP_MODE_BEAM_SEARCH:
        case PP_MODE_BEAM_SEARCH_WORD_THRESHOLD:
            ret = beam_search_start(pp_instance[model_index].handle_ext0, word_scores, cmd_ids, cmd_scores, cmd_num, end_type);
            break;
        case PP_MODE_FST:
            ret = fst_dec_start(pp_instance[model_index].handle_ext0, word_scores, cmd_ids, cmd_scores);
            break;
        case PP_MODE_PEAK_FST:
            ret = peak_fst_search_start(model_index, word_scores, cmd_ids, cmd_scores);
            break;                                    
        case PP_MODE_WAKEUP_FST:
            ret = wakeup_fst_search_start(model_index, word_scores, cmd_ids, cmd_scores);
            break;
        case PP_MODE_PEAK_BEAM:
            ret = peak_beam_search_start(model_index, word_scores, cmd_ids, cmd_scores, cmd_num);
            break;
#endif            
        case PP_MODE_WAKEUP:
        case PP_MODE_WAKEUP_BEAM:
            // beam is called by wakeup userdefine layer, do nothing here
            ret = wakeup_search_start(pp_instance[model_index].word_num, word_scores, cmd_ids, cmd_scores, &response_delay);
            break;
        case PP_MODE_PEAK_SEARCH:
            ret = peak_search_max_score(pp_instance[model_index].handle, word_scores, cmd_ids, cmd_scores, &response_delay);
            break;
        case PP_MODE_LAST_CMD:
            ret = peak_search_last_cmd(pp_instance[model_index].handle, word_scores, cmd_ids, cmd_scores, &response_delay);
            break;
        case pp_MODE_THRESHOLD_ONLY:
            ret = peak_search_threshold_only(pp_instance[model_index].handle, word_scores, cmd_ids, cmd_scores, &response_delay);
            break;        
#if (defined NLP_1CHIP) || (defined NLP_2CHIP)            
        case PP_MODE_INTENT_SLOT:
        {
            // result(in char) is written back to the input since it need 256 in short
            extern int16_t intent_input[];
            ret = nlp_intent_result(pp_instance[model_index].handle, (int16_t*)word_scores, intent_input, (char*)word_scores);
            break;
        }
#endif            
        default:
            break;
    }

    // update response delay value for cmd net only valid cmd is gotten
    // the responsde delay of wakeup net is gotten in wakeup userdefine layer
    if(ret == PP_RESULT_VALID_CMD)
    {
        reponse_delay_in_frame[model_index] = response_delay;
    }    

    // return s1 score to app also
    *cmd_scores |= (gru_s1_max_score[model_index]<<16);
    gru_s1_max_score[model_index] = 0;
        
    return ret;
 
}


