/*****************************************************************************
*  VAD Controller Driver Head File for Bach2c.
*
*  Copyright (C) 2018-2020 IET Limited. All rights reserved.
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
*  @file     iet_vad.h
*  @brief    iet VAD Controller Driver Head File
*  @author   ZengHua.Gao
*  @email    zhgao@intenginetech.com
*  @version  1.0
*  @date     2020-03-30
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/03/30 | 1.0   | ZengHua.Gao      | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __IET_VAD_H__
#define __IET_VAD_H__
#include "iet_common.h"

/**
 * \defgroup    DEVICE_IET_VAD  IET VAD Controller Driver Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for VAD Controller Driver (\ref iet_vad.h)
 * \details provide interfaces for vad driver to implement
 * @{
 *
 * \file
 * \brief   vad controller definitions
 * \details provide common definitions for vad controller,
 *  then software developer can develop vad driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */
/**
 * \brief   Definitions for VAD modules.
 * @{
 */
/**
 * \brief   Macro  Definitions for VAD driver and app.
 * @{
 */
#define IET_VAD_FROM                    (20)    /*!< bit 20: Control reading fbank data from system memory to VAD module [0:from MFCC output; 1:from memory] */
#define IET_VAD_FROM_MFCC               (0)     /*!< VAD data source from MFCC output */
#define IET_VAD_FROM_MEM                (1)     /*!< VAD data source from memory */
#define IET_VAD_FROM_MASK               (1 << IET_VAD_FROM) /*!< width:1 */

#define IET_VAD_NUM_CHANS               (21)    /*!< bit 21: number of fbank features when manual start VAD module */
#define IET_VAD_NUM_CHANS_MASK          (0x7f << IET_VAD_NUM_CHANS) /*!< width:7 */
#define IET_VAD_NUM_CHANS_VAL(x)        ((x & 0x7f) << IET_VAD_NUM_CHANS) /*!< width:7 */

#define IET_VAD_MODE                    (3)     /*!< bit 3: VAD module working mode [0: manual; 1: auto] */
#define IET_VAD_MODE_MANUA              (0)     /*!< VAD module working mode is manual */
#define IET_VAD_MODE_AUTO               (1)     /*!< VAD module working mode is auto */
#define IET_VAD_MODE_MASK               (1 << IET_VAD_MODE) /*!< width:1 */

#define IET_VAD_START                   (7)     /*!< bit 7: Writing 1 to start VAD module */
#define IET_VAD_START_EN                (1)     /*!< Writing 1 to start VAD module */
#define IET_VAD_START_MASK              (1 << IET_VAD_START) /*!< width:1 */

#define IET_VAD_RESET_GRU               (8)     /*!< bit 8: Writing 1 to Reset GRU status buffer */
#define IET_VAD_RESET_GRU_EN            (1)     /*!< Writing 1 to Reset GRU status buffer */
#define IET_VAD_RESET_GRU_MASK          (1 << IET_VAD_RESET_GRU) /*!< width:1 */

#define IET_VAD_STATUS                  (4)     /*!< bit 4: VAD module status [0:busy; 1:idle] */
#define IET_VAD_STATUS_MASK             (1 << IET_VAD_STATUS)
#define IET_VAD_INTR_STATUS             (4)     /*!< bit 3: one frame VAD calculation done interrupt status */
#define IET_VAD_INTR_STATUS_MASK        (1 << IET_VAD_INTR_STATUS)
/** @} */

/**
 * \brief   Definitions for VAD param check.
 * @{
 */
#define IS_IET_VAD_MODE(VAL)            ((VAL == IET_VAD_MODE_AUTO) || (VAL == IET_VAD_MODE_MANUA))
#define IS_IET_VAD_INTYPE(TYPE)         ((TYPE == IET_VAD_FROM_MFCC) || (TYPE == IET_VAD_FROM_MEM))
#define IS_IET_VAD_ADDR(VAL)            IS_VALID_VAL(VAL)
#define IS_IET_VAD_NUM_CHANS(VAL)       IS_LENGTH_VAL(VAL)

/** @} */

/**
 * \brief   IET VAD config structure definition
 * @{
 */
typedef struct
{
    uint32_t mode;              /*!< vad operation mode [IET_VAD_MODE_AUTO/IET_VAD_MODE_MANUA] */
    uint32_t intype;            /*!< vad inputbuffer type [IET_VAD_FROM_MFCC/IET_VAD_FROM_MEM] */
    uint32_t chans;             /*!< vad number of fbank features for inputbuffer [IET_VAD_NUM_CHANS_VAL] */
    uint32_t *in;               /*!< vad inputbuffer address */
    uint32_t *out;              /*!< vad outputbuffer address */
    void     *callback;         /*!< vad callback */
} IET_VAD_Config_TypeDef;
/** @} */
/** @} */

/**
 * \defgroup    DEVICE_IET_VAD_CTRLCMD      VAD Device Control Commands
 * \ingroup DEVICE_IET_VAD
 * \brief   Definitions for VAD Driver Control Command
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - RET_VAD_OK,      Control device successfully
 *   - RET_VAD_OPNED,   Device is opened, The second time can not be opened.
 *   - RET_VAD_PAR,     Parameter is not valid for current control command
 *   - RET_VAD_IO,      Control command is not supported or not valid
 * @{
 */
/** Define VAD control commands for common usage */
#define DEV_SET_VAD_SYSCMD(cmd)     DEV_SET_SYSCMD(0x00030000|(cmd))
/** Define VAD control commands for master usage */
#define DEV_SET_VAD_MST_SYSCMD(cmd) DEV_SET_SYSCMD(0x00034000|(cmd))
/** Define VAD control commands for slave usage */
#define DEV_SET_VAD_SLV_SYSCMD(cmd) DEV_SET_SYSCMD(0x00038000|(cmd))

/* ++++ Common commands for VAD Device ++++ */
/**
 * Set VAD driver config
 * - Param type : VAD_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_VAD_CMD_S_CONFIG            DEV_SET_VAD_SYSCMD(0)

/**
 * Get VAD driver config
 * - Param type : VAD_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_VAD_CMD_G_CONFIG            DEV_SET_VAD_SYSCMD(1)

/**
 * Get VAD driver status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_VAD_CMD_G_STATUS            DEV_SET_VAD_SYSCMD(2)

/**
 * Get VAD driver intr status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_VAD_CMD_G_INTR_STATUS       DEV_SET_VAD_SYSCMD(3)
/** @} */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    DEVICE_IET_VAD_FUN VAD Driver Functions
 * \ingroup     DEVICE_IET_VAD
 * \brief       Definitions for VAD Driver API functions
 * @{
 */
/**
  \fn           uint32_t iet_vad_open(IET_VAD_Config_TypeDef *vad_config)
  \brief        open iet vad
  \param[in]    vad_config      vad config struct IET_Fft_Config_TypeDef
  \return       RET_VAD_OK      open successfully without any issues
  \return       RET_VAD_OPNED   if device was opened before with different parameters, return RET_VAD_OPNED
  \return       RET_VAD_PAR     parameter is not valid
 */
extern uint32_t iet_vad_open(IET_VAD_Config_TypeDef *vad_config);

/**
  \fn           int32_t iet_vad_close(void)
  \brief        close iet vad
  \return       RET_VAD_OK      close successfully
*/
extern uint32_t iet_vad_close(void);

/**
  \fn           iet_vad_control(uint32_t ctrl_cmd, void *param)
  \brief        control vad by control command
  \param[in]    ctrl_cmd        ref DEVICE_IET_VAD_CTRLCMD "control command", to change or get some thing related to vad
  \param[in,out]
                param           parameters that maybe argument of the command, or return values of the command, must not be null
  \return       RET_VAD_OK      control device successfully
  \return       RET_VAD_PAR     parameter is not valid for current control command
  \return       RET_VAD_IO      control command is not supported or not valid
*/
extern uint32_t iet_vad_control(uint32_t ctrl_cmd, void *param);

/**
  \fn           int32_t iet_vad_start(void)
  \brief        start vad
  \return       RET_VAD_OK      start successfully
*/
extern uint32_t iet_vad_start(void);

/**
  \fn           int32_t iet_vad_stop(void)
  \brief        stop vad
  \return       RET_VAD_OK      stop successfully
*/
extern uint32_t iet_vad_stop(void);
/** @} */
#ifdef __cplusplus
}
#endif
/** @} */
/** @} */

#endif /* __IET_VAD_H__ */

/* --------------------------------- End Of File ------------------------------ */
