#ifndef __USERDEFINE_H__
#define __USERDEFINE_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include "nn_file_struct.h"
//#include "nn_common.h"
//#include "mid_common.h"

#define MAX_WAKEUP_S2_OUT_SIZE      (16)    // the first one for silent
#define MAX_AM_WORD_NUM             (32)      // UPDATE this as per real AM out size
#define MAX_BUFFER_FRAME_LEN		(1)       // UPDATE this to the real value of AM out buffer, such as 40 if run s1
#define MAX_OUT_CMD_NUM				(2)

/*
 * work mode of stage 1 for wakeup userdefine 
 */
typedef enum WakeupSearchMode
{
	S1_MODE_MAX_SCORE = 0,
	S1_MODE_LAST_CMD,
	S1_MODE_BEAM,
	S1_MODE_NUM
}WakeupSearchMode;


/*
 * model related setting
 * npu convert gets the info from the float model, and write to wakeup layer of ISD
 * ISD2BIN write it to the 'userDefine' area of this layer
 */
typedef struct WakeupModelSetting
{
	int16_t s1_startId;         // start layer id of stage 1
	int16_t s1_endId;           // end layer id of stage 1
	int16_t s2_startId;         // start layer id of stage 2
	int16_t s2_endId;           // end layer id of stage 2
	int16_t times_len;          // deprecated
	int16_t run_times;          // run times of stage 2, 0: no run
	int16_t reverse;			// direction of process input data for stage 2 when GRU is used, 0: fowward, 1: reverse
	int16_t reserved;
}WakeupModelSetting_t;


/*
 * threshold and other setting gotten from userdefine.json
 * set by packweight.exe as private data of the layer
 */
typedef struct WakeupExtraSetting
{
	uint8_t with_prefix;         // 1: with prefix word, 0: w/o
	uint8_t wakeup_number;       // wakup word number
	uint8_t word_number;         // command number, not include <sil>
	uint8_t on_cmd_number;       // 0: no "on-off" command pair for stage 2 to process, other value: 'on' command number that stage 2 can recognize
	uint8_t extra_label;         // type of exra label, reference to 
	int16_t s1_mode;    		// work mode of stage 1, reference to WakeupSearchMode for details
}WakeupExtraSetting_t;


extern int nn_userdefine_wakeup_init(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase);
extern int nn_userdefine_s2_confirm_set(int model_index, int s2_reconfirm_frames);
int nn_userdefine_wakeup(const Net_t *pNet, int model_index, const LayerParam_t *param, void *pBase);

#ifdef __cplusplus
}
#endif
#endif 
