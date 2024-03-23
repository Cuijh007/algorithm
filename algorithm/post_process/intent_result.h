#ifndef __INTENT_RESULT_H__
#define __INTENT_RESULT_H__

#include "nn_file_struct.h"
#include "nn_common.h"
#include "mid_pp.h"

#define MAX_WORD_NUM                          16
#define MAX_BI_GROUP_NUM 5 
#define MAX_PRIVATE_SLOT_SUPPORT_INTENT_NUM 10

typedef struct{
    int16_t int_part;
    int16_t decimal_part;
}floating_s;

typedef struct{
    int16_t day;
    int16_t hour;
    int16_t minute;
    int16_t second;
}time_s;


typedef enum SlotParseType
{
	SLOT_PARSE_TYPE_CHARACTER	        = 0,    // character: e.g. |14<15:peng_ren >|<DE:kao_xiang> 
	SLOT_PARSE_TYPE_NUMBER_UNIT	        = 1,    // number_unit: e.g. |9<22:800W>|
	SLOT_PARSE_TYPE_TIME   	            = 2,    // time: e.g. |13|<DS:1d 0h 7m 0s>
	SLOT_PARSE_TYPE_FRACTION   	        = 3,    // fraction: e.g. |42<24:3_4F>| (si fen zhi san)	
	SLOT_PARSE_TYPE_PERCENT   	        = 4,    // percent:  e.g. |42<18:50P>| (bai fen zhi wu shi)	
	SLOT_PARSE_TYPE_NUM
}SlotParseType;

typedef struct Slot_BI
{
	int16_t B_id;                       // B-gear, B-color, B-degree ...
	int16_t I_id;                       // I-gear, I-color, I-degree ...

}Slot_BI;

typedef struct Slot
{
	int16_t id;                          // 0 ~ ? 
	int16_t unit;                        // C,G,P,M, or blank ' '. or '' for common slot
	int32_t abbr;                        // int8_t abbr[4], FL (floor), LO (location), DE (device), ... 
	int16_t type;                        // slot parse type: character, time, number_unit, fraction
	int16_t is_common;                   // Is common or private: 1 (common) or 0 (private)	
	Slot_BI bi_groups[MAX_BI_GROUP_NUM]; // max num 5, time is 3
	int16_t intent_ids[MAX_PRIVATE_SLOT_SUPPORT_INTENT_NUM];         // intent ids with private slot
	
}Slot;

typedef struct IntentPrivateData
{
	int16_t intent_num;                    // actual intent num
	int16_t slots_type_num;                // actual slots type num , (a pair or a combination of pairs of B-, I-)
	Slot slots[];                          // slots 
	
}IntentPrivateData;


typedef struct IntentResultInitPara {
    uint16_t *intent_thresholds;          // thresholds for each intent
    void *p_private_data;                 // private data of intent
} IntentResultInitPara;


typedef struct IntentResultInstance {
    int16_t intent_num;                 // intent number, gotten from privated data
    int16_t slots_type_num;             // actual slots type num , (a pair or a combination of pairs of B-, I-)
    uint16_t *intent_thresholds;
    Slot *slots; 
} IntentResultInstance;


int32_t nlp_intent_result(MidHandle handle, int16_t *word_scores, int16_t *commands, char *intent_str);
int32_t nlp_intent_result_init(IntentResultInitPara *p_init, MidHandle *mid_pp_inst);
#endif