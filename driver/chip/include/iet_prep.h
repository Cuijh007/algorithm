/*****************************************************************************
*  PREP Controller Driver Head File for Bach2c.
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
*  @file     iet_prep.h
*  @brief    iet PREP Controller Driver Head File
*  @author   ZengHua.Gao
*  @email    zhgao@intenginetech.com
*  @version  1.0
*  @date     2020-03-27
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/03/27 |    1.0    |   ZengHua.Gao  | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __IET_PREP_H__
#define __IET_PREP_H__

#include "iet_common.h"

/**
 * \defgroup    DEVICE_IET_PREP  IET PREP Controller Driver Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for PREP Controller Driver (\ref iet_prep.h)
 * \details provide interfaces for prep driver to implement
 * @{
 *
 * \file
 * \brief   prep controller definitions
 * \details provide common definitions for prep controller,
 *  then software developer can develop prep driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */
/**
 * \brief   Definitions for PREP modules.
 * @{
 */
/**
 * \brief   Macro Definitions for PREP driver and app
 * @{
 */
#define IET_PREP_FILTER_K           (7946)  /*!< K of pre-process */

#define IET_PREP_MODE               (0)     /*!< bit 0: Pre-process module working mode [0: manual; 1: auto] */
#define IET_PREP_MODE_MANUA         (0)     /*!< Pre-process module working mode is manual */
#define IET_PREP_MODE_AUTO          (1)     /*!< Pre-process module working mode is auto */
#define IET_PREP_MODE_MASK          (1 << IET_PREP_MODE) /*!< width:1 */

#define IET_PREP_START              (4)     /*!< bit 4: Writing 1 to start Pre-process module */
#define IET_PREP_START_EN           (1)     /*!< Writing 1 to start Pre-process module */
#define IET_PREP_START_MASK         (1 << IET_PREP_START) /*!< width:1 */

#define IET_PREP_STATUS             (1)     /*!< bit 1: pre-process module status [0:busy; 1:idle] */
#define IET_PREP_STATUS_MASK        (1 << IET_PREP_STATUS) /*!< width:1 */
#define IET_PREP_INTR_STATUS        (1)     /*!< bit 1: pre-process done interrupt status */
#define IET_PREP_INTR_STATUS_MASK   (1 << IET_PREP_STATUS) /*!< width:1 */
/** @} */
/**
 * \brief   Definitions for PREP param check
 * @{
 */
#define IS_IET_PREP_FILTER_K(VAL)   (VAL == IET_PREP_FILTER_K)
#define IS_IET_PREP_MODE(VAL)       ((VAL == IET_PREP_MODE_AUTO) || (VAL == IET_PREP_MODE_MANUA))
#define IS_IET_PREP_ADDR(VAL)       IS_VALID_VAL(VAL)
/** @} */

/**
 * \brief   IET PREP config structure definition
 * @{
 */
typedef struct
{
    uint32_t mode;          /*!< prep operation mode [IET_PREP_MODE_AUTO/IET_PREP_MODE_MANUA] */
    uint32_t *in;           /*!< prep inputbuffer address [512*16bit] */
    uint32_t *out;          /*!< prep outputbuffer address [512*32bit] */
    uint32_t filter_k;      /*!< Sample pre-process pre-emphasis coefficient */
    void*    callback;      /*!< prep callback fun */
} IET_PREP_Config_TypeDef;
/** @} */
/** @} */

/**
 * \defgroup    DEVICE_IET_PREP_CTRLCMD      PREP Drive Control Commands
 * \ingroup DEVICE_IET_PREP
 * \brief   Definitions for PREP Driver Control Command
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/uint32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - RET_PREP_OK,      Control device successfully
 *   - RET_PREP_OPNED,   Device is opened, The second time can not be opened.
 *   - RET_PREP_PAR,     Parameter is not valid for current control command
 *   - RET_PREP_IO,      Control command is not supported or not valid
 * @{
 */
/** Define PREP control commands for common usage */
#define DEV_SET_PREP_SYSCMD(cmd)     DEV_SET_SYSCMD(0x00030000|(cmd))
/** Define PREP control commands for master usage */
#define DEV_SET_PREP_MST_SYSCMD(cmd) DEV_SET_SYSCMD(0x00034000|(cmd))
/** Define PREP control commands for slave usage */
#define DEV_SET_PREP_SLV_SYSCMD(cmd) DEV_SET_SYSCMD(0x00038000|(cmd))

/* ++++ Common commands for PREP driver ++++ */
/**
 * Set PREP driver config
 * - Param type : IET_PREP_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_PREP_CMD_S_CONFIG            DEV_SET_PREP_SYSCMD(0)

/**
 * Get PREP driver config
 * - Param type : IET_PREP_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_PREP_CMD_G_CONFIG            DEV_SET_PREP_SYSCMD(1)

/**
 * Get PREP driver status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_PREP_CMD_G_STATUS            DEV_SET_PREP_SYSCMD(2)

/**
 * Get PREP driver intr status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_PREP_CMD_G_INTR_STATUS       DEV_SET_PREP_SYSCMD(3)

/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    DEVICE_IET_PREP_FUN PREP Driver Functions
 * \ingroup     DEVICE_IET_PREP
 * \brief       Definitions for PREP Driver API functions
 * @{
 */
/**
  \fn           uint32_t iet_prep_open(IET_PREP_Config_TypeDef *prep_config)
  \brief        open iet prep
  \param[in]    prep_config      prep config struct IET_PREP_Config_TypeDef
  \return       RET_PREP_OK      open successfully without any issues
  \return       RET_PREP_OPNED   if device was opened before with different parameters, return RET_PREP_OPNED
  \return       RET_PREP_PAR     parameter is not valid
 */
extern uint32_t iet_prep_open(IET_PREP_Config_TypeDef *prep_config);

/**
  \fn           int32_t iet_prep_close(void)
  \brief        close iet prep
  \return       RET_PREP_OK      close successfully
*/
extern uint32_t iet_prep_close(void);

/**
  \fn           iet_prep_control(uint32_t ctrl_cmd, void *param)
  \brief        control prep by control command [optional]
  \param[in]    ctrl_cmd        ref DEVICE_IET_PREP_CTRLCMD "control command", to change or get some thing related to prep
  \param[in,out]
                param           parameters that maybe argument of the command, or return values of the command, must not be null
  \return       RET_PREP_OK     control device successfully
  \return       RET_PREP_PAR    parameter is not valid for current control command
  \return       RET_PREP_IO     control command is not supported or not valid
*/
extern uint32_t iet_prep_control(uint32_t ctrl_cmd, void *param);

/**
  \fn           int32_t iet_prep_start(void)
  \brief        start prep
  \return       RET_PREP_OK     start successfully
*/
extern uint32_t iet_prep_start(void);
/** @} */
#ifdef __cplusplus
}
#endif
/** @} */
/** @} */

#endif /* __IET_PREP_H__ */

/* --------------------------------- End Of File ------------------------------ */
