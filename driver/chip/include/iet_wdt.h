/*****************************************************************************
*  WDT Controller Driver Head File for CHOPIN.
*
*  Copyright (C) 2018-2019 IET Limited. All rights reserved.
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
*  @file     iet_wdt.h
*  @brief    iet wdt Controller Driver Head File
*  @author   Qiang.Li
*  @email    qiangli@intenginetech.com
*  @version  1.0
*  @date     2018-10-22
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2018/10/22 | 1.0       | Qiang.Li       | Create file
*  2019/02/11 | 1.1       | Qiang.Li       | New driver framework
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __IET_WDT_H__
#define __IET_WDT_H__

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"
/**
 * \defgroup    DEVICE_IET_WDT  IET WDT Driver Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for wdt controller (\ref iet_wdt.h)
 * \details provide interfaces for wdt driver to implement
 * @{
 *
 * \file
 * \brief   wdt device definitions
 * \details provide common definitions for wdt device,
 *  then software developer can develop wdt driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */

/**
 * \defgroup    DEVICE_IET_WDT_DEVCONFIG    IET WDT Driver Config
 * \ingroup DEVICE_IET_WDT
 * \brief   contains definitions of wdt driver config.
 * \details this config will be used in user implemented code, which was called
 *     Device Driver Implement Layer for wdt to realize in user code.
 * @{
 */
#define  INTERRUPT   1
#define  RESET       0
/**
 * \brief   IET WDT timerout enum definition
 * \details define WDT_Timerout_TypeDef
 * @{
 */
typedef enum
{
    WDT_TIMEOUT_64K_CLK,    //WDT_TIMEOUT_488US,
    WDT_TIMEOUT_128K_CLK,   //WDT_TIMEOUT_977US,
    WDT_TIMEOUT_256K_CLK,   //WDT_TIMEOUT_1954US,
    WDT_TIMEOUT_512K_CLK,   //WDT_TIMEOUT_3908US,
    WDT_TIMEOUT_1M_CLK,     //WDT_TIMEOUT_7MS,
    WDT_TIMEOUT_2M_CLK,     //WDT_TIMEOUT_15MS,
    WDT_TIMEOUT_4M_CLK,     //WDT_TIMEOUT_30MS,
    WDT_TIMEOUT_8M_CLK,     //WDT_TIMEOUT_61MS,
    WDT_TIMEOUT_16M_CLK,    //WDT_TIMEOUT_122MS,
    WDT_TIMEOUT_32M_CLK,    //WDT_TIMEOUT_244MS,
    WDT_TIMEOUT_64M_CLK,    //WDT_TIMEOUT_488MS,
    WDT_TIMEOUT_128M_CLK,   //WDT_TIMEOUT_977MS,
    WDT_TIMEOUT_256M_CLK,   //WDT_TIMEOUT_1954MS,
    WDT_TIMEOUT_512M_CLK,   //WDT_TIMEOUT_3908MS,
    WDT_TIMEOUT_1G_CLK,     //WDT_TIMEOUT_7S,
    WDT_TIMEOUT_2G_CLK,     //WDT_TIMEOUT_15S,
} WDT_Timerout_TypeDef;
/** @} */
#define IS_WDT_TIMEROUT(timerout) (((timerout) >= WDT_TIMEOUT_64K_CLK)  && ((timerout) <= WDT_TIMEOUT_2G_CLK))

/**
 * \brief   IET WDT reset pulse len enum definition
 * \details define WDT_ResetPulseLen_TypeDef
 * @{
 */
typedef enum
{
    WDT_RPL_PCLK_2_CLK,
    WDT_RPL_PCLK_4_CLK,
    WDT_RPL_PCLK_8_CLK,
    WDT_RPL_PCLK_16_CLK,
    WDT_RPL_PCLK_32_CLK,
    WDT_RPL_PCLK_64_CLK,
    WDT_RPL_PCLK_128_CLK,
    WDT_RPL_PCLK_256_CLK,
} WDT_ResetPulseLen_TypeDef;
/** @} */
#define IS_WDT_RPL(len) (((len) >= WDT_RPL_PCLK_2_CLK)  && ((len) <= WDT_RPL_PCLK_256_CLK))
/** @} */

/**
 * \defgroup    DEVICE_IET_WDT_CTRLCMD      IET WDT Driver Io-Control Commands
 * \ingroup DEVICE_IET_WDT
 * \brief   Definitions for wdt driver io-control commands
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure or value, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref RET_WDT_OK,     Control device successfully
 * @{
 */
/** Define WDT control commands for common usage */
#define DEV_SET_WDT_SYSCMD(cmd)     DEV_SET_SYSCMD(DEV_SYS_CMDBSE|(cmd))

/* ++++ Common commands for TIMER Device ++++ */
/**
 * Set WDT timerout
 * - Param type : uint32_t
 * - Param usage : NULL
 * - Return value explanation :
 */
#define WDT_CMD_SET_TIMEROUT        DEV_SET_WDT_SYSCMD(0)

/**
 * Set WDT feed dog.
 * - Param type : NULL
 * - Param usage : NULL
 * - Return value explanation :
 */
#define WDT_CMD_FEED                DEV_SET_WDT_SYSCMD(1)

/**
 * Get WDT current value.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation :
 */
#define WDT_CMD_CURRENT_VALUE       DEV_SET_WDT_SYSCMD(2)
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    DEVICE_IET_TIMER_FUN IET timer driver functions
 * \ingroup     DEVICE_IET_TIMER
 * \brief       API functions for IET timer driver
 * @{
 */

/**
  \fn           void iet_wdt_open(WDT_ResetPulseLen_TypeDef reset_len, WDT_Timerout_TypeDef timeout)
  \brief        open wdt
  \param[in]    reset_len       number clk after reset.
  \param[in]    timerout        timerout of watchdog.
  \return       NULL
*/
extern void iet_wdt_open(WDT_ResetPulseLen_TypeDef reset_len, WDT_Timerout_TypeDef timeout);

/**
  \fn           uint32_t iet_wdt_control(uint32_t ctrl_cmd, void *param)
  \brief        wdt control function.
  \param[in]    ctrl_cmd            io-control command.
  \param[in]    param               command params.
  \return       RET_WDT_OK
*/
extern uint32_t iet_wdt_control(uint32_t ctrl_cmd, void *param);

extern uint8_t iet_get_wdt_int_stat();

extern void iet_clear_wdt_int();

/** @} */
#ifdef __cplusplus
}
#endif
/** @} */

#endif /* __IET_WDT_H__ */

