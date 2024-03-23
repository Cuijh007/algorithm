#include "nn_userdefine.h"
#include "nn_file_struct.h"
#include "nn_net.h"
#include "nn_common.h"
#include "mid_common.h"
#include "mid_pp_private.h"
#include "mid_pp.h"
#include "peak_search_private.h"


// for debug only
#ifdef __x86_64
#define MAX_FILE_NAME_LENGTH            2048
short wakeup_gru1_max_score = 0;
short wakeup_prefix_max_score = 0;
char running_file_name[MAX_FILE_NAME_LENGTH];
FILE *fp_wakeup;
#endif 

// max cmd num to hold for once asr, valid for v2 only
#define MAX_CMD_HOLD_NUM        5

int32_t thres_target_s1 = 0;
extern int frame_index_to_print ;

typedef union int32_16x2{
    int32_t a;
    int16_t b[2];
}int32_16x2_t;

uint16_t *p_wakeup_stage1_thresholds[MAX_MODEL_SUPPORTED];  // index 0 for the word used for double confirm, this word is always the last one in the input shape
uint16_t *p_wakeup_stage2_thresholds[MAX_MODEL_SUPPORTED];  
uint16_t *p_cmd_thresholds[MAX_MODEL_SUPPORTED];
uint16_t *p_command_delay[MAX_MODEL_SUPPORTED] = {0, 0};             // pointer to the command delay data

int gru_s1_max_score[MAX_MODEL_SUPPORTED] = {0, 0};  // use by mid_pp to show log also
static int16_t prefix_max_score[MAX_MODEL_SUPPORTED] = {32766, 32766};  // use 32766 to mark as 1 for net w/o prefix, it's different from normal score 32767 for net w/ prefix
static int16_t active_cmd_score[MAX_MODEL_SUPPORTED]; // score of the cmds
static int16_t active_cmd_id[MAX_MODEL_SUPPORTED];    // id of the cmds
static int16_t distance_to_prefix[MAX_MODEL_SUPPORTED] = {0, 0}; // the frame count since prefix word is found
static int16_t s2_reconfirm_valid_frames[MAX_MODEL_SUPPORTED] = {0, 0};
static int16_t last_s1_wake_frames[MAX_MODEL_SUPPORTED] = {0, 0};
static int16_t delay_frames[MAX_MODEL_SUPPORTED] = {0, 0};
static int16_t valid_cmd_count[MAX_MODEL_SUPPORTED] = {0, 0};   // valid command found in Stage 1
static int32_t response_delay[MAX_MODEL_SUPPORTED] = {0, 0};

static int16_t cmd_number_to_reconfim[MAX_MODEL_SUPPORTED] = {0, 0}; // if the cmd number from s1 is over than this, need to run stage 2

/*
 * set result of wakeup word part
 *
 * parameters:
 * result_id: id of the wakeup word
 * result: score this word
 * output_addr: out addr in 32bit, to sync with NPU on board, its memeory can be accessed  by 32bit only
 *              but 2bytes for each command
 * output_size: wakeup word number
 * delay: response delay
 * 
*/
void wakeup_set_output(int32_t result_id, int32_t result_score, int32_t *output_addr, int output_size, int delay)
{   
#if 0
    // 0 for silent always
    // 2 bytes for each command, but access by 4bytes to sync with NPU on board
    for(int i = 0; i < (output_size+1)/2; i++)
    {
        output_addr[i] = 0;
    }
    
    int index = result_id>>1;
    if(result_id&0x1)
    {
        output_addr[index] = (int32_t)result_score<<16;
    }
    else
    {
        output_addr[index] = result_score&0xFFFF;
    }
#else
    /*
     * With beam search, the command number is over than the net out size of userdefine layer, 
     * it's same as that of its input, since the convert tool doesn't know the real command number, 
     * it knows only the subword number, that's the out size of upper layer of the userdefine layer.
     * the real command number is saved in the post process data
     * 
     * So to deal with the beam search case also, we store the result here to the special position
     * in the followin order:
     * <result Id 0><result score 0><result id 1><result socre 1>....
     * each in 16 bits, one pair is used now
     * 
     * NOTE: 
     *     only 1 cmd is supported now. MUST updated input and here if multi cmds is needed
     *     cmd delay is written to the fixed position, and pass to pp
     */
    
    // 2 bytes for each command, but access by 4bytes to sync with NPU on board
    for(int i = 0; i < (output_size+1)/2; i++)
    {
        output_addr[i] = 0;
    }

    output_addr[0] = result_id | (result_score<<16);
    output_addr[1] = delay;
#endif

    return;
}

void wakeup_clear_status(const Net_t *pNet, int model_index, int start_id, int end_id, void *pBase)
{
    LayerParam_t *pLayers = &pNet->pNetInfo->Layers[0];
    for(int LayerIndex = start_id; LayerIndex <= end_id; LayerIndex++)
    {
        LayerParam_t *p_layer = &pLayers[LayerIndex];
        if(p_layer->type == GRUSpecialTreatment/* || p_layer->saveFlag*/)
        {
            int32_t ExtenOutputC = ALIGN_16(p_layer->outputShape.C);
            int32_t outputsize = p_layer->outputShape.H * p_layer->outputShape.W * ExtenOutputC/2;
            int32_t *saveAddr = (int32_t*)((char*)pBase + (p_layer->addr.saveResultAddr*32));
            if(pNet->pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
            {
                saveAddr = (int32_t *)((char*)VPU_PSRAM_BASE + p_layer->addr.saveResultAddr*64);
            }
            
            for(int i=0; i<outputsize; i++)
            {
                    saveAddr[i] = 0;
            }
        }
    }
    return ;
}

/*
 * Note: cmd_id, return value, low 16bit for the real command id, high 16bit for cmd id need to run stage 2
 * 
 */
void stage1_peak_search(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase, 
                        int32_t *cmd_id, int32_t *cmd_score)
{
    WakeupExtraSetting_t *p_extra_setting =(WakeupExtraSetting_t *)(((uint8_t*)pNet->pPrivateData)+param->extraDataOffset);
    LayerParam_t *pLayers = &pNet->pNetInfo->Layers[0];
    LayerParam_t *input = &pLayers[param->inputLayerId[1]];    // softmax layer    
    int32_t *inputAddr = (int32_t *)((char*)pBase + param->addr.inputAddr*32);
    int max_word_index = p_extra_setting->word_number;
    if(p_extra_setting->s1_mode>>8) // checkout word_number limit
    {
        max_word_index += (p_extra_setting->s1_mode>>8);
    }
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
    // for net with prefix, the data after normal command is for it.
    if(p_extra_setting->with_prefix)
        max_word_index ++;

    if(p_extra_setting->with_prefix && distance_to_prefix[model_index])
    {
        distance_to_prefix[model_index]++;
    }

    *cmd_id = 0;
    *cmd_score = 0;
    response_delay[model_index]++;

    int max_cmd_score = 0;
    int max_all_score = 0;
    int max_cmd_index = 0;

    // find max score of the command
    // iet_print(IET_PRINT_ALG_DBG, "ud，F%d M%d: ", frame_index_to_print, model_index);
    for(int i=1; i<=max_word_index; i++)
    {
        int32_16x2_t result_tmp;
        result_tmp.a = inputAddr[i>>1];
        int16_t result = result_tmp.b[i&0x1];        
        if(result > max_cmd_score)
        {
            max_cmd_score = result;
            max_cmd_index = i;
        }
        // iet_print(IET_PRINT_ALG_DBG, "%6d, ", result);
    }
    // find max score of all except of sil
    max_all_score = max_cmd_score;
    for(int i=max_word_index+1; i<param->outputShape.C; i++)
    {
        int32_16x2_t result_tmp; 
        result_tmp.a = inputAddr[i>>1];
        int16_t result = result_tmp.b[i&0x1];        
        if(result > max_all_score)
        {
            max_all_score = result;
        }
        // iet_print(IET_PRINT_ALG_DBG, "%6d, ", result);      
    }
    // iet_print(IET_PRINT_ALG_DBG, "\r\n");

    // if cmd_scores > 0.3 start record and find max
    // check this firstly to reduce cpu load
    if(max_cmd_score > MIN_SCORE_VALID_FRAME)
    {
        int thres_target;
        int active_index = max_cmd_index;        
        
        // for thresholds: 
        // index 0 is for prefix word(exists when p_extra_setting->with_prefix ==1)
        // wakeup workd has two threshold values, the value of the stage1 one should be used here
        // for normal command word, only one threshold is provided
        if(active_index <= p_extra_setting->wakeup_number)
            thres_target = p_wakeup_stage1_thresholds[model_index][active_index];
        else if(p_extra_setting->with_prefix && active_index == max_word_index)
            thres_target = p_wakeup_stage1_thresholds[model_index][0];
        else
            thres_target = p_cmd_thresholds[model_index][active_index];
        
        thres_target_s1 = thres_target;
        if(max_cmd_score >= thres_target)
        {
            if(p_extra_setting->with_prefix)
            {
                if(active_index == max_word_index)
                {
                    if(max_cmd_score > prefix_max_score[model_index])
                    {
                        prefix_max_score[model_index] = max_cmd_score;
                    }
                    distance_to_prefix[model_index] = 1;                        
                }
                else
                {
                    // prefix word found and still valid
                    if(distance_to_prefix[model_index] && 
                        distance_to_prefix[model_index] <= 50)
                    {
                        if(active_index != active_cmd_id[model_index])
                        {
                            // the number is accurate for some case, for example: id0 -> id1 -> id0
                            // the count is used to check on/off pair, only need to know whether the
                            // command gotten from s1 is over than 1
                            valid_cmd_count[model_index]++;
                        }                        
                        if((p_extra_setting->s1_mode&0xFF) == S1_MODE_LAST_CMD // just checkout L8 bits
                            && active_index != active_cmd_id[model_index])
                        {
                            active_cmd_score[model_index] = max_cmd_score;
                            active_cmd_id[model_index] = active_index;
                            response_delay[model_index] = 0;
                        }
                        else 
                        {
                            if(max_cmd_score > active_cmd_score[model_index])
                            {
                                active_cmd_score[model_index] = max_cmd_score;
                                active_cmd_id[model_index] = active_index;
                                response_delay[model_index] = 0;
                            } 
                        }
                    }
                }
            }
            else
            {
                if(active_index != active_cmd_id[model_index])
                {
                    // the number is inaccurate for some case, for example: id0 -> id1 -> id0
                    // the count is used to check on/off pair, only need to know whether the
                    // command gotten from s1 is over than 1
                    valid_cmd_count[model_index]++;
                }
                if((p_extra_setting->s1_mode&0xFF) == S1_MODE_LAST_CMD // just checkout L8 bits
                    && active_index != active_cmd_id[model_index])
                {
                    active_cmd_score[model_index] = max_cmd_score;
                    active_cmd_id[model_index] = active_index;
                    response_delay[model_index] = 0;
                }
                else 
                {
                    if(max_cmd_score > active_cmd_score[model_index])
                    {
                        active_cmd_score[model_index] = max_cmd_score;
                        active_cmd_id[model_index] = active_index;
                        response_delay[model_index] = 0;
                    } 
                }
            }                
        }
    }

    // cmd end
    if(active_cmd_id[model_index])
    {
        int is_cmd_end = 0;
        if(p_extra_setting->extra_label == EXTRA_LABEL_TYPE_EOS)
        {
            int16_t eos_id = max_word_index + 1;
            int16_t eos_score = inputAddr[eos_id>>1] & 0xFFFF;
            if(eos_id&1)
            {
                eos_score = (inputAddr[eos_id>>1]>>16) & 0xFFFF;
            }
            if(eos_score > EOS_THRESHOLD)
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
            int valid_cmd_id = 0;

            valid_cmd_id = active_cmd_id[model_index];
            delay_frames[model_index]++;
            if(p_command_delay[model_index])
            {
                delay_threhold = p_command_delay[model_index][valid_cmd_id];
            }
            if(delay_frames[model_index] > delay_threhold)
            {
                // the command need reconfirm(on/off pair) is wakeup word also
                if(valid_cmd_count[model_index] >= cmd_number_to_reconfim[model_index] && 
                    valid_cmd_id <= p_extra_setting->wakeup_number)
                {
                    // do 2nd stage for wakeup cmd when cmds gotten from s1 is over than the specified value
                    // cmd_id: low 16bits for s1 result cmd id, high 16bits for cmd that need to run stage 2
                    *cmd_id = valid_cmd_id | (valid_cmd_id<<16);
                    // *cmd_score = active_cmd_score[model_index];
                    // gru_s1_max_score[model_index] = active_cmd_score[model_index];
                }
                else
                {
                    // normal cmd or only one cmd is found for wakeup cmd
                    *cmd_id = valid_cmd_id;
                    // *cmd_score = active_cmd_score[model_index];
                    // gru_s1_max_score[model_index] = 0;

        #ifdef __x86_64           
                    // for LibVPU debug only
                    if(valid_cmd_id <= p_extra_setting->wakeup_number )
                    { 
                        // for wakeup word, two stages threshold is used by ietsim_run.py to statistics the result
                        // so must set it here, take them as the same value.
                        fprintf(fp_wakeup, "%s\t%6d\tid%d_%d\tpre: %6d\ts1: %6d\ts2s: %6d\n", running_file_name, frame_index_to_print, 
                            *cmd_id, valid_cmd_id, prefix_max_score[model_index], active_cmd_score[model_index], 32766);
                    }
        #endif                                                          
                }
                *cmd_score = active_cmd_score[model_index];
                gru_s1_max_score[model_index] = active_cmd_score[model_index];

                // clear the status
                delay_frames[model_index] = 0;
                distance_to_prefix[model_index] = 0;
                active_cmd_score[model_index] = 0;
                active_cmd_id[model_index] = 0;
                valid_cmd_count[model_index] = 0;
            }
        }
    }
}

/*
 * special process to select the cmd from a cmd pair, such as on/off, up/down etc
 * the stages 2 net is for the ON... cmds only, all 'on' cmds are put together.
 */
void stage2_on_off_check(int model_index, int32_t run_stage2_id, int32_t net_out_score, 
                        int32_t on_cmd_num, int32_t *result_id, int32_t *result_score)
{
    // check the last out score only for this step
    int stage2_score;
    int reverse_id; 
    int reverse_score;

    if(run_stage2_id <= on_cmd_num)
    {
        stage2_score = net_out_score;
        reverse_score = 32768 - net_out_score;
        reverse_id = run_stage2_id + on_cmd_num;
    }
    else
    {
        stage2_score = 32768 - net_out_score;
        reverse_score = net_out_score;
        reverse_id = run_stage2_id - on_cmd_num;
    }

    *result_id = 0;
    *result_score = 0;
    if(stage2_score >= p_wakeup_stage2_thresholds[model_index][run_stage2_id])
    {
        *result_id = run_stage2_id;  
        *result_score = stage2_score;
        iet_print(IET_PRINT_ALG_DBG, "v2 -normal %d\r\n", run_stage2_id);
#ifdef __x86_64
        fprintf(fp_wakeup, "%s\t%6d\tid%d_%d\tpre: %6d\ts1: %6d\ts2s: %6d\n", running_file_name, frame_index_to_print, 
            run_stage2_id, result_id, prefix_max_score[model_index], gru_s1_max_score[model_index], result_score);
#endif      
    }
    // else if(reverse_score >= p_wakeup_stage2_thresholds[model_index][reverse_id])
    else if(stage2_score < p_wakeup_stage2_thresholds[model_index][run_stage2_id] - 6553)
    {
        iet_print(IET_PRINT_ALG_DBG, "v2 - reverse to %d\r\n", reverse_id);
        // Note: for this case the s1 score is from the original cmd id, instead of the reversed id
        // when statistics by the ietsim_run.py, the model_0_result.txt data may be a little different from the
        // result gotten by ietsim.exe directly since ietsim_run.py use the match s1/s2 score for each cmd.
        // the error with '-0.2' is less than that of the check of reversed threshold directly
        *result_id = reverse_id;  
        *result_score = reverse_score;
#ifdef __x86_64
        fprintf(fp_wakeup, "%s\t%6d\tid%d_%d\tpre: %6d\ts1: %6d\ts2v--0.2: %6d\n", running_file_name, frame_index_to_print, 
            run_stage2_id, result_id, prefix_max_score[model_index], gru_s1_max_score[model_index], result_score);
#endif                  
    }      
    else
    {
#ifdef __x86_64
        fprintf(fp_wakeup, "%s\t%6d\tid%d_%d\tpre: %6d\ts1: %6d\ts2f: %6d\n", running_file_name, frame_index_to_print, 
            run_stage2_id, run_stage2_id, prefix_max_score[model_index], gru_s1_max_score[model_index], net_out_score);
#endif                 
    }       
}

/*
 * Stage 2: TDNN process
 * 
 */
void stage2_tdnn(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase, 
                int32_t run_stage2_id, int32_t *result_id, int32_t *result_score)
{
    WakeupExtraSetting_t *p_extra_setting =(WakeupExtraSetting_t *)(((uint8_t*)pNet->pPrivateData)+param->extraDataOffset);
    WakeupModelSetting_t *p_model_setting = (WakeupModelSetting_t *)&param->userDefine;         

    LayerParam_t *pLayers = &pNet->pNetInfo->Layers[0];
    LayerParam_t *input1 = &pLayers[param->inputLayerId[0]];   // timedelay buffer
    int32_t *src_addr = (int32_t *)((char*)pBase + input1->addr.saveResultAddr*32);
    int32_t in_frame_size = ALIGN_16(input1->outputShape.C);   // Upper one of the timedelay layer should be 1x1xC only, or else update here
    int32_t in_frame_number = input1->outputShape.H * input1->outputShape.W;
    if(pNet->pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
    {
        src_addr = (int32_t *)((char*)VPU_PSRAM_BASE + input1->addr.saveResultAddr*64);
    }

    int16_t __attribute__ ((aligned(4))) output_buffer[MAX_WAKEUP_S2_OUT_SIZE];  
    int32_t *dst_addr = (int32_t*)((char*)pBase + pLayers[p_model_setting->s2_startId].addr.inputAddr*32);
    int copy_len = in_frame_size * in_frame_number;
    int s2_2nd_run = 0;
    // last_s1_wake_frames[model_index]: the way of the frame count is different between A101 and A102, 
    //    - for A101 it's same as that of the original input
    //    - for A102 it's half of the original input since max pooling with stride[0]=2 will drop half of frames
    // in_frame_number is to count frame after subsampling(64ms) always.
    // "/2": valid for A101 since timedelay of TDNN input do the subsampling.
    // A101 is used majorly now. 
    // Here use timedelay's stride[0] to distinguish A101 from A102
    int distance_to_last_s1_wake = last_s1_wake_frames[model_index];
    if(input1->strides[0] == 2)
    {
        distance_to_last_s1_wake >>= 1;
    }
    if(distance_to_last_s1_wake < (in_frame_number + 16))
    {
        int clear_len = ((in_frame_number + 16) - distance_to_last_s1_wake);
        clear_len = clear_len <= (in_frame_number/2) ? clear_len : (in_frame_number/2);
        clear_len *= (in_frame_size/2);
                        
        for(int j = 0; j < clear_len; j++)
        {
            src_addr[j] = 0;
        }
    }
S2_FORWARD:            
    for(int j = 0; j < copy_len/2; j++)
    {
        dst_addr[j] = src_addr[j];
    }

    wakeup_clear_status(pNet, model_index, p_model_setting->s2_startId, p_model_setting->s2_endId, pBase);
    memset(output_buffer, 0, MAX_WAKEUP_S2_OUT_SIZE*sizeof(int16_t));
    // run_times is 0 or 1 for TDNN, 2021.08.24
    //for(int i = 0; i < p_model_setting->run_times; i++)
    {   
        load_vcode_layers(model_index, p_model_setting->s2_startId, p_model_setting->s2_endId);
        NetForward(pNet, model_index, p_model_setting->s2_startId, p_model_setting->s2_endId, NULL, output_buffer);             
    }
    if(p_extra_setting->on_cmd_number == 0)
    {
        *result_score = output_buffer[1];  // to show S2 score in log only
        // check the last out score only for this step for wakeup net
        if(output_buffer[1] >= p_wakeup_stage2_thresholds[model_index][run_stage2_id])
        {
            *result_id = run_stage2_id;  
            *result_score = output_buffer[1];
#ifdef __x86_64
            fprintf(fp_wakeup, "s2s: %6d",  output_buffer[1]);
#endif      
        }
        else if(!s2_2nd_run && (last_s1_wake_frames[model_index] < s2_reconfirm_valid_frames[model_index]))
        {
            s2_2nd_run = 1;
            for(int j = 0; j < copy_len/4; j++)
            {
                src_addr[j] = 0;
            } 
            goto S2_FORWARD;
        }        
        else
        {
#ifdef __x86_64
            fprintf(fp_wakeup, "s2f: %6d",  output_buffer[1]);
#endif                 
        }
    }  
    else
    {
        // do process for net with 'on/off' etc cmd pair
        stage2_on_off_check(model_index, run_stage2_id, output_buffer[1], 
                p_extra_setting->on_cmd_number, result_id, result_score);
    }  
}


/*
 * Stage 2: gru only process
 * 
 */
void stage2_gru(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase, 
                int32_t run_stage2_id, int32_t *result_id, int32_t *result_score)
{
    WakeupExtraSetting_t *p_extra_setting =(WakeupExtraSetting_t *)(((uint8_t*)pNet->pPrivateData)+param->extraDataOffset);
    WakeupModelSetting_t *p_model_setting = (WakeupModelSetting_t *)&param->userDefine;         
    LayerParam_t *pLayers = &pNet->pNetInfo->Layers[0];
    LayerParam_t *input1 = &pLayers[param->inputLayerId[0]];   // timedelay buffer
    int32_t *src_addr = (int32_t *)((char*)pBase + input1->addr.saveResultAddr*32);
    int32_t *dst_addr = (int32_t*)((char*)pBase + pLayers[p_model_setting->s2_startId].addr.inputAddr*32);
    int32_t in_frame_size = ALIGN_16(input1->outputShape.C);   // input of the timedelay layer should be 1x1xC only, or else update here
    int32_t in_frame_number = input1->outputShape.H * input1->outputShape.W;
    int16_t __attribute__ ((aligned(4))) output_buffer[MAX_WAKEUP_S2_OUT_SIZE];  
    if(pNet->pNetInfo->DataMemoryType[model_index] == MEMORY_TYPE_PSRAM)
    {
        src_addr = (int32_t *)((char*)VPU_PSRAM_BASE + input1->addr.saveResultAddr*64);
    }

    // two ways for gru: forward or reverse, p_model_setting->reverse == 0 for forward
    int32_t *p_src = src_addr + (in_frame_size * (in_frame_number-1)/2);
    int32_t max_s2_score = 0;
    if(p_model_setting->reverse == 0)
    {
        p_src = src_addr;
    }
    wakeup_clear_status(pNet, model_index, p_model_setting->s2_startId, p_model_setting->s2_endId, pBase);
    load_vcode_layers(model_index, p_model_setting->s2_startId, p_model_setting->s2_endId);
    for(int i=0; i<in_frame_number; i++)
    {
        int32_t *p_dst = dst_addr;
        for(int j=0; j<in_frame_size/2; j++)
        {
            *p_dst++ = *p_src++;
        }
        NetForward(pNet, model_index, p_model_setting->s2_startId, p_model_setting->s2_endId, NULL, output_buffer);
        if(output_buffer[1] > max_s2_score)
        {
            max_s2_score = output_buffer[1];
            *result_score = max_s2_score;   // to show S2 score in log only
        }

#ifdef __x86_64
        if(max_s2_score > MIN_SCORE_VALID_S2 && output_buffer[1] < CUTOFF_VALUE_VALID_FRAME)
#else
        if(max_s2_score > p_wakeup_stage2_thresholds[model_index][run_stage2_id] && output_buffer[1] < CUTOFF_VALUE_VALID_FRAME)
#endif
        {
            // for simultor test, very lower threshold(such as 0) maybe used
            // must make sure a potential cmd is found, then exit the process when the current score is less than 0.2
            // normal, the score should likes this:
            //  low low middle high middle low
            break;
        }
        if(p_model_setting->reverse == 1)
        {
            p_src -= in_frame_size;
        }
    }

    if(max_s2_score >= p_wakeup_stage2_thresholds[model_index][run_stage2_id])
    {
        *result_id = run_stage2_id;  
        *result_score = max_s2_score;
#ifdef __x86_64
        fprintf(fp_wakeup, "s2s: %6d",  max_s2_score);
#endif
    }
    else
    {
#ifdef __x86_64
        fprintf(fp_wakeup, "s2f: %6d",  max_s2_score);
#endif       
    }
}

/*
 * user define layer for wakeup
 * Two inputs:
 * 1) softmax result of the cmd classifier, it's in format: <sil><cmd1>...<cmdN>[prefix_id][unk], 
 *    prefix_id and unk is optional.
 * 2) frames buffer(normally a time delay layer's output) used to do double-comfirm
 * Two work ways:
 * 1) wakeup cmds + normal cmds, each cmd need to do stage 2, stage 2 is used to double confirm
 *    whether the cmd gotten by stage 1 is a valid wakeup cmd. stage 2 out has two elements only.
 *    the threshold of stage two for each command is from the statistics point of view.
 * 2) cmds only with special cmd pairs, such as <on - off> <up - down>
 *    two cmds may be gotten from stage 1, stage 2 is used to select the command. Stage 2 only
 *    recoginize on/up as per trainning
 *
 */
int nn_userdefine_wakeup(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase)
{
    // int32_t max_cmd_score = 0;
    // int32_t max_all_score = 0;
    // int32_t max_cmd_index = 0;

    int32_t result_score = 0;    
    int32_t result_id = 0;
    int32_t run_stage2_id = 0;
    int32_t interim_cmd = 0;

    // do stage 1 process
    WakeupExtraSetting_t *p_extra_setting =(WakeupExtraSetting_t *)(((uint8_t*)pNet->pPrivateData)+param->extraDataOffset);
    if((p_extra_setting->s1_mode&0xFF)!= S1_MODE_BEAM) // just checkout L8 bits
    {
        stage1_peak_search(pNet, model_index, param, pBase, &result_id, &result_score);
    }
    else
    {
        // do beam search
        // for wakeup beam, prefix cmd is reported from BS though the sentence doesn't end, it will confirm by stage 2
        int32_t cmd_ids[MAX_CMD_HOLD_NUM];
        uint32_t cmd_scores[MAX_CMD_HOLD_NUM];
        int32_t ret = wakeup_s1_beam_search(pNet, model_index, param, pBase, cmd_ids, cmd_scores, MAX_CMD_HOLD_NUM);            
        if(ret == PP_RESULT_VALID_CMD)
        {
            // BS will mark wether the cmds is an interim one.
            // cmd_ids: High16bit for the flag, 1 for interim one, 0 for normal command. Low16bits: the cmd id
            uint32_t thres_target;
            int32_t id = cmd_ids[0];
            interim_cmd = (cmd_ids[0]>>16);
            id = cmd_ids[0]&0xFFFF;

            if(id <= p_extra_setting->wakeup_number)
                thres_target = p_wakeup_stage1_thresholds[model_index][id];
            else
                thres_target = p_cmd_thresholds[model_index][id];
            thres_target_s1 = thres_target;
            // select the last cmd that score is over than its threshold
            if(cmd_scores[0] >= thres_target)
            {
                result_id = id;
                result_score = cmd_scores[0];
                if(result_id <= p_extra_setting->wakeup_number)
                {
                    // marked to run stage 2
                    result_id |= (id<<16);
                }
                gru_s1_max_score[model_index] = cmd_scores[0];
            }
            else
            {
                if(interim_cmd == 0)
                {
                    // reset BS if the score is less than the threshold
                    wakeup_beam_search_reset(model_index);
                }

            }
        }

    }
    run_stage2_id = result_id >> 16;
    result_id &= 0xFFFF;

    if(result_id)
    {
#ifdef __x86_64           
        // for LibVPU debug only
        fprintf(fp_wakeup, "%s\t%6d\tid%d_%d\tpre: %6d\ts1: %6d\t", running_file_name, frame_index_to_print, 
            run_stage2_id, result_id, prefix_max_score[model_index], gru_s1_max_score[model_index]);
#endif        
        
        iet_print(IET_PRINT_ALG_DBG, "Frame%6d: G1, id=%d score: %6d %6d\r\n", frame_index_to_print, 
               run_stage2_id,  gru_s1_max_score[model_index], thres_target_s1);

        // clear this value if the command no stage 2
        if(!run_stage2_id)
        {
            gru_s1_max_score[model_index] = 0;
        }
    }
    
    // run the 2nd stage for the wakeup word only, or special cmd needed
    // run_stage2_id: 0 or equal to result_id
    if(run_stage2_id)
    {
        //uint32_t starttime = osKernelGetTickCount();
        result_id = 0;
        result_score = 0;
        WakeupModelSetting_t *p_model_setting = (WakeupModelSetting_t *)&param->userDefine;
        if(p_model_setting->run_times == 0)
        {
            // TDNN for stage 2
            uint32_t start_time = iet_systick_read();
            stage2_tdnn(pNet, model_index, param, pBase, run_stage2_id, &result_id, &result_score);
            uint32_t end_time =  iet_systick_read();
            iet_print(IET_PRINT_ALG_DBG, "Frame%6d: T2, id=%d score: %6d %6d, %d\r\n", 
                    frame_index_to_print, run_stage2_id, result_score, p_wakeup_stage2_thresholds[model_index][run_stage2_id],
                    end_time - start_time);
        }
        else if(p_model_setting->run_times == 1)
        {
            // conv + GRU for stage 2
        }
        else
        {
            uint32_t start_time = iet_systick_read();
            stage2_gru(pNet, model_index, param, pBase, run_stage2_id, &result_id, &result_score);
            uint32_t end_time =  iet_systick_read();
            iet_print(IET_PRINT_ALG_DBG, "Frame%6d: G2, id=%d score: %6d %6d, %d\r\n", 
                    frame_index_to_print, run_stage2_id, result_score, p_wakeup_stage2_thresholds[model_index][run_stage2_id],
                    end_time - start_time);
        }
        
#ifdef __x86_64
        fprintf(fp_wakeup, "\r\n");
#endif
        last_s1_wake_frames[model_index] = 0;          
        //uint32_t endtime = osKernelGetTickCount();
        //iet_print(IET_PRINT_ALG_DBG, "time:%d\r\n", endtime-starttime); 
    }

    /*
     * Need reset BS for the following cases:
     * 1) score of the cmd from BS is less than the threshold value
     * 2) Fail the stage 2 check. 
     * 3) a normal cmd is detected
     * 4) a wakeup cmd is detected and wait N frames, why doesn't it reset BS once a wakeup cmd is detected?
     *    two cases for the wakeup cmds here, 
     *    - a real wakeup cmd, such as "小牛开灯", reset should be done in time
     *    - a subword taken as wakeup cmd, such as "小牛客厅灯", BS shouldn't be reset after it's detected until
     *      it fail the double check of stage2
     *    "wait N frames": BS will do it internally
     *
     */
    if((p_extra_setting->s1_mode&0xFF) == S1_MODE_BEAM) // just checkout L8 bits
    {
        if(interim_cmd)
        {      
            // don't report interim cmd to app
            result_id  = 0; 
            result_score = 0;    
        }
        else if((run_stage2_id && result_id == 0) || (run_stage2_id == 0 && result_id))
        {
            wakeup_beam_search_reset(model_index);
        }
    }
    int32_t *output_addr = (int32_t *)((char*)pBase + param->addr.outputAddr*32);
    wakeup_set_output(result_id, result_score, output_addr,  param->outputShape.C, response_delay[model_index]);
    last_s1_wake_frames[model_index]++;
    return 0;   
}


int nn_userdefine_wakeup_init(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase) 
{
    // reset wakeup local varibles
    memset(&active_cmd_score[0], 0, sizeof(active_cmd_score));
    memset(&active_cmd_id[0], 0, sizeof(active_cmd_id));
    memset(&valid_cmd_count[0], 0, sizeof(valid_cmd_count));
    NetInfo_t *pNetInfo = pNet->pNetInfo;    
    WakeupExtraSetting_t *p_extra_setting =(WakeupExtraSetting_t *)(((uint8_t*)pNet->pPrivateData)+param->extraDataOffset);
    if(pNetInfo->PostDataOffset[model_index] && (p_extra_setting->s1_mode&0xFF) != S1_MODE_BEAM) // just checkout L8 bits
    {
        // p_command_delay is valid for none  beam search only
        // beam search private data and peak private data share same area both defined by pNetInfo->PostDataOffset[]
        int word_number = p_extra_setting->word_number;
        if(p_extra_setting->s1_mode>>8)  // checkout word_number limit
        {
            word_number += (p_extra_setting->s1_mode>>8);
        }
        void *p_private_data =  (uint8_t*)pNet->pPrivateData + pNetInfo->PostDataOffset[model_index];
        PostProcessDataHeader *p_header = (PostProcessDataHeader *)p_private_data;
        if(p_header->command_delay_num)
        {
            p_command_delay[model_index] = (uint16_t *)(p_private_data + p_header->command_delay_offset);
            if(p_header->command_delay_num - 1 != word_number)
            {
                iet_print(IET_PRINT_ERROR, "warning: net[%d] %s command number[%d] doesn't match with the command delay number [%d]\r\n",
                    model_index, __FUNCTION__, word_number, p_header->command_delay_num-1);
            }
        }
    }
    // for net with 'on/off' pairs, run stage 2 if over than one cmd is detected by S1
    // the 'on/off' pais are 'wakup cmd'
    if(p_extra_setting->on_cmd_number)
    {
        cmd_number_to_reconfim[model_index] = 1;
    }
    else
    {
        cmd_number_to_reconfim[model_index] = 0;
    }
    return 0;
}

int nn_userdefine_s2_confirm_set(int model_index, int s2_reconfirm_frames) {
    s2_reconfirm_valid_frames[model_index] = s2_reconfirm_frames;
    last_s1_wake_frames[model_index] = s2_reconfirm_frames;
    return 0;
}
