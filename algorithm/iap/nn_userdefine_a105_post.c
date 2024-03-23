#include "nn_userdefine_a105_post.h"
#include "nn_file_struct.h"
#include "nn_net.h"
#include "nn_common.h"
#include "mid_common.h"
#include "intent_result.h"
#include "mid_npu.h"


// for debug only
#ifdef __x86_64

#endif 

#define MAX_H_W_NUM             16      // max value of encoder out: H*W
#define A105_INTENT_ID_OFFSET   5       //Intent id = a105_id + 5, and intent use 2 as <eos>
#define INTENT_EOS              2

int32_t  num_endpoint_blank = 0;
int32_t  num_encoder_blank = 0;
int32_t  non_blank = 0;
int32_t  pre_idx = -1;
int16_t  intent_input[MAX_WORD_NUM];
int16_t  word_ids_fifo[MAX_WORD_NUM];
int32_t  fifo_wr = 0;
int32_t  fifo_rd = 0;
int32_t  encoder_eos_id = 417;
int32_t  intent_eos_id = 2;

extern int32_t global_frame_index;

int16_t word_ids_kept[MAX_WORD_NUM];   // kept words that has been processed by intent
int32_t kept_word_num = 0;
int32_t kept_word_flag = 0;
int32_t frame_count_since_last_process = 0; //  frame count(in 32 ms) since last intent processs
int32_t words_max_kept_time_in_frame = 0;


int16_t word_add_by_app[MAX_WORD_NUM] = {0};
int32_t word_add_num = 0;              // additional word from app as per human-machine dialogue
int32_t nlp_reponse_delay_in_frame = 0;        // record the distance bewteen the last word and the intent result point

inline void clear_kept_words()
{
    kept_word_num = 0;
}

/*
 * call by app to set the extra words needed by next command recognization, used by Q&A condition
 * set word_num to zero to disable this function, or else it will add the setting words as prefix always
 * Parameters:
 * word_id_list: word ids, max 16
 * word_num:  valid word number
 *           
 */
int mid_nlp_intent_set_status(int16_t *word_id_list, int word_num)
{
    if(word_num > MAX_WORD_NUM)
    {
        word_add_num = 0;
        iet_print(IET_PRINT_ERROR, "Set words [%d] is over than %d", word_num, MAX_WORD_NUM);
        return 0;
    }
    for(int i = 0; i < word_num; i++)
    {
        word_add_by_app[i] = word_id_list[i];
    }
    word_add_num = word_num;

    return 0;
}

int vad_check(int32_t *vad_out, int32_t vad_c, int frame_num, int threshold)
{
    int32_t vad_step = (vad_c+15)/16*8;
    int32_t *p_data = vad_out;
    if(vad_c == 2)
    {
        for(int i=0; i<frame_num; i++)
        {
            int32_t valid_id = 0;
            int16_t max_score = -32768;
            for(int k=0; k<vad_c/2; k++)
            {
                int32_t value = *p_data;
                int16_t value_0 = value&0xFFFF;
                int16_t value_1 = (value>>16)&0xFFFF;
                if(value_0 > max_score)
                {
                    max_score = value_0;
                    valid_id = 2*k;
                }
                if(value_1 > max_score)
                {
                    max_score = value_1;
                    valid_id = 2*k+1;
                }
                p_data++;
            }

            if(valid_id == 0)
                num_endpoint_blank ++;
            else
                num_endpoint_blank = 0;

            // p_data += vad_step;
            p_data++;
        }
    }
    else if(vad_c == 1)
    {
        int16_t value = *p_data&0xFFFF;

        if(value > threshold)
        {
            num_endpoint_blank = 0;
        }
        else
        {
            num_endpoint_blank += frame_num;
        }
    
    }

}

int word_search(int32_t *encoder_out, int32_t encoder_c, int frame_num)
{
    // NOTE: must update here if C is odd number
    int16_t encoder_ids[MAX_H_W_NUM];
    int32_t encoder_step = (encoder_c+15)/16*8;

    int32_t *p_data = encoder_out;
    // for(int i=0; i<8; i++)
    // {
    //     iet_print(IET_PRINT_ALG_DBG, "einF%3d-%d:", global_frame_index, i);
    //     // int32_t value = p_data[0];
    //     // int16_t value_0 = value&0xFFFF;
    //     // int16_t value_1 = (value>>16)&0xFFFF;
    //     // iet_print(IET_PRINT_ALG_DBG, "%6d %6d ", value_0, value_1);
    //     for(int j=1; j<418/2; j++)
    //     {
    //         int32_t value = p_data[j];
    //         int16_t value_0 = value&0xFFFF;
    //         int16_t value_1 = (value>>16)&0xFFFF;
    //         if(value_0>1000)       
    //             iet_print(IET_PRINT_ALG_DBG, "[%6d %6d]", 2*j, value_0);
    //         if(value_1>1000)       
    //             iet_print(IET_PRINT_ALG_DBG, "[%6d %6d]", 2*j+1, value_1);                
    //     }
    //     p_data += 432/2;
    //     iet_print(IET_PRINT_ALG_DBG, "\r\n");
    // }

    // Find the max one for each frame
    for(int i=0; i<frame_num; i++)
    {
        int16_t max_score = -32768;      
        p_data = encoder_out;
        for(int k=0; k<encoder_c/2; k++)
        {
            int32_t value = *p_data;
            int16_t value_0 = value&0xFFFF;
            int16_t value_1 = (value>>16)&0xFFFF;            
            if(value_0 > max_score)
            {
                max_score = value_0;
                encoder_ids[i] = 2*k;
            }
            if(value_1 > max_score)
            {
                max_score = value_1;
                encoder_ids[i] = 2*k+1;
            }
            p_data++;
        }
        // iet_print(IET_PRINT_ALG_DBG, "enc[%d]: %2d %6d\r\n", i, encoder_ids[i], max_score);

        encoder_out +=  encoder_step;
    }

    for(int i=0; i<frame_num; i++)
    {   
        if(encoder_ids[i])
        {
            iet_print(IET_PRINT_ALG_DBG,"v&w F%4d-%d %2d, %6d\r\n", frame_index_to_print, i, num_endpoint_blank, encoder_ids[i]);
        }
    }

    for(int i=0; i<frame_num; i++)
    {
        // iet_print(IET_PRINT_ALG_DBG,"vad&w: f %4d : %6d %6d pre: %d\r\n", i, endpoint_ids[i], encoder_ids[i], pre_idx);
        if (encoder_ids[i] != 0 
            && encoder_ids[i] != 1 
            && encoder_ids[i] != encoder_eos_id)
        {
            // the position of the word, since here to count to get the distance to the intent result
            nlp_reponse_delay_in_frame = 0;

            num_encoder_blank = 0;
            // When the first word is detected, set a big negative value to prevent the accumulation
            // from exceeding the limit "words_max_kept_time_in_frame".
            frame_count_since_last_process = -32768;
            if( encoder_ids[i] != pre_idx)
            {
                non_blank += 1;
                // iet_print(IET_PRINT_ALG_DBG,"up: i= %d, rd/wr: %d %d, %d\r\n", i, fifo_rd, fifo_wr, encoder_ids[i]);
                word_ids_fifo[fifo_wr++] = encoder_ids[i];
                if(fifo_wr == MAX_WORD_NUM)
                {
                    fifo_wr = 0;
                }
                // keep the lastest MAX_WORD_NUM word only
                if(fifo_wr == fifo_rd)
                {
                    fifo_rd += 1;
                    if(fifo_rd == MAX_WORD_NUM)
                    {
                        fifo_rd = 0;
                    }                    
                }

            }
        }
        else
        {
            nlp_reponse_delay_in_frame++;
            num_encoder_blank += 1;
            // check the last group, drop it if timeout
            frame_count_since_last_process++;
            if(frame_count_since_last_process > words_max_kept_time_in_frame)
            {
                // iet_print(IET_PRINT_ALG_DBG, "Clear %d\r\n", frame_count_since_last_process);
                kept_word_num = 0;
            }
        }
        pre_idx = encoder_ids[i];
    }

    // clear the odd word casued by enviroment
    // 64: 8*0.256 = 2s
    if(num_encoder_blank >= 64 && non_blank == 1)
    {
        fifo_rd = 0;
        fifo_wr = 0;
        num_endpoint_blank = 0;
        num_encoder_blank = 0;
        non_blank = 0;
        pre_idx = -1;
        kept_word_num = 0;
        iet_print(IET_PRINT_ALG_DBG,"drop F%4d: %6d\r\n", frame_index_to_print, word_ids_fifo[fifo_rd]);
    }

    // if(non_blank >= 2)
    // {
    //     if (
    //         (num_endpoint_blank >= 8 && num_encoder_blank >= 5)
    //         || num_endpoint_blank > 16
    //         || num_encoder_blank > 15
    //      )
    //      {
    //         word_ids_fifo[fifo_wr] = encoder_eos_id;
    //         num_endpoint_blank = 0;
    //         num_encoder_blank = 0;
    //         non_blank = 0;
    //         pre_idx = -1;
    //         // {// for debug
    //         //     iet_print(IET_PRINT_ALG_DBG, "nlp103 word_ids_fifo: rd/wr: %d %d, blank: %d %d\r\n", fifo_rd, fifo_wr, num_endpoint_blank, num_encoder_blank);
    //         //     int index = fifo_rd;
    //         //     for(int i=0; i<MAX_WORD_NUM; i++)
    //         //     {            
    //         //         iet_print(IET_PRINT_ALG_DBG,"%d,", word_ids_fifo[index]);
    //         //         index++;
    //         //         if(index == MAX_WORD_NUM)
    //         //         {
    //         //             index = 0;
    //         //         }                    }
    //         //     iet_print(IET_PRINT_ALG_DBG, "\r\n");                
    //         // }
    //         return 1;
    //     }
    // }

    return 0;
}

int cmd_end_check()
{
    if(non_blank >= 2)
    {
        if (
            (num_endpoint_blank >= 12 && num_encoder_blank >= 5)
            || num_endpoint_blank > 16
            || num_encoder_blank >= 15
         )
         {
            word_ids_fifo[fifo_wr] = encoder_eos_id;
            num_endpoint_blank = 0;
            num_encoder_blank = 0;
            non_blank = 0;
            pre_idx = -1;
            // {// for debug
            //     iet_print(IET_PRINT_ALG_DBG, "nlp103 word_ids_fifo: rd/wr: %d %d, blank: %d %d\r\n", fifo_rd, fifo_wr, num_endpoint_blank, num_encoder_blank);
            //     int index = fifo_rd;
            //     for(int i=0; i<MAX_WORD_NUM; i++)
            //     {            
            //         iet_print(IET_PRINT_ALG_DBG,"%d,", word_ids_fifo[index]);
            //         index++;
            //         if(index == MAX_WORD_NUM)
            //         {
            //             index = 0;
            //         }                    }
            //     iet_print(IET_PRINT_ALG_DBG, "\r\n");                
            // }
            return 1;
        }
    }

    return 0;    
}

/*
 * return value: 1: without intent out since cmd doesn't end, 0: with intent out
 */
int intent_process(int16_t *output)
{
    // uint32_t start = iet_systick_read();
    output[128] = 0;    // To notify app, no result now.
    if(!cmd_end_check())
        return RESULT_TYPE_NORMAL;

    // sentence end
    for(int i=0; i<MAX_WORD_NUM; i++)
    {
        intent_input[i] = 0;
    }

    // concat the last group words with the new one together as the input of intent process
    // e.g.: "bu yao .... chui wo", the duration between the two groups of words will cause them
    // be fed to intent process independently. A way to improve:
    // - concat them together and feed to intent process
    // - drop the last group after timeout

    // - keep only the latest group if the words number after concat is over than the max size
    int curent_group_word_num = fifo_wr - fifo_rd; // don't include eos, just vaild words
    if(curent_group_word_num < 0)
    {
        curent_group_word_num += MAX_WORD_NUM;
    }
    if(word_add_num || (!kept_word_flag))
    {
        // drop the last group words if extral words are set from app
        kept_word_num = 0;
    }
    if(kept_word_num + curent_group_word_num >= MAX_WORD_NUM) // don't include eos
    {
        kept_word_num = 0;
    }
    if(word_add_num + curent_group_word_num >= MAX_WORD_NUM) // don't include eos
    {
        word_add_num = 0;
    }

    // copy the last group of words(not include EOS etc.) to input buffer of intent process
    int i=0;
    for(i=0; i<kept_word_num; i++)
    {
        intent_input[i] = word_ids_kept[i];
    }
    kept_word_num = 0;

    // copy the additional words(not include EOS etc.) from app, as prefix
    for(; i<word_add_num; i++)
    {
        intent_input[i] = word_add_by_app[i];
    }

    for(;i<MAX_WORD_NUM; i++)
    {
        if(word_ids_fifo[fifo_rd] == encoder_eos_id)
            intent_input[i] = INTENT_EOS;
        else
        {
            intent_input[i] = word_ids_fifo[fifo_rd] + A105_INTENT_ID_OFFSET;
            // save the new word group to buffer to use by next time intent process
            word_ids_kept[kept_word_num] = intent_input[i];
            kept_word_num++;
        }
        if(fifo_rd == fifo_wr)
            break;
        fifo_rd++;
        if(fifo_rd == MAX_WORD_NUM)
            fifo_rd = 0;
    }
    fifo_rd = 0;
    fifo_wr = 0;
    frame_count_since_last_process = 0;

    //call intent
    extern char * app_nlp_project_get_pinyin(uint32_t id);
    iet_print(IET_PRINT_ALG_DBG, "nlp post_o[%d]:", frame_index_to_print);
    for(int i=0; i<MAX_WORD_NUM; i++)
    {
        if(intent_input[i] == INTENT_EOS)
        {
            extern int16_t eos_offset_index;     // eos position of each sentence.
            eos_offset_index = i;
            break;
        }            

        iet_print(IET_PRINT_ALG_DBG,"%s,", app_nlp_project_get_pinyin(intent_input[i]));
    }
    iet_print(IET_PRINT_ALG_DBG, "\r\n");

    // don't reset buffer intent_input[] after intent_start(), need by nlp pp
    intent_start(NET_ID_INTENT, intent_input, output);          

    return RESULT_TYPE_VALID_CMD;
}

/*
 * Check if the sentence is end and do intent if needed
 */
int vad_intent_check(int vad_value, int16_t *output)
{
    num_endpoint_blank = vad_value;
    return intent_process(output);
}

/*
 * To get the words from the input and do intent if sentence end
 * saved on PSRAM with NLP2.0 A105 models
 */
int nn_userdefine_a105_post(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase, int16_t *output) 
{
    LayerParam_t *pLayers = &pNet->pNetInfo->Layers[0];
    // inputLayerId[0] for the layer of words(nlp encoder out)
    LayerParam_t *layer_enc = &pLayers[param->inputLayerId[0]];
    int32_t *input_addr_enc = (int32_t *)((char*)pBase + param->addr.inputAddr*32);
    if(layer_enc->saveFlag==1)
    {
        if(pNet->pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
        {
            input_addr_enc = (int32_t *)((char*)VPU_PSRAM_BASE + layer_enc->addr.saveResultAddr*64);
        }
        else
        {
            input_addr_enc = (int32_t *)((char*)pBase + layer_enc->addr.saveResultAddr*32);
        }    
    } 
    int32_t enc_c = layer_enc->inputShape.C;
    int32_t enc_frame_num = layer_enc->outputShape.H * layer_enc->outputShape.W;

    // inputLayerId[1] for vad's out(softmax) if it exits
    if(param->inputLayerSize > 1)
    {
        LayerParam_t *layer_vad = &pLayers[param->inputLayerId[1]];  
        int32_t *input_addr_vad = (int32_t *)((char*)pBase + param->addr.inputAddr*32);
        if(layer_vad->saveFlag==1)
        {
            if(pNet->pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
            {
                input_addr_vad = (int32_t *)((char*)VPU_PSRAM_BASE + layer_vad->addr.saveResultAddr*64);
            }
            else
            {
                input_addr_vad = (int32_t *)((char*)pBase + layer_vad->addr.saveResultAddr*32);
            }    
        } 
        int32_t vad_c = layer_vad->outputShape.C;
        int32_t vad_frame_num = layer_enc->outputShape.H * layer_enc->outputShape.W;
        UserDefineA105PostPrivateData *last_vad = (UserDefineA105PostPrivateData *)&param->userDefine;
        int16_t threshold = last_vad->vad_threshold;
        vad_check(input_addr_vad, vad_c, vad_frame_num, threshold);
    }
  
    word_search(input_addr_enc, enc_c, enc_frame_num);
    return intent_process(output);
}


int nn_userdefine_a105_post_init(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase)
{
    num_endpoint_blank = 0;
    num_encoder_blank = 0;
    non_blank = 0;
    pre_idx = -1;
    fifo_wr = 0;
    fifo_rd = 0;
    uint32_t *p_data = (uint32_t *)intent_input;
    for(int i = 0; i < MAX_WORD_NUM/2; i++)  
    {
        *p_data++ = 0;
    }
    kept_word_num = 0;
    frame_count_since_last_process = 0;
    frame_index_to_print = 0;
    word_add_num =0;
    return 0;
}

