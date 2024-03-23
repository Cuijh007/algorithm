/*****************************************************************************
*  FFVAD Controller Driver Head File for Bach2c.
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
*  @file     iet_ffvad.h
*  @brief    iet FFVAD Controller Driver Head File
*  @author   ZengHua.Gao
*  @email    zhgao@intenginetech.com
*  @version  1.0
*  @date     2020-06-15
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/06/15 | 1.0   | ZengHua.Gao      | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __IET_FFVAD_H__
#define __IET_FFVAD_H__
#include "iet_common.h"

/**
 * \defgroup    DEVICE_IET_FFVAD  IET FFVAD Controller Driver Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for FFVAD Controller Driver (\ref iet_ffvad.h)
 * \details provide interfaces for ffvad driver to implement
 * @{
 *
 * \file
 * \brief   ffvad controller definitions
 * \details provide common definitions for ffvad controller,
 *  then software developer can develop ffvad driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */
/**
 * \brief   Definitions for FFVAD modules
 * @{
 *
 */
#define IET_FFVAD_INTR_SS       (0)
#define IET_FFVAD_INTR_PREP     (1)
#define IET_FFVAD_INTR_FFT      (2)
#define IET_FFVAD_INTR_MFCC     (3)
#define IET_FFVAD_INTR_VAD      (4)
#define IET_FFVAD_INTR_NUM      (5)

#define IET_FFVAD_MANUAL_START_PREP     (4)
#define IET_FFVAD_MANUAL_START_FFT      (5)
#define IET_FFVAD_MANUAL_START_MFCC     (6)
#define IET_FFVAD_MANUAL_START_VAD      (7)
#define IET_FFVAD_MANUAL_START_NUM      (4)
#define IET_FFVAD_MANUAL_RESET_FRU      (8)
/**
 * \brief   Macro  Definitions for FFVAD driver and app
 * @{
 */

/** @} */

/**
 * \brief   Definitions for FFVAD param check.
 * @{
 */

/** @} */

/**
 * \brief   IET FFVAD config structure definition
 * @{
 */

/** @} */
/** @} */

/**
 * \defgroup    DEVICE_IET_FFVAD_CTRLCMD      FFVAD Device Control Commands
 * \ingroup DEVICE_IET_FFVAD
 * \brief   Definitions for FFVAD Driver Control Command
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - RET_FFVAD_OK,      Control device successfully
 *   - RET_FFVAD_OPNED,   Device is opened, The second time can not be opened.
 *   - RET_FFVAD_PAR,     Parameter is not valid for current control command
 *   - RET_FFVAD_IO,      Control command is not supported or not valid
 * @{
 */
/** Define FFVAD control commands for common usage */
#define DEV_SET_FFVAD_SYSCMD(cmd)     DEV_SET_SYSCMD(0x00030000|(cmd))
/** Define FFVAD control commands for master usage */
#define DEV_SET_FFVAD_MST_SYSCMD(cmd) DEV_SET_SYSCMD(0x00034000|(cmd))
/** Define FFVAD control commands for slave usage */
#define DEV_SET_FFVAD_SLV_SYSCMD(cmd) DEV_SET_SYSCMD(0x00038000|(cmd))

/* ++++ Common commands for FFVAD Device ++++ */
/**
 * Set FFVAD driver config
 * - Param type : FFVAD_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_FFVAD_CMD_S_CONFIG            DEV_SET_FFVAD_SYSCMD(0)

/**
 * Get FFVAD driver config
 * - Param type : FFVAD_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_FFVAD_CMD_G_CONFIG            DEV_SET_FFVAD_SYSCMD(1)

/**
 * Get FFVAD driver status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_FFVAD_CMD_G_STATUS            DEV_SET_FFVAD_SYSCMD(2)

/**
 * Get FFVAD driver intr status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_FFVAD_CMD_G_INTR_STATUS       DEV_SET_FFVAD_SYSCMD(3)
/** @} */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    DEVICE_IET_FFVAD_FUN FFVAD Driver Functions
 * \ingroup     DEVICE_IET_FFVAD
 * \brief       Definitions for FFVAD Driver API functions
 * @{
 */

/**
  \fn           int32_t iet_ffvad_init(uint8_t index, uint32_t addr, void* callback)
  \brief        start ffvad
  \return       RET_FFVAD_OK      init successfully
*/
extern uint32_t iet_ffvad_init(uint8_t index, uint32_t addr, void* callback);


/**
  \fn           int32_t iet_ffvad_start(uint8_t index, uint32_t addr, void* callback)
  \brief        start ffvad
  \return       RET_FFVAD_OK      start successfully
*/
extern uint32_t iet_ffvad_start(uint8_t index, uint32_t addr, void* callback);

/**
  \fn           int32_t iet_ffvad_stop(uint8_t index)
  \brief        stop ffvad
  \return       RET_FFVAD_OK      stop successfully
*/
extern uint32_t iet_ffvad_stop(uint8_t index);
/** @} */
#ifdef __cplusplus
}
#endif
/** @} */
/** @} */

#endif /* __IET_FFVAD_H__ */

/* --------------------------------- End Of File ------------------------------ */
