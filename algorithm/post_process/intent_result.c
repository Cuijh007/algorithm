#include <stdint.h>
#include "iet_common.h"
#include "intent_result.h"
#include "mid_pp.h"


#define SLOT_DATA_NUM  	32
#define MAX_INTENT_NUM	8

static IntentResultInstance intentResultInst;

/**
 * @brief pinyin to number dict
 * number_dict = {'ling': 0, 'ban': 0.5, 'yi': 1, '$0er': 2, 'er': 2, 'lia': 2, 'liang': 2, 'san': 3,
                  'si': 4, 'wu': 5, 'liu': 6, 'qi': 7, 'ba': 8, 'jiu': 9, 'shi': 10, 'bai': 100, 'qian': 1000}
 */
#define NUMBER_DICT_NUM  17

int16_t number_dict[NUMBER_DICT_NUM][2] = {{188, 0}, {375, 1}, {90, 2}, {182, 2}, {184, 2}, 
                     {294, 3}, {320, 4}, {355, 5}, {189, 6}, {264, 7},
                     {12, 8}, {148, 9}, {310, 10}, {14, 5}, {74, 1}, {13, 100}, {266, 1000}};

void process_optional_slot(int valid_slots_num, char *optional_slots, int16_t *valid_commands){
    for(int m = 0; m < valid_slots_num; m++)
    {
        if ((m+1) == valid_slots_num)
            sprintf(optional_slots, "%s%d", optional_slots, valid_commands[m]);	
        else
            sprintf(optional_slots, "%s%d_", optional_slots, valid_commands[m]);	
    }
}

void process_intent_str(int valid_slots_num, char *intent_str, int16_t *valid_commands, int intent_valid_idx, int16_t slot_type_id, int16_t slot_unit){
    sprintf(intent_str, "%s%d<%d:", intent_str, intent_valid_idx, slot_type_id);
    for(int m = 0; m < valid_slots_num; m++)
    {
        if ((m+1) == valid_slots_num)
            sprintf(intent_str, "%s%d%c>", intent_str, valid_commands[m], slot_unit);	
        else
            sprintf(intent_str, "%s%d_", intent_str, valid_commands[m]);	
    }			
    sprintf(intent_str, "%s|", intent_str);
}

/**
 * @brief Get the private slot item object
 * 		  遍历私有数据中所有 slots,查找 is_common = false 的私有数据，
 * 	      再查找 intent_ids 数组中是否包含指定的 intent_valid_index (其中 0 为无效值) 
 * 		  找到对应 slot 后返回
 * 			
 * @param p_inst 
 * @param slot_id_list 
 * @param intent_valid_index 
 * @param max_slot_num 
 * @return ** void 
 */
void get_private_slot_item(IntentResultInstance *p_inst, int16_t *slot_id_list, int intent_valid_index, int max_slot_num)
{
    
    int16_t slots_num = p_inst->slots_type_num;
    int16_t is_common;
    Slot *slot;
    int16_t search_slot_index = 0;
    for(int i = 0; i < slots_num; i++){
        int16_t search_slot_flag = -1;
        slot = &p_inst->slots[i];
        is_common = slot->is_common;
        if(is_common == 1){
            continue;			
        }

        for(int j = 0; j < MAX_PRIVATE_SLOT_SUPPORT_INTENT_NUM; j++){
            if(slot->intent_ids[j] == 0){
                break;
            }

            if(slot->intent_ids[j] == intent_valid_index){
                search_slot_flag = 1;
                break;
            }
        }

        if(search_slot_flag != -1){
            slot_id_list[search_slot_index] = slot->id;
            search_slot_index += 1;                       
        }
        if(search_slot_index == max_slot_num){
            break;
        }
    }
    return;
}

int get_start_end_index(int16_t *slots, int16_t *commands, int16_t *valid_commands, int16_t start_bio, int16_t continue_bio){
    int start_idx = -1;
    int end_idx = -1;

    for(int i = 0; i < 16; i++){
        //iet_print(IET_PRINT_ALG_DBG, ":%d \n", slots[i*2]);
        if(slots[i*2] == start_bio){
            start_idx = i;
            end_idx = i+1;
            break;
        }        
    }
    while(end_idx < 16 && slots[end_idx*2] == continue_bio){
        end_idx += 1;
    }
    if(start_idx != -1){
        for(int i = start_idx; i < end_idx; i++){
            valid_commands[i-start_idx] = commands[i];
        }
    }
    // iet_print(IET_PRINT_ALG_DBG, "end_idx : %d, start_idx : %d\n", end_idx, start_idx);
    return end_idx - start_idx;
}

void process_general_slot(int16_t *count_option, int16_t *slot, 
                          int16_t *commands, int16_t *valid_commands, char *optional_slots,
                          int16_t start_bio, int16_t continue_bio, int16_t slot_type){
    
    int16_t valid_slots_num = 0;
    valid_slots_num = get_start_end_index(slot, commands, valid_commands, start_bio, continue_bio);
    if(valid_slots_num){	
        if(*count_option > 0)
            sprintf(optional_slots, "%s,%d:", optional_slots, slot_type);	
        else
            sprintf(optional_slots, "%s%d:", optional_slots, slot_type);
        process_optional_slot(valid_slots_num, optional_slots, valid_commands);
        *count_option += 1;
    }
}

floating_s covert_pinyin_to_number(int16_t *valid_slots, int valid_slots_num){
    int16_t pinyi_dian = 74;
    int int_part = 0;
    int decial_part = 0;
    int result_num = 1;
    floating_s result;

    for(int i = 0; i < valid_slots_num; i++){  
        if(valid_slots[i] != pinyi_dian){
            int_part += result_num;
            decial_part += (1 - result_num);
        }   
        if(valid_slots[i] == pinyi_dian){
            result_num = 0;
        }
    }
    result_num = 0;

    //iet_print(IET_PRINT_ALG_DBG,"int_part : %d, decial_part : %d\n", int_part, decial_part);
    if(int_part == 1){   /*one number, includeing 10, 100*/        
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                result_num = number_dict[j][1];
                break;
            }
        }        
    }else if(valid_slots[0] == number_dict[12][0]){ /*11 ~ 19*/
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[1]){
                result_num = 10 + number_dict[j][1];
                break;
            }
        }
    }else if(int_part == 2 && valid_slots[1] == number_dict[12][0]){ /*10,20,30,40...*/
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                result_num = 10 * number_dict[j][1];
                break;
            }
        }
     }else if(int_part == 2 && valid_slots[1] == number_dict[15][0]){ /*bai... 100, 200, 300...*/
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                result_num = 100 * number_dict[j][1];
                break;
            }
        }
    }else if(int_part == 2 && valid_slots[1] == number_dict[16][0]){ /*qian... 1000, 2000, 3000...*/
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                result_num = 1000 * number_dict[j][1];
                break;
            }
        }
    }else if(int_part == 2 && valid_slots[0] == number_dict[0][0]){ /* 零四 取 4*/
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[1]){
                result_num = number_dict[j][1];
                break;
            }
        }
    }else if(int_part == 2){ /*降低个三四度 取三*/
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                result_num = number_dict[j][1];
                break;
            }
        }
    }else if(int_part == 3 && valid_slots[1] == number_dict[12][0]){ /*others like 21, 22, 33, 34...*/
        int ten_num = 0;
        int ge_num = 0;
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                ten_num = number_dict[j][1];                
            }
            if(number_dict[j][0] == valid_slots[2]){
                ge_num = number_dict[j][1];
            }
        }
        result_num = 10 * ten_num + ge_num;
    }else if(int_part == 4 && valid_slots[1] == number_dict[15][0] && valid_slots[3] == number_dict[12][0]){
        /*yi bai wu shi 150*/
        int bai_num = 0;
        int ten_num = 0;
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                bai_num = number_dict[j][1];                
            }
            if(number_dict[j][0] == valid_slots[2]){
                ten_num = number_dict[j][1];
            }
        }
        result_num = 100 * bai_num + 10 * ten_num;
    }else if(int_part == 4 && valid_slots[1] == number_dict[15][0] && valid_slots[2] == number_dict[0][0]){
        /*er bai ling wu 205*/
        int bai_num = 0;
        int ge_num = 0;
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                bai_num = number_dict[j][1];                
            }
            if(number_dict[j][0] == valid_slots[3]){
                ge_num = number_dict[j][1];
            }
        }
        result_num = 100 * bai_num + ge_num;
    }else if(int_part == 4 && valid_slots[1] == number_dict[16][0] && valid_slots[2] == number_dict[0][0]){
        /*yi qian ling wu 1005*/
        int qian_num = 0;
        int ge_num = 0;
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                qian_num = number_dict[j][1];                
            }
            if(number_dict[j][0] == valid_slots[3]){
                ge_num = number_dict[j][1];
            }
        }
        result_num = 1000 * qian_num + ge_num;
    }else if(int_part == 4 && valid_slots[1] == number_dict[16][0] && valid_slots[3] == number_dict[15][0]){
        /*yi qian jiu bai 1900*/
        int qian_num = 0;
        int bai_num = 0;
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                qian_num = number_dict[j][1];                
            }
            if(number_dict[j][0] == valid_slots[2]){
                bai_num = number_dict[j][1];
            }
        }
        result_num = 1000 * qian_num + 100 * bai_num;
    }else if(int_part == 5 && valid_slots[1] == number_dict[15][0] && valid_slots[3] == number_dict[12][0]){
        /*yi bai wu shi liu 156*/
        int bai_num = 0;
        int ten_num = 0;
        int ge_num = 0;
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                bai_num = number_dict[j][1];                
            }
            if(number_dict[j][0] == valid_slots[2]){
                ten_num = number_dict[j][1];
            }
            if(number_dict[j][0] == valid_slots[4]){
                ge_num = number_dict[j][1];
            }
        }
        result_num = 100 * bai_num + 10 * ten_num + ge_num;
    }else if(int_part == 5 && valid_slots[1] == number_dict[16][0] && valid_slots[2] == number_dict[0][0] && valid_slots[3] == number_dict[12][0]){
        /*yi qian ling wu shi 1050*/
        int qian_num = 0;
        int ten_num = 0;
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                qian_num = number_dict[j][1];                
            }
            if(number_dict[j][0] == valid_slots[4]){
                ten_num = number_dict[j][1];
            }
        }
        result_num = 1000 * qian_num + 10 * ten_num;
    }else if(int_part == 6 && valid_slots[1] == number_dict[16][0] && valid_slots[2] == number_dict[0][0] && valid_slots[3] == number_dict[12][0]){
        /*yi qian ling wu shi liu 1056*/
        int qian_num = 0;
        int ten_num = 0;
        int ge_num = 0;
        for(int j = 0; j < NUMBER_DICT_NUM; j++){
            if(number_dict[j][0] == valid_slots[0]){
                qian_num = number_dict[j][1];                
            }
            if(number_dict[j][0] == valid_slots[4]){
                ten_num = number_dict[j][1];
            }
            if(number_dict[j][0] == valid_slots[5]){
                ge_num = number_dict[j][1];
            }
        }
        result_num = 1000 * qian_num + 10 * ten_num + ge_num;
    }else if((int_part == 5 && valid_slots[1] == number_dict[16][0] && valid_slots[3] == number_dict[15][0]) 
            || int_part == 6 && valid_slots[1] == number_dict[16][0] && valid_slots[3] == number_dict[15][0] && valid_slots[5] == number_dict[12][0]){
        /* yi qian jiu bai wu 1950 or yi qian jiu bai wu*/
        int qian_num = 0;
        int bai_num = 0;
        int ten_num = 0;
        for(int j = 0; j < 15; j++){
            if(number_dict[j][0] == valid_slots[0]){
                qian_num = number_dict[j][1];                
            }
            if(number_dict[j][0] == valid_slots[2]){
                bai_num = number_dict[j][1];
            }
            if(number_dict[j][0] == valid_slots[4]){
                ten_num = number_dict[j][1];
            }
        }
        result_num = 1000 * qian_num + 100 * bai_num + 10 * ten_num;
    }else if(int_part == 7 && valid_slots[1] == number_dict[16][0] && valid_slots[3] == number_dict[15][0] && valid_slots[5] == number_dict[12][0]){
        /*yi qian jiu bai wu shi san 1933*/
        int qian_num = 0;
        int bai_num = 0;
        int ten_num = 0;
        int ge_num = 0;
        for(int j = 0; j < 15; j++){
            if(number_dict[j][0] == valid_slots[0]){
                qian_num = number_dict[j][1];                
            }
            if(number_dict[j][0] == valid_slots[2]){
                bai_num = number_dict[j][1];
            }
            if(number_dict[j][0] == valid_slots[4]){
                ten_num = number_dict[j][1];
            }
            if(number_dict[j][0] == valid_slots[6]){
                ge_num = number_dict[j][1];
            }
        }
        result_num = 1000 * qian_num + 100 * bai_num + 10 * ten_num + ge_num;
    }

    result.int_part = result_num;
    //iet_print(IET_PRINT_ALG_DBG, "result_num : %d\n", result_num);
    /*process decimal part*/
    result.decimal_part = 0;
    if(decial_part > 0){
        result_num = 0;
        for(int i = 0; i < decial_part; i++){    
            int start_idx = int_part + 1;
            for(int j = 0; j < NUMBER_DICT_NUM; j++){
                if(number_dict[j][0] == valid_slots[start_idx+i]){
                    result_num = result_num*10 + number_dict[j][1];
                    break;
                }
            }
        }
        //iet_print(IET_PRINT_ALG_DBG, "result_num : %d\n", result_num);
        result.decimal_part = result_num;
    }
    
    return result;
}

time_s convert_time_amount(int16_t *valid_slots, int valid_slots_num){
    int16_t ban_pinyin = 14;
    int16_t ge_pinyin = 106;
    int16_t xiaoshi_pinyin[2] = {360, 310};
    int16_t zhongtou_pinyin[2] = {404, 341};

    int16_t ke_pinyin = 158;
    int16_t fen_pinyin = 96;
    int16_t miao_pinyin = 211;
    int16_t zhong_pinyin = 404;
    int16_t tian_pinyin = 336;
    
    int result_day = 0;
    int result_hour = 0;
    int result_min = 0;
    int result_sec = 0;
    
    int minutes_idx = 0;
    int second_idx = 0;
    for(int i = 0; i < valid_slots_num; i++){

        if(valid_slots[i] == tian_pinyin){
            // 1 day ~ 9 day
            int16_t day_command[16] = {0};
            day_command[0] = valid_slots[i-1];
            floating_s tmp_result = covert_pinyin_to_number(day_command, i);
            result_day = tmp_result.int_part;
        }

        /*process hour 半个小时、 1个小时、1小时、 1个半小时*/
        if(valid_slots[i] == ge_pinyin){
            if((i+2) < valid_slots_num && 
                ((valid_slots[i+1] == xiaoshi_pinyin[0] && valid_slots[i+2] == xiaoshi_pinyin[1]) || 
                (valid_slots[i+1] == zhongtou_pinyin[0] && valid_slots[i+2] == zhongtou_pinyin[1]) ||
                (valid_slots[i+1] == ban_pinyin && valid_slots[i+2] == xiaoshi_pinyin[0] && valid_slots[i+3] == xiaoshi_pinyin[1]) ||
                (valid_slots[i+1] == ban_pinyin && valid_slots[i+2] == zhongtou_pinyin[0] && valid_slots[i+3] == zhongtou_pinyin[1]))){
                int16_t hour_command[16] = {0};
                for(int j = 0; j < i; j++){
                    hour_command[j] = valid_slots[j];
                    if(hour_command[j] == ban_pinyin)
                    {
                        result_min = 30;
                    }
                    //iet_print(IET_PRINT_ALG_DBG, "%d\n", hour_command[j]);
                }
                if(!result_min && i > 0){
                    floating_s tmp_result = covert_pinyin_to_number(hour_command, i);
                    result_hour = tmp_result.int_part;
                    if(tmp_result.decimal_part/10 == 0){
                        result_min = 60*tmp_result.decimal_part/10;
                    }else if(tmp_result.decimal_part/100 == 0){
                        result_min = 60*10*tmp_result.decimal_part/100;
                    }else if(tmp_result.decimal_part/1000 == 0){
                        result_min = 60*100*tmp_result.decimal_part/1000;
                    }					
                }	
                
                if(valid_slots[i+1] == ban_pinyin){
                    result_min = 30;
                    i+=1;
                }	
                i+=2;
                minutes_idx = i+1;	
                //iet_print(IET_PRINT_ALG_DBG, "%dh %dm\n", result_hour, result_min);
            }

        }

        if((i+1) < valid_slots_num && 
            ((valid_slots[i] == xiaoshi_pinyin[0] && valid_slots[i+1] == xiaoshi_pinyin[1]) || 
            (valid_slots[i] == zhongtou_pinyin[0] && valid_slots[i+1] == zhongtou_pinyin[1]))){
            int16_t hour_command[16] = {0};
            for(int j = 0; j < i; j++){
                hour_command[j] = valid_slots[j];
                if(hour_command[j] == ban_pinyin)
                {
                    result_min = 30;
                }
                //iet_print(IET_PRINT_ALG_DBG, "%d\n", hour_command[j]);
            }
            if(!result_min && i > 0){
                floating_s tmp_result = covert_pinyin_to_number(hour_command, i);
                result_hour = tmp_result.int_part;
                if(tmp_result.decimal_part/10 == 0){
                    result_min = 60*tmp_result.decimal_part/10;
                }else if(tmp_result.decimal_part/100 == 0){
                    result_min = 60*10*tmp_result.decimal_part/100;
                }else if(tmp_result.decimal_part/1000 == 0){
                    result_min = 60*100*tmp_result.decimal_part/1000;
                }					
                
            }	
            i+=1;
            minutes_idx = i+1;
            //iet_print(IET_PRINT_ALG_DBG, "%dh %dm\n", result_hour, result_min);
        }

        /*process 分钟*/		
        if(((valid_slots[i] == ke_pinyin)|| (valid_slots[i] == fen_pinyin))){
            int16_t minute_command[16] = {0};
            //iet_print(IET_PRINT_ALG_DBG, " minutes_idx : %d\n", minutes_idx);
            for(int j = minutes_idx; j < i; j++){
                minute_command[j-minutes_idx] = valid_slots[j];
                if(minute_command[j-minutes_idx] == ban_pinyin)
                {
                    if(valid_slots[i] == ke_pinyin){
                        result_min = 7;
                    }else{
                        result_sec = 30;
                    }
                }
                //iet_print(IET_PRINT_ALG_DBG, "%d\n", minute_command[j-minutes_idx]);
            }
            if(!result_sec && !result_min && (i-minutes_idx) > 0){
                floating_s tmp_result = covert_pinyin_to_number(minute_command, i-minutes_idx);
                if(valid_slots[i] == ke_pinyin){
                    result_min = tmp_result.int_part*15;
                }else{
                    result_min = tmp_result.int_part;
                    if(tmp_result.decimal_part/10 == 0){
                        result_sec = 60*tmp_result.decimal_part/10;
                    }else if(tmp_result.decimal_part/100 == 0){
                        result_sec = 60*10*tmp_result.decimal_part/100;
                    }else if(tmp_result.decimal_part/1000 == 0){
                        result_sec = 60*100*tmp_result.decimal_part/1000;
                    }	
                }			
            }
            if(valid_slots[i+1] == zhong_pinyin){
                i+=1;
            }
            second_idx = i+1;
        }

        /*process second of time*/
        if(valid_slots[i] == miao_pinyin){
            int16_t second_command[16] = {0};
            //iet_print(IET_PRINT_ALG_DBG, " second_idx : %d\n", second_idx);
            for(int j = second_idx; j < i; j++){
                second_command[j-second_idx] = valid_slots[j];
                if(second_command[j-second_idx] == ban_pinyin)
                {
                    result_sec = 0;
                }
                //iet_print(IET_PRINT_ALG_DBG, "%d\n", second_command[j-second_idx]);
            }
            if((i-second_idx) > 0){
                floating_s tmp_result = covert_pinyin_to_number(second_command, i-second_idx);
                result_sec = tmp_result.int_part;							
            }
            if(valid_slots[i+1] == zhong_pinyin){
                i+=1;
            }
            i+=1;
        }
    }
    //iet_print(IET_PRINT_ALG_DBG, "%dh %dm %ds\n", result_hour, result_min, result_sec);

    time_s result_time;
    result_time.day = result_day;
    result_time.hour = result_hour;
    result_time.minute = result_min;
    result_time.second = result_sec;
    return result_time;
}

time_s convert_time(int16_t *valid_slots, int valid_slots_num){
    int16_t ban_pinyin = 14;
    int16_t dian_pinyin = 74;

    int16_t ke_pinyin = 158;
    int16_t fen_pinyin = 96;
    int16_t miao_pinyin = 211;
    int16_t zhong_pinyin = 404;

    int result_hour = 0;
    int result_min = 0;
    int result_sec = 0;
    
    int minutes_idx = 0;
    int second_idx = 0;
    for(int i = 0; i < valid_slots_num; i++){
        /*process hour 九点*/
        if(valid_slots[i] == dian_pinyin){
            
            int16_t hour_command[16] = {0};
            for(int j = 0; j < i; j++){
                hour_command[j] = valid_slots[j];
            }
            floating_s tmp_result = covert_pinyin_to_number(hour_command, i);
            result_hour = tmp_result.int_part;
            minutes_idx = i+1;			
        }

        /*process 分钟*/		
        if((valid_slots[i] == ke_pinyin) || (valid_slots[i] == fen_pinyin) || (valid_slots[i] == ban_pinyin) || ((i+1) == valid_slots_num)){
            int16_t minute_command[16] = {0};
            for(int j = minutes_idx; j <= i; j++){
                minute_command[j-minutes_idx] = valid_slots[j];				
            }
            if(valid_slots[i] == ban_pinyin){
                result_min = 30;
            }
            int len = i-minutes_idx;
            if((i+1) == valid_slots_num){
                len += 1;
            }
            // 去掉 fen
            if(minute_command[len-1] == fen_pinyin){
                minute_command[len-1] = 0;
                len = len - 1;
            }
            if(!result_sec && !result_min && len > 0){
                floating_s tmp_result = covert_pinyin_to_number(minute_command, len);
                if(valid_slots[i] == ke_pinyin){
                    result_min = tmp_result.int_part*15;
                }else{
                    result_min = tmp_result.int_part;					
                }			
            }
            if(valid_slots[i+1] == zhong_pinyin){
                i+=1;
            }
            second_idx = i+1;
            break;
        }		
    }
    // iet_print(IET_PRINT_ALG_DBG, "%dh %dm %ds\n", result_hour, result_min, result_sec);

    time_s result_time;
    result_time.hour = result_hour;
    result_time.minute = result_min;
    result_time.second = result_sec;
    return result_time;
}


time_s convert_overall_time(int16_t *slots, int16_t *commands, int16_t *valid_commands, 
    int16_t B_day_bio, int16_t I_day_bio, int16_t B_clock_bio, int16_t I_clock_bio, 
    int16_t B_period_bio, int16_t I_period_bio, int *time_flag){
    /*
    转换完整的时间，包括day，period和clock
    此函数可能用于开始时间或者结束时间，由slot的名字，也就是函数传入的参数名而定，开始或者结束时间计算的具体逻辑是相同的
    */
        
    // 具体时间，比如“今天下午十一点半开机”中的“今天下午十一点半”
    /*day e.g. 今天、明天*/

    int valid_slots_num = 0;
    valid_slots_num = get_start_end_index(slots, commands, valid_commands, B_day_bio, I_day_bio);

    int16_t day_num = 0;
    if(valid_slots_num){
        int16_t jin_piyin = 145;
        int16_t ming_pinyin = 214;
        int16_t da_pinyin = 63;
        int16_t hou_pinyin = 130;
        int16_t tian_pinyin = 336;
        if(valid_commands[0] == jin_piyin){
            day_num = 0;
        }else if(valid_commands[0] == ming_pinyin){
            day_num = 1;
        }else if(valid_commands[0] == hou_pinyin){
            day_num = 2;
        }else if(valid_commands[0] == da_pinyin && valid_commands[1] == hou_pinyin){
            day_num = 3;
        }
        *time_flag = 1;
    }

    // time e.g. 五点五分
    valid_slots_num = get_start_end_index(slots, commands, valid_commands, B_clock_bio, I_clock_bio);	
    if(valid_slots_num){
        *time_flag = 1;
    }
    time_s result_time = convert_time(valid_commands, valid_slots_num);
    // time_period, e.g. 早上、下午、晚上
    valid_slots_num = get_start_end_index(slots, commands, valid_commands, B_period_bio, I_period_bio);
    if(valid_slots_num){
        int16_t zhong_piyin = 404;
        int16_t xia_pinyin = 357;
        int16_t wu_pinyin = 355;
        int16_t bang_pinyin = 15;
        int16_t wan_pinyin = 349;
        int16_t shang_pinyin = 304;
        int16_t ban_pinyin = 14;
        int16_t ye_pinyin = 374;

        if(valid_commands[0] == zhong_piyin && valid_commands[1] == wu_pinyin){
            if(result_time.hour >= 1 && result_time.hour <= 3){
                result_time.hour += 12;
            }
        }else if(valid_commands[0] == xia_pinyin && valid_commands[1] == wu_pinyin){
            if(result_time.hour >= 1 && result_time.hour <= 7){
                result_time.hour += 12;
            }
        }else if(valid_commands[0] == wan_pinyin){
            if(result_time.hour >= 6 && result_time.hour <= 12){
                result_time.hour += 12;
            }
        }else if(valid_commands[0] == bang_pinyin && valid_commands[1] == wan_pinyin){
            if(result_time.hour >= 6 && result_time.hour <= 8){
                result_time.hour += 12;
            }
        }else if(valid_commands[0] == ban_pinyin && valid_commands[1] == ye_pinyin){
            if(result_time.hour >= 10 && result_time.hour <= 12){
                result_time.hour += 12;
            }
        }
        *time_flag = 1;
    }
    // 晚上 12 点记为 0 点 24 点 为 1d
    // time['d'] = int((time_num['h'] + added_hour) / 24) + time['d']
    // time['h'] = (time_num['h'] + added_hour) % 24

    result_time.day = result_time.hour / 24 + day_num;
    result_time.hour = result_time.hour % 24;

    return result_time;
}

extern int16_t eos_offset_index;
/*
 * Parse out of intent net
 * Parameters:
 * word_scores[in]: net out of intent, concat of intent part and slot.
 * commands[in]:	intput word IDs of intent net, needed by parse here also
 * intent_str[out]:
 *      [result_in_string<max 128 short>][original_out of intent net<max 128 short>]
 *        +--> eg. 
 * 				 [|34-30C|<FL:234_345,DS:0h>....]                              +
 *               [| A:8*2 for intent| B: 16*2 for slot | C: 16 word ids|...]   
 */
int32_t nlp_intent_result(MidHandle handle, int16_t *word_scores, int16_t *commands, char *intent_str)
{
    IntentResultInstance *p_inst = (IntentResultInstance*)handle;

    int16_t nlp_intent_out[128] = {0};

    int16_t intent_num = p_inst->intent_num;
    int pred_intent_num = 0;
    int16_t *intent_out = word_scores;

    for(int j = 0; j < intent_num; j++){
        if(intent_out[j] > p_inst->intent_thresholds[j]){
            iet_print(IET_PRINT_ALG_DBG,"id: %d, score: %d ,threshold: %d \r\n", j, intent_out[j], p_inst->intent_thresholds[j]);
            nlp_intent_out[pred_intent_num*2] = j;
            nlp_intent_out[pred_intent_num*2+1] = intent_out[j];
            pred_intent_num++;
        }
    }
    
    for(int j = pred_intent_num; j < MAX_INTENT_NUM; j++){
        nlp_intent_out[j*2] = -1;
        nlp_intent_out[j*2 + 1] = -1;
    }

    int16_t intent_score_after_pad_num = ((intent_num + 15)/16)*16;
    int16_t *slot_out = word_scores + intent_score_after_pad_num;


    for(int i = 0; i < eos_offset_index; i++){    
        int16_t max_index = slot_out[i*2];
        int16_t max_score = slot_out[i*2+1];
        nlp_intent_out[(i+8)*2] = max_index;
        nlp_intent_out[(i+8)*2+1] = max_score;
    }

    memset(word_scores, 0, NLP_OUT_SIZE*sizeof(int16_t));
    //word_scores前128 int16_t用于存储字符串结果,后128 int16_t带出intent_out、slot_out、intent_input
    //前16个int16_t是规整化后的intent_out，其后32个int16_t是规整化后的slot_out，故为48*sizeof(int16_t)
    // memcpy(word_scores + NLP_OUT_SIZE/2, nlp_intent_out, 48*sizeof(int16_t));
    iet_shell_memcpy32(word_scores + NLP_OUT_SIZE/2, nlp_intent_out, sizeof(int16_t) * 48);
    //intent_input:
    // memcpy(word_scores + NLP_OUT_SIZE/2 + 48, commands, 16*sizeof(int16_t));
    iet_shell_memcpy32(word_scores + NLP_OUT_SIZE/2 + 48, commands, sizeof(int16_t) * 16);

    int16_t index, score;
    int8_t intent_valid_num = 0;
    int16_t intent_valid[8] = {0};

    //当前 前端留足了128个int16_t，用作char，故置256*sizeof(char) = 256 byte为0
    memset(intent_str, 0, NLP_OUT_SIZE*sizeof(char));
    
    intent_str[0] = '|';
        
    /*intent result*/
    for(int i = 0; i < 8; i++){
        index = nlp_intent_out[i*2];
        score = nlp_intent_out[i*2 + 1];
        if(nlp_intent_out[i*2] != -1 && nlp_intent_out[i*2] != 0){
            intent_valid[intent_valid_num] = index;
            intent_valid_num++;
            //iet_print(IET_PRINT_ALG_DBG, "[%s][%d] Intent [%d][%d]\r\n", __FUNCTION__, __LINE__, index, score);
        }
    }
    // iet_print(IET_PRINT_ALG_DBG, "intent_valid_num: %d \r\n", intent_valid_num);
    /*slot analysis*/
    if(intent_valid_num == 0){
        if(intent_str[1] == 0){
            sprintf(intent_str, "|0|");
        }
        return PP_RESULT_SILENT;
    }
    
    // process common slots: floor, location, device, time, duration
    // append as <FL:san, LO:ke ting, DE:tong deng, TI:1d 8h 30m, DU:0d 10h 20m>
    char optional_slots[128] = "<";
    // 用于计数通用 slot, 控制显示结果
    int16_t count_option = 0;
    int16_t valid_slots_num = 0;
    int16_t valid_commands[16] = {0};

    /*process 通用 slot: floor, location, device part, prop, substance, etc*/

    Slot common_slot;
    Slot_BI slot_bi;
    for(int i = 0; i < p_inst->slots_type_num; i++){
        common_slot = p_inst->slots[i];
        if(common_slot.is_common == 1){

            if(common_slot.type == SLOT_PARSE_TYPE_CHARACTER){
                // floor location device part prop ....
                slot_bi = common_slot.bi_groups[0];
                process_general_slot(&count_option, &nlp_intent_out[16], commands, valid_commands, optional_slots,
                                      slot_bi.B_id, slot_bi.I_id, common_slot.id);
            }else if(common_slot.type == SLOT_PARSE_TYPE_TIME){		
                // 时间段-开始, 比如“四小时后关机”中的“四小时”,以duration start表示，缩写DS
                // 时间段-结束, 比如“最大热风吹四小时关机”中的“四小时” 以duration end，缩写DD
                Slot_BI slot_bi_du = common_slot.bi_groups[0];
                if(common_slot.bi_groups[1].B_id == 0){
                    valid_slots_num = get_start_end_index(&nlp_intent_out[16], commands, valid_commands, slot_bi_du.B_id, slot_bi_du.I_id);
                    if(valid_slots_num){
                        time_s result_time = convert_time_amount(valid_commands, valid_slots_num);
                        if(count_option > 0)
                            sprintf(optional_slots, "%s,%d:", optional_slots, common_slot.id);	
                        else
                            sprintf(optional_slots, "%s%d:", optional_slots, common_slot.id);

                        sprintf(optional_slots, "%s%dd_%dh_%dm_%ds", optional_slots, result_time.day, result_time.hour, result_time.minute, result_time.second);
                        
                        count_option += 1;
                    }
                }else{
                    Slot_BI slot_bi_day = common_slot.bi_groups[0];
                    Slot_BI slot_bi_clock = common_slot.bi_groups[1];
                    Slot_BI slot_bi_period = common_slot.bi_groups[2];

                    //具体时间-开始，比如“今天下午十一点半开机”中的“今天下午十一点半”,以time start表示，缩写TS
                    //具体时间-结束，比如“开机到今天下午十一点半”中的“今天下午十一点半”,以time end表示，缩写TE
                    int time_flag = 0;
                    time_s result_time = convert_overall_time(&nlp_intent_out[16], commands, valid_commands, 
                                                            slot_bi_day.B_id, slot_bi_day.I_id, slot_bi_clock.B_id, slot_bi_clock.I_id, 
                                                            slot_bi_period.B_id, slot_bi_period.I_id, &time_flag);
                    
                    //|3|<DE:dian_re_shui_qi,TS:0d_0h_0m_0s,AL:qvan_bu> 零点
                    if(time_flag == 1){
                        if(count_option > 0)
                            sprintf(optional_slots, "%s,%d:%dd_%dh_%dm_%ds", optional_slots, common_slot.id, result_time.day, result_time.hour, result_time.minute, result_time.second);
                        else
                            sprintf(optional_slots, "%s%d:%dd_%dh_%dm_%ds", optional_slots, common_slot.id, result_time.day, result_time.hour, result_time.minute, result_time.second);

                        count_option += 1;
                    }

                }
            }
        }

    }

    // END optional_slots  
    sprintf(optional_slots, "%s>", optional_slots);

    Slot slot;
    // process intent with private slot
    for(int i = 0; i < intent_valid_num; i++){
        /**
         *  "id": 18,
            "name": "percent",
            "intent_ids": [17,31],

            "id": 24,
            "name": "fraction",
            "intent_ids": [17,31],
         * 
         */
        // maybe multiple, max 5
        int16_t max_slot_num = 5;
        int16_t slot_id_list[max_slot_num];
        memset(slot_id_list, -1, sizeof(slot_id_list));
        get_private_slot_item(p_inst, slot_id_list, intent_valid[i], max_slot_num);

        if(slot_id_list[0] == -1){
            sprintf(intent_str, "%s%d|", intent_str, intent_valid[i]);
            continue;
        }

        int16_t slot_id = -1;
        int16_t slot_parse_type = -1;
        for(int s_i = 0; s_i < max_slot_num; s_i ++){

            slot_id = slot_id_list[s_i];

            if(slot_id == -1){
                break;
            }

            slot = p_inst->slots[slot_id];
            slot_parse_type = slot.type;

            int16_t valid_commands[16] = {0};
            int16_t valid_slots_num = 0;
            Slot_BI slot_bi = slot.bi_groups[0];

            valid_slots_num = get_start_end_index(&nlp_intent_out[16], commands, valid_commands, slot_bi.B_id, slot_bi.I_id);	

            // 通过 slot 私有数据解析类型进行处理
            if(slot_parse_type == SLOT_PARSE_TYPE_CHARACTER){ 
                /**
                 * process color # has color,  # e.g. |44<CL:hong se >|
                 */
                if(valid_slots_num)
                    process_intent_str(valid_slots_num, intent_str, valid_commands, intent_valid[i], slot.id, slot.unit);
                else
                    sprintf(intent_str, "%s%d|", intent_str, intent_valid[i]);

            }else if(slot_parse_type == SLOT_PARSE_TYPE_NUMBER_UNIT){
                /* process gear # has gear, e.g. |40<GE:3G>| |9<WT:800W>|*/
                
                if(valid_slots_num){
                    floating_s result = covert_pinyin_to_number(valid_commands, valid_slots_num);
                    if(result.decimal_part != 0)
                        sprintf(intent_str, "%s%d<%d:%d.%d%c>|", intent_str, intent_valid[i], slot.id, result.int_part, result.decimal_part, (slot.unit));
                    else
                        sprintf(intent_str, "%s%d<%d:%d%c>|", intent_str, intent_valid[i], slot.id, result.int_part, slot.unit);
                }else
                    sprintf(intent_str, "%s%d|", intent_str, intent_valid[i]);

            }else if(slot_parse_type == SLOT_PARSE_TYPE_PERCENT || (slot_parse_type == SLOT_PARSE_TYPE_FRACTION && slot_id == 18)){
                /**
                 * 兼容旧版本: 当 18 percent 的 type 是 fraction 时
                 *  "id": 18,
                    "name": "percent",
                    "abbr": "PC",
                * process percent has percent,  # e.g. |42<PR:50P>| 	e.g. 'bai fen zhi san shi'
                */
                if(valid_slots_num){
                    
                    int16_t fen_pinyin_id = 96;
                    int16_t zhi_pinyin_id = 403;

                    int16_t valid_commands_percent[16] = {0};
                    int16_t valid_slots_num_percent = 0;

                    int16_t percent_flag[2] = {0};
                    for(int z=0; z < valid_slots_num; z++){
                        if (fen_pinyin_id == valid_commands[z])
                            percent_flag[0] = 1;
                        if (zhi_pinyin_id == valid_commands[z])
                            percent_flag[1] = 1;						

                        if(percent_flag[0] == 1 && percent_flag[1] == 1 && valid_commands[z+1] !=0){
                            valid_commands_percent[valid_slots_num_percent] = valid_commands[z+1];
                            valid_slots_num_percent += 1;
                        }						
                    }
                    valid_slots_num = valid_slots_num_percent;
                    floating_s result = covert_pinyin_to_number(valid_commands_percent, valid_slots_num);
                    sprintf(intent_str, "%s%d<%d:%d%c>|", intent_str, intent_valid[i], slot.id, result.int_part, slot.unit);
                }
            }else if(slot_parse_type == SLOT_PARSE_TYPE_FRACTION){
                // 普通分数，X分之Y,或者一半转为二分之一
                if(valid_slots_num){
                    
                    int16_t fen_pinyin_id = 96;
                    int16_t zhi_pinyin_id = 403;

                    int16_t yi_pinyin_id = 375;
                    int16_t ban_pinyin_id = 14;
                    int16_t fraction_flag_yiban[2] = {0};

                    int16_t valid_commands_first[16] = {0};
                    int16_t valid_slots_num_percent_fisrt = 0;

                    int16_t valid_commands_second[16] = {0};
                    int16_t valid_slots_num_percent_second = 0;

                    int16_t fraction_flag[2] = {0};				
                    int16_t fen_index = -1;

                    for(int z=0; z < valid_slots_num; z++){
                        if (fen_pinyin_id == valid_commands[z]){
                            fen_index = z;
                            fraction_flag[0] = 1;
                        }else if (zhi_pinyin_id == valid_commands[z]){
                            fraction_flag[1] = 1;
                        }else if (yi_pinyin_id == valid_commands[z]){
                            fraction_flag_yiban[0] = 1;
                        }else if (ban_pinyin_id == valid_commands[z]){
                            fraction_flag_yiban[1] = 1;
                        }
                            
                        if(fraction_flag[0] == 1 && fraction_flag[1] == 1){
                            if(valid_commands[z+1] !=0){
                                // zhi yi -> find yi 
                                valid_commands_second[valid_slots_num_percent_second] = valid_commands[z+1];
                                valid_slots_num_percent_second += 1;
                            }else if(fen_index != -1){
                                // san fen -> find san
                                for(int f_i = 0; f_i < fen_index; f_i ++){
                                    valid_commands_first[valid_slots_num_percent_fisrt] = valid_commands[f_i];
                                    valid_slots_num_percent_fisrt +=1 ;
                                }
                            }
                        }						
                    }

                    if(valid_slots_num_percent_fisrt != 0 && valid_slots_num_percent_second !=0){
                        floating_s result_first = covert_pinyin_to_number(valid_commands_first, valid_slots_num_percent_fisrt);
                        floating_s result_scond = covert_pinyin_to_number(valid_commands_second, valid_slots_num_percent_second);
                        sprintf(intent_str, "%s%d<%d:%d_%d%c>|", intent_str, intent_valid[i], slot.id, result_scond.int_part, result_first.int_part, slot.unit);
                    }else if(fraction_flag_yiban[0] == 1 && fraction_flag_yiban[1] == 1){
                        // 一半 -> 1_2
                        sprintf(intent_str, "%s%d<%d:1_2%c>|", intent_str, intent_valid[i], slot.id, slot.unit);
                    }
                }
            }
        }
        
    }

    if(optional_slots[1] != '>')
        sprintf(intent_str, "%s%s", intent_str, optional_slots);
    
    if(intent_str[1] == 0){
        sprintf(intent_str, "|0|");
    }	

    iet_print(IET_PRINT_ALG_DBG, "Intent&Slot Predict: %s\r\n", intent_str);
    return PP_RESULT_VALID_CMD;
}

int32_t nlp_intent_result_init(IntentResultInitPara *p_init, MidHandle *mid_pp_inst)
{   
    *mid_pp_inst = (MidHandle)&intentResultInst;
    memset(&intentResultInst, 0, sizeof(IntentResultInstance));

    IntentPrivateData *intent_private_data = (IntentPrivateData*)p_init->p_private_data;
    intentResultInst.slots = &intent_private_data->slots[0];
    intentResultInst.intent_num = intent_private_data->intent_num;
    intentResultInst.slots_type_num = intent_private_data->slots_type_num;
    intentResultInst.intent_thresholds = p_init->intent_thresholds;

    return 0;
}