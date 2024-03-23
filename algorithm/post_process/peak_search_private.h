/*****************************************************************************
*  
*  Generated automatically for beam search, please don't touch it 
*  Common header file for cmd_tree.bin 
*  
*****************************************************************************/
#ifndef _PEAK_SEARCH_PRIVATE_H_
#define _PEAK_SEARCH_PRIVATE_H_


#include <stdint.h>


typedef enum PeakSearchMode {
    PEAK_MODE_MAX_SCORE,
    PEAK_MODE_LAST_CMD,
} PeakSearchMode;

typedef struct PeakSearchInitPara {
    uint16_t word_num;                  // real cmd number of this model
    uint16_t score_num;                 // data number for the cmds in the input word_scores
    uint16_t out_size;                  // total data number of the input word_scores
    uint16_t *word_thresholds;          // thresholds for each word/phrase/command
    void *p_private_data;               // private data of this post process
} PeakSearchInitPara;

typedef struct PeakSearchInstance {
    uint16_t word_num;
    uint16_t score_num;
    uint16_t out_size;
    uint16_t extra_data_size;           // extra data(vad or am) size
    uint16_t with_extra_data;           // extra data is provided also, used to check the end of the cmd
    uint16_t eos_frame_count;           // check eos as per extra data, take the frame as EOS if the max score of extra data is from sil
    uint16_t *word_thresholds;
    uint16_t *p_command_delay;          // extra delay in frame for each command
    uint16_t active_cmd_id;
    uint16_t active_cmd_score;
    uint16_t delay_frames;              // frame count of delaying
    uint16_t extra_label;
    uint16_t response_delay;             // reponse delay in frame
} PeakSearchInstance;

// the struct must be allocated to the beginning of private data areas, such as PeakSearchInitPara:p_private_data
// Make sure the size is aligned with 4 if change this struture.
typedef struct PostProcessDataHeader {
    uint16_t command_delay_offset;  // offset of the command delay data, refer to PeakSearchInitPara:p_private_data, that's the start addr of PostProcessDataHeader
    uint16_t command_delay_num;     // number of command with delay data, include <sil>
    uint16_t extra_label;           // type of exra label, reference to
    uint16_t reserved_0;
    uint16_t reserved_1;
    uint16_t reserved_2;
    uint16_t reserved_3;
    uint16_t reserved_4;
    uint16_t reserved_5;
    uint16_t reserved_6;
    uint16_t reserved_7;        
    uint16_t reserved_8;
} PostProcessDataHeader;

#endif // _PEAK_SEARCH_PRIVATE_H_