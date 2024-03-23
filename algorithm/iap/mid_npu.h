/******************************************************************************
*  MID NPU Control Module Head File for Chopin.
*
*  Copyright (C) 2019-2020 IET Limited. All rights reserved.
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License version 3 as
*  published by the Free Software Foundation.
*
*  You should have received a copy of the GNU General Public License
*  along with OST. If not, see <http://www.gnu.org/licenses/>.
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*  @file     mid_npu.h
*  @brief    NPU control module Head File
*  @author   Zhiyun.Ling
*  @email    Zhiyun.Ling@intenginetech.com
*  @version  1.0
*  @date     2019-12-17
*  @license  GNU General Public License (GPL)
*
*------------------------------------------------------------------------------
*  Remark         : Description
*------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*------------------------------------------------------------------------------
*  2019/12/17 | 1.0       | Zhiyun.Ling    | Create file
*  2020/02/26 | 1.1       | Hui.Zhang      | Filled function
*
******************************************************************************/

#ifndef __NPU_CONTROL_H__
#define __NPU_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************* Include *************************************/
#include "mid_common.h"


/**
 * \defgroup    MID_NPU_CONTROL NPU Control Interface
 * \ingroup IET_MIDDLEWARE
 * \brief   definitions for NPU control (\ref mid_npu.h)
 * \details provide interfaces for NPU control to implement
 * @{
 *
 * \file
 * \brief   npu control definitions
 * \details provide common definitions for npu control module,
 *  then software developer can develop npu control
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */

/**
 * \defgroup    MID_NPU_CONTROL_DEFINE  NPU control definitions
 * \ingroup MID_NPU_CONTROL
 * \brief   contains definitions of NPU control.
 * \details this config will be used in user implemented code.
 * @{
 */

/**
 * \brief   NPU Control weight verification enum
 * \details define MID_NPU_VERIFY_MODE
 * @{
 */
typedef enum
{
    MID_NPU_VERIFY_NONE,
    MID_NPU_VERIFY_MD5,
    MID_NPU_VERIFY_CRC,
    MID_NPU_VERIFY_INVALID
} MID_NPU_VERIFY_MODE;
/** @} */

/**
 * \brief   NPU Control Running mode enum
 * \details define MID_NPU_RUN_MODE
 * @{
 */
typedef enum
{
    MID_NPU_RUN_FRAME,
    MID_NPU_RUN_LAYER,
    MID_NPU_RUN_INVALID
} MID_NPU_RUN_MODE;
/** @} */


/**
 * \brief   net type enum
 * \details define NetType
 * @{
 */
typedef enum
{
 	QUICK_MODEL = 1,  // 快速模型
    COMMAND_MODEL_BIG = 2,  // 命令词模型 (大)
    KWS_MODEL_BIG = 3,  // 唤醒词模型 (大) 500K
    KWS_MODEL_BIG_TEST = 4,  //唤醒词模型 (大-TEST) 500K 新
    FREE_KWS_MODEL_BIG = 5,  // 免唤醒模型 (大)
    FREE_KWS_MODEL_MEDIUM = 6,  // 免唤醒模型 (中) 300K
    FREE_KWS_MODEL_SMALL = 7,  //免唤醒模型 (小) 220K
    FREE_KWS_MODEL_LOW_POWER = 8,  //免唤醒模型 (小) 低功耗
    COMMAND_MODEL_SMALL = 9,  //命令词模型 (小)
    KWS_MODEL_SMALL = 10,  // 唤醒词模型 (小)
    NLP_MODEL = 11,  // NLP 模型单片
    MINI_NLP_MODEL = 12,  // M-NLP 模型(MINI-NLP)
    NLP_LITE_MODEL = 13,  // NLP-L 模型(NLPLite)
    FST_MODEL = 14,  // 极速模型(解码图)
    NLP_2CHIP_MODEL = 15,  // NLP 模型双片
    SV_MODEL = 16, // 声纹模型
}NetType;
/** @} */

/**
 * \brief   MID NPU callback function
 * \details define mid_npu_callback
 * \param[in] model_index  model index, or other value as per the caller
 * @{
 */
typedef void (*mid_npu_callback)(int model_index);
/** @} */

/**
 * \brief   MID NPU layer callback function
 * \details define mid_npu_callback
 * \param[in] layer_index  layer index
 * \param[in] p_data  pointer to the out data of this layer
 * @{
 */
typedef void (*mid_npu_layer_callback)(int layer_index, int16_t *p_data);
/** @} */

/**
 * \brief   MID NPU config structure definition
 * \details define MID_NPU_OPEN_CONFIG
 */
typedef struct
{
    MID_NPU_VERIFY_MODE verify_mode;   /*!< Weight verification mode         */
    MID_NPU_RUN_MODE    run_mode;      /*!< NPU running mode                 */
    mid_npu_callback    npu_callback;  /*!< NPU done callback as run mode    */
#ifdef __x86_64
    mid_npu_layer_callback   npu_layer_callback; /*!< NPU layer done callback as run mode    */
#endif
} MID_NPU_OPEN_CONFIG;
/** @} */
/** @} */


/**
 * \brief   MID NPU config structure definition
 * \details define MID_NPU_OPEN_CONFIG
 */
typedef struct
{
    int16_t model_index;                /*!< Model index in the weight file  */
    int32_t input_size;                 /*!< Input size of the model in 2Bytes */
    int32_t result_size;                /*!< Output size of the model in 2Bytes */
    int16_t frame_length;               /*!< frame length in ms              */
    int16_t frame_offset;               /*!< Frame offset in ms              */
    int16_t mic_number;                 /*!< Mics used                       */
    char * model_info;                  /*!< Pointer to the mode info string */
} MID_NPU_MODEL_INFO;
/** @} */
/** @} */


/**
 * \brief   MID NPU init param structure definition
 * \details define MID_NPU_OPEN_CONFIG
 */
typedef struct
{
    void *p_net_info;                /*!< Pointer to the net info structure  */
    void *p_weight;                  /*!< Pointer to the weight */
    void *p_vpu_code;                /*!< Pointer to the vpu code  */
    void *p_mic_weight_matrix;       /*!< Pointer to the mic weight matrix */
    void *p_private_data;            /*!< Pointer to the private data of this net */   
} MID_NPU_INIT_PARAMS;
/** @} */
/** @} */


/**
 * \defgroup    MID_NPU_CONTROL_CMD     NPU Control Commands
 * \ingroup MID_NPU_CONTROL
 * \brief   Definitions for NPU Control Command
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - MID_NPU_OK,        Control device successfully
 *   - MID_NPU_CLSED,     Device is not opened
 *   - MID_NPU_OPEND,     Device is opened, The second time can not be opened.
 *   - MID_NPU_PAR,       Parameter is not valid for current control command
 * @{
 */

/**
 * Get NPU control module version
 * - Param type : char *
 * - Param usage : store result of NPU control version
 * - Return value explanation :
 */
#define MID_NPU_CMD_BIN_VERSION         (0)

/**
 * Clear the parameter of NPU control module
 * - Param type : NULL
 * - Param usage: NULL
 * - Return value explanation :
 */
#define MID_NPU_CMD_CLEARTIMES          (1)

/**
 * Clear GRU state info
 * - Param type : NULL
 * - Param usage: set gru state
 * - Return value explanation :
 */
#define MID_NPU_CMD_CLEAR_GRU_STATE     (2)

/**
 * Get NPU LIBRARY VERSION
 * - Param type : char *
 * - Param usage: get lib version
 * - Return value explanation :
 */
#define MID_NPU_CMD_LIB_VERSION         (3)

/**
 * Get NPU LIBRARY COMPILE DATE
 * - Param type : char *
 * - Param usage: get lib compile date
 * - Return value explanation :
 */
#define MID_NPU_CMD_LIB_COMPILEDATE     (4)


/**
 * Get NPU LIBRARY COMPILE DATE
 * - Param type : char *
 * - Param usage: get model info
 * - Return value explanation :
 */
#define MID_NPU_CMD_MODEL_INFO          (5)


/**
 * Clear all layer with save flag
 * - Param type : NULL
 * - Param usage: NULL
 * - Return value explanation :
 */
#define MID_NPU_CMD_CLEAR_SAVE          (6)


/**
 * Init layers priviate data
 * - Param type : NULL
 * - Param usage: NULL
 * - Return value explanation :
 */
#define MID_NPU_CMD_INIT_PRIVATE_DATA   (7)


/**
 * Set layer debug out info
 * - Param type : NULL
 * - Param usage: NULL
 * - Return value explanation :
 */
#define MID_NPU_CMD_LAYER_DEBUG_SETTING   (8)


/**
 * Set layer debug out info
 * - Param type : NULL
 * - Param usage: NULL
 * - Return value explanation :
 */
#define MID_NPU_CMD_LAYER_DEBUG_ENABLE   (9)


/**
 * Set userdefinewakeup layer confirm Valid for argc frames after s1 wake up
 * - Param type : int
 * - Param usage: default : 80
 * - Return value explanation :
 */
#define MID_NPU_CMD_USERDEFINE_CONFIRM      (10)

#define MID_NPU_CMD_SET_KWS_FLASHBASE       (15)
#define MID_NPU_CMD_SET_MAX_INTENT_NUM      (16)
#define MID_NPU_CMD_SET_INTENT_THRESHOLD    (17)
#define MID_NPU_CMD_NLP_POST_CLEAR          (18)
#define MID_NPU_CMD_NLP_SET_MAX_KEPT_TIME   (19)

/**
 * Get model index net type
 * - Param type : int *
 * - Param usage: get net type version
 * - Return value explanation :
 */
#define MID_NPU_CMD_GET_NET_TYPE            (20)

/**
 * Set wether to keep the last group of words from asr
 */
#define MID_NPU_CMD_SET_KEPT_WORD           (21)

/**
 * call to print net info
 */
#define MID_NPU_CMD_LAYER_INFO_PRINT        (100)

/**
 * Set sound pressure of the frame to process by npu. Set with kws mode only
 * - Param type : uint32_t
 * - Param usage: set the value gotten by isr_get_spl to param here
 * - Return value explanation :
 */
#define MID_NPU_CMD_SET_SPL                 (200)


/** @} */


/**
 * Max models supported, used by app only, to reduce memory size, set the real model number here
 */
#define MID_NPU_MAX_MODELS                  (3)

/**
 * \defgroup    MID_NPU_CONTROL_FUNCTION NPU control functions
 * \ingroup     MID_NPU_CONTROL
 * \brief       API functions for NPU control module
 * @{
 */

/**
  \fn           uint32_t mid_npu_init(uint32_t weight_addr, int16_t *input_size, int16_t *result_size)
  \brief        Initialize NPU control module
  \param[in]    weight_addr     weight flash addr
  \param[out]   model_info      Info of the models
  \param[out]   vmodel_number   Model number in this weight file
  \return       MID_NPU_OK      NPU initialized successfully
*/
extern uint32_t mid_npu_init(MID_NPU_INIT_PARAMS *init_param, MID_NPU_MODEL_INFO *model_info, int32_t *model_number);

/**
  \fn           uint32_t mid_npu_open(MID_NPU_OPEN_CONFIG *npu_config)
  \brief        Open NPU control module
  \param[in]    MID_NPU_OPEN_CONFIG     the pointer for NPU configuration
  \return       MID_NPU_OK          NPU is opened successfully
  \return       MID_NPU_WEIGHT      Weight verification failed
  \return       MID_NPU_PAR         Invalid parameter
*/
extern uint32_t mid_npu_open(MID_NPU_OPEN_CONFIG *npu_config);

/**
  \fn           uint32_t mid_npu_close(void)
  \brief        Close NPU control module
  \param[in]    NULL
  \return       MID_NPU_OK
*/
extern uint32_t mid_npu_close(void);

/**
  \fn           uint32_t mid_npu_start(const int16_t *input_buffer, int16_t input_size)
  \brief        Start NPU control module
  \param[in]    input_feature   buffer address of input feature
  \param[out]   out_result      buffer address of output result
  \return       MID_NPU_OK
*/
extern uint32_t mid_npu_start(int32_t model_index, int16_t *input_buffer, int16_t *out_result);

/**
  \fn           uint32_t mid_npu_stop(void)
  \brief        Stop NPU control module
  \param[in]    NULL
  \return       MID_NPU_OK
*/
extern uint32_t mid_npu_stop(const int32_t model_index);

/**
  \fn            uint32_t mid_npu_control(uint32_t ctrl_cmd, void *param)
  \brief         Control NPU control module as command
  \param[in]     ctrl_cmd   MID_NPU_CONTROL_CMD "control command", to change or get some thing related to NPU
  \param[in,out] param      parameters that maybe argument of the command, or return values of the command, can set NULL
  \return        MID_NPU_OK
*/
extern uint32_t mid_npu_control(const int32_t model_index, uint32_t ctrl_cmd, void *param);
/** @} */

#ifdef __cplusplus
}
#endif
/** @} */

#endif /* __NPU_CONTROL_H__ */
