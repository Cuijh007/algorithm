#ifndef __INTENT_SLOT_H__
#define __INTENT_SLOT_H__


typedef enum
{
    ALG_NLP_SLOT_FLOOR = 0, //floor FL          "type": "character",                    
    ALG_NLP_SLOT_LOCATION = 1, //location LO    "type": "character",
    ALG_NLP_SLOT_DEVICE = 2, //device DE        "type": "character",
    ALG_NLP_SLOT_TIME_START = 3, //time-start TS            具体时间-开始，比如“今天下午十一点半开机”中的“今天下午十一点半”,以time start表示，缩写 TS
    ALG_NLP_SLOT_DURATION_START = 4, //duration-start  DS   时间段-开始, 比如“四小时后关机”中的“四小时”,以duration start表示，缩写 DS
    ALG_NLP_SLOT_DURATION_END = 5, //duration-end    DD     时间段-结束, 比如“最大热风吹四小时”中的“四小时”，以duration end，缩写 DD
    ALG_NLP_SLOT_TIME_END = 6, //time-end     TE            具体时间-结束，比如“开机到今天下午十一点半”中的“今天下午十一点半”,以time end表示，缩写 TE
    ALG_NLP_SLOT_PART = 7, //part PA             "type": "character",
    ALG_NLP_SLOT_PROP = 8, //prop PR             "type": "character",
    ALG_NLP_SLOT_SUBSTANCE = 9, //substance SU   "type": "character",
    ALG_NLP_SLOT_BRAND = 10, //brand BR          "type": "character",
    ALG_NLP_SLOT_MOTION = 11, //motion MT        "type": "character",    <11:34_87>                   
    ALG_NLP_SLOT_MODE = 12, //mode MD            "type": "character",    <12:34_87_120 >
    ALG_NLP_SLOT_LITER = 13, //liter LT          "type": "number_unit",  <13:300L>
    ALG_NLP_SLOT_GEAR = 14, //gear GE             "type": "number_unit", <14:80G>
    ALG_NLP_SLOT_COLOR = 15, //color CL           "type": "character",   <15:34_56 >
    ALG_NLP_SLOT_DEGREE = 16, //degree DG         "type": "number_unit", <16:80C>
    ALG_NLP_SLOT_ANGLE = 17, //angle AG           "type": "number_unit", <17:80A>
    ALG_NLP_SLOT_PERCENT = 18, //percent PC       "type": "fraction",    <18:50P>
    ALG_NLP_SLOT_MILLILITER = 19, //milliliter ML "type": "number_unit", <19:300M>
    ALG_NLP_SLOT_CHANNEL = 20, //channel CH       "type": "number_unit", <20:25 >
    ALG_NLP_SLOT_ALL = 21, //all AL               "type": "character",   <21:67_89>
    ALG_NLP_SLOT_WATT = 22, //watt WT             "type": "number_unit", <22:800W>
    ALG_NLP_SLOT_STATE = 23, //state ST           "type": "character",   <23:67_89>
    ALG_NLP_SLOT_FRACTION = 24, //fraction FR     "type": "fraction",    <24:50F>
    ALG_NLP_SLOT_DIRECTION = 25, //direction DI   "type": "character",   <25:67_89>
} ALG_NLP_SLOT_TYPE;

#endif