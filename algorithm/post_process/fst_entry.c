/*****************************************************************************
*  This software is confidential and proprietary and may be used 
*  only as expressly authorized by a licensing agreement from
*                     Intengine.
*            
*              (C) COPYRIGHT 2021 INTENGINE
*                     ALL RIGHTS RESERVED 
*
*  @file     fst_entry.c
*  @brief    fst decoder
*  @author   Xiaoyu.Ren
*  @email    xyren@intenginetech.com
*  @version  0.1
*  @date     2020/08/27
*
*----------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/08/27 | 0.1       | Xiaoyu.Ren     | initial version
*****************************************************************************/

#include "fst_def.h"
#include "fst_entry.h"
#include "fst_asr.h"
#include "fst_decoder.h"

#include "mid_common.h"
#include "mid_mem.h"
#include "iet_common.h"


// one instance is supported only now.
static FstInstance fstInst;
static Fsts fsts;
#ifdef __x86_64  
Arc** fst_arcs;         
#endif

extern Decoder *decoder;
#ifdef WIN_DECODE
extern Decoder *decoder_shift1;
extern Decoder *decoder_shift2;
#endif

/**************************************
* fst init
*
*
**************************************/
int fst_init(Fsts* fsts, void *fst_info_data)
{
    FstsPack *p_fst_pack  = (FstsPack *)fst_info_data;
    fsts->num_states = p_fst_pack->num_states;
    fsts->num_finals = p_fst_pack->num_finals;
    fsts->num_arcs = (int16_t*)(fst_info_data+p_fst_pack->num_arcs_offset);
    fsts->final_states = (int16_t*)(fst_info_data+p_fst_pack->final_states_offset);
    fsts->weight_am_shift = p_fst_pack->weight_am_shift;

#ifdef __x86_64    
    fst_arcs =(Arc**)malloc(sizeof(Arc*)*fsts->num_states);
    if(!fst_arcs)
    {
        iet_printf_err("%s %d\r\n", __FUNCTION__, __LINE__);
    }
    uint32_t *p_fst_arcs_data = (uint32_t *)(fst_info_data+p_fst_pack->Arcs_offset);
    for(int i=0; i<p_fst_pack->num_states; i++)
    {
        fst_arcs[i] = (Arc*)(fst_info_data+p_fst_arcs_data[i]);
    }
    fsts->Arcs = fst_arcs;    
#else
    uint32_t *p_fst_arcs_data = (uint32_t *)(fst_info_data+p_fst_pack->Arcs_offset);
    for(int i=0; i<p_fst_pack->num_states; i++)
    {
        p_fst_arcs_data[i] += (uint32_t)fst_info_data;
    }
    fsts->Arcs = (Arc**)p_fst_arcs_data;
#endif
#if 0
    fsts->Arcs = fst_arcs;
    fsts->num_states = NUM_STATES;
    fsts->num_finals = NUM_FINALS;
    fsts->num_arcs = num_arcs;
    fsts->final_states = final_states;
    //fsts->words = words;
#endif    
    return 1;
}


/**************************************
* fst decoder init
*
*
**************************************/
int32_t fst_dec_init(int model_index, FstInitPara *p_init, MidHandle *mid_pp_inst)
{    
    *mid_pp_inst = (MidHandle)&fstInst;
    fstInst.word_num = p_init->word_num;
    fstInst.pdf_num = p_init->pdf_num;
    fstInst.data_offset = p_init->data_offset;
    fstInst.word_thresholds = p_init->word_thresholds; // not used by fst

    if(!p_init->p_private_data)
    {
        iet_printf_err("%s %d\r\n", __FUNCTION__, __LINE__);
        return MID_PP_ERR;        
    }

    decoder = (Decoder*)mid_mem_malloc(sizeof(Decoder));
    if(decoder == NULL)
    {
        iet_printf_err("%s %d\r\n", __FUNCTION__, __LINE__);
        return MID_PP_ERR;
    }

#ifdef WIN_DECODE
    decoder_shift1 = (Decoder*)mid_mem_malloc(sizeof(Decoder));
    if(decoder_shift1 == NULL)
    {
        iet_printf_err("%s %d\r\n", __FUNCTION__, __LINE__);
        return MID_PP_ERR;
    }

#ifdef TRIPLE_DECODE
    decoder_shift2 = (Decoder*)mid_mem_malloc(sizeof(Decoder));
    if(decoder_shift2 == NULL)
    {
        iet_printf_err("%s %d\r\n", __FUNCTION__, __LINE__);
        return MID_PP_ERR;
    }
#endif

#endif

    int ret = fst_init(&fsts, p_init->p_private_data);
    if (ret < 0) 
    {
        iet_printf_err("%s %d\r\n", __FUNCTION__, __LINE__);
        return MID_PP_ERR;
    }
    // fsts.sil_index = p_init->word_num;
    fsts.sil_index = 1;
    ret = fn_asr_init(&fsts);
    if (ret < 0)
    {
        iet_printf_err("%s %d\r\n", __FUNCTION__, __LINE__);
        return MID_PP_ERR;
    }
    fn_asr_reset();

    iet_printf_dbg("Net%d:silId%d\r\n", model_index, fsts.sil_index);

    return MID_PP_OK;
}

/**************************************
* fst decoding
* 
* word_scores: [0, 32767]
*
**************************************/
int32_t fst_dec_start(MidHandle handle, uint16_t *word_scores, int32_t *cmd_ids, uint32_t *cmd_scores)
{
    FstInstance *p_inst = (FstInstance*)handle;
    int ret = fn_asr_process((int16_t *)word_scores + p_inst->data_offset, p_inst->pdf_num, cmd_ids, (int32_t*)cmd_scores);    
    // for fst output with N commmands, the out is <eps>, cmd_1 ... cmd_N, others_useless, <#0>
    if(ret == 1 && (*cmd_ids) <p_inst->word_num)
    {
        *cmd_scores = 32767;    // TODO: use setting threshold once threshold for FST is confirmed.
        return PP_RESULT_VALID_CMD;
    }
    return PP_RESULT_CHECKING_CMD;
}

