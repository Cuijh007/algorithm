/*****************************************************************************
*  
*  Common header file for fst decoder wrapper 
*  
*****************************************************************************/
#ifndef _FST_PRIVATE_H_
#define _FST_PRIVATE_H_

#include <stdint.h>

typedef struct FstInitPara {
    uint16_t word_num;                  // word number of this model
    uint16_t pdf_num;                   // fst pdf number of this model
    uint16_t data_offset;               // start offset of the valid data from the input data ('word_scores'), in data number
    uint16_t *word_thresholds;          // thresholds for each word/phrase/command
    uint8_t *p_private_data;            // private data of this post process
} FstInitPara;


typedef struct FstInstance {
    uint16_t word_num;                  // valid cmd number of this model, includes <sil>
    uint16_t pdf_num;                   // fst pdf number of this model
    uint16_t data_offset;               // start offset of the valid data from the input data ('word_scores'), in data number
    uint16_t *word_thresholds;          // thresholds for each word/phrase/command
} FstInstance;

#endif // _FST_PRIVATE_H_