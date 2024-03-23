/*****************************************************************************
*  FFT Controller Driver Head File for Bach2c.
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
*  @file     iet_fft.h
*  @brief    iet FFT Controller Driver Head File
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
*  2020/03/27 | 1.0   | ZengHua.Gao      | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __IET_FFT_H__
#define __IET_FFT_H__
#include "iet_common.h"

/**
 * \defgroup    DEVICE_IET_FFT  IET FFT Controller Driver Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for FFT Controller Driver (\ref iet_fft.h)
 * \details provide interfaces for fft driver to implement
 * @{
 *
 * \file
 * \brief   fft controller definitions
 * \details provide common definitions for fft controller,
 *  then software developer can develop fft driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */
/**
 * \brief   Definitions for FFT modules.
 * @{
 */
/**
 * \brief   Macro Definitions for FFT driver and app.
 * @{
 */
#define IET_FFT_MODE               (1)     /*!< bit 1: FFT module working mode [0: manual; 1: auto] */
#define IET_FFT_MODE_MANUA         (0)     /*!< FFT module working mode is manual */
#define IET_FFT_MODE_AUTO          (1)     /*!< FFT module working mode is auto */
#define IET_FFT_MODE_MFCC          (2)     /*!< FFT module use by MFCC module */
#define IET_FFT_MODE_MASK          (1 << IET_FFT_MODE) /*!< width:1 */

#define IET_START_FFT              (5)     /*!< bit 5: Writing 1 to start FFT module */
#define IET_START_FFT_EN           (1)     /*!< Writing 1 to start FFT module */
#define IET_START_FFT_MASK         (1 << IET_START_FFT) /*!< width:1 */

#define IET_FFT_STATUS             (2)     /*!< bit 2: FFT module status [0:busy; 1:idle] */
#define IET_FFT_STATUS_MASK        (1 << IET_FFT_STATUS) /*!< width:1 */
#define IET_FFT_INTR_STATUS        (2)     /*!< bit 2: FFT done interrupt status */
#define IET_FFT_INTR_STATUS_MASK   (1 << IET_FFT_INTR_STATUS) /*!< width:1 */
/** @} */

/**
 * \brief   Definitions for FFT param check.
 * @{
 */
#define IS_IET_FFT_MODE(VAL)       ((VAL == IET_FFT_MODE_AUTO) || (VAL == IET_FFT_MODE_MANUA))
#define IS_IET_FFT_ADDR(VAL)       IS_VALID_VAL(VAL)
/** @} */

/**
 * \brief   IET FFT config structure definition
 * @{
 */
typedef struct
{
    uint32_t mode;          /*!< fft operation mode [IET_FFT_MODE_AUTO/IET_FFT_MODE_MANUA/IET_FFT_MODE_MFCC] */
    uint32_t *inout;        /*!< fft inputbuffer/outputbuffer address [512*32bit] */
    void*    callback;      /*!< fft callback fun */
} IET_FFT_Config_TypeDef;
/** @} */
/** @} */

/**
 * \defgroup    DEVICE_IET_FFT_CTRLCMD      FFT Device Control Commands
 * \ingroup DEVICE_IET_FFT
 * \brief   Definitions for FFT Driver Control Command
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/uint32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - RET_FFT_OK,      Control device successfully
 *   - RET_FFT_OPNED,   Device is opened, The second time can not be opened.
 *   - RET_FFT_PAR,     Parameter is not valid for current control command
 *   - RET_FFT_IO,      Control command is not supported or not valid
 * @{
 */
/** Define FFT control commands for common usage */
#define DEV_SET_FFT_SYSCMD(cmd)     DEV_SET_SYSCMD(0x00030000|(cmd))
/** Define FFT control commands for master usage */
#define DEV_SET_FFT_MST_SYSCMD(cmd) DEV_SET_SYSCMD(0x00034000|(cmd))
/** Define FFT control commands for slave usage */
#define DEV_SET_FFT_SLV_SYSCMD(cmd) DEV_SET_SYSCMD(0x00038000|(cmd))

/* ++++ Common commands for FFT driver ++++ */
/**
 * Set FFT driver config
 * - Param type : IET_FFT_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_FFT_CMD_S_CONFIG            DEV_SET_FFT_SYSCMD(0)

/**
 * Get FFT driver config
 * - Param type : IET_FFT_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_FFT_CMD_G_CONFIG            DEV_SET_FFT_SYSCMD(1)

/**
 * Get FFT driver status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_FFT_CMD_G_STATUS            DEV_SET_FFT_SYSCMD(2)

/**
 * Get FFT driver intr status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_FFT_CMD_G_INTR_STATUS       DEV_SET_FFT_SYSCMD(3)

/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    DEVICE_IET_FFT_FUN FFT Driver Functions
 * \ingroup     DEVICE_IET_FFT
 * \brief       Definitions for FFT Driver API functions
 * @{
 */
/**
  \fn           uint32_t iet_fft_open(IET_FFT_Config_TypeDef *fft_config)
  \brief        open iet fft
  \param[in]    fft_config      fft config struct IET_FFT_Config_TypeDef
  \return       RET_FFT_OK      open successfully without any issues
  \return       RET_FFT_OPNED   if device was opened before with different parameters, return RET_FFT_OPNED
  \return       RET_FFT_PAR     parameter is not valid
 */
extern uint32_t iet_fft_open(IET_FFT_Config_TypeDef *fft_config);

/**
  \fn           int32_t iet_fft_close(void)
  \brief        close iet fft
  \return       RET_FFT_OK      close successfully
*/
extern uint32_t iet_fft_close(void);

/**
  \fn           iet_fft_control(uint32_t ctrl_cmd, void *param)
  \brief        control fft by control command
  \param[in]    ctrl_cmd        ref DEVICE_IET_FFT_CTRLCMD "control command", to change or get some thing related to fft
  \param[in,out]
                param           parameters that maybe argument of the command, or return values of the command, must not be null
  \return       RET_FFT_OK      control device successfully
  \return       RET_FFT_PAR     parameter is not valid for current control command
  \return       RET_FFT_IO      control command is not supported or not valid
*/
extern uint32_t iet_fft_control(uint32_t ctrl_cmd, void *param);

/**
  \fn           int32_t iet_fft_start(void)
  \brief        start fft
  \return       RET_FFT_OK      start successfully
*/
extern uint32_t iet_fft_start(void);

/**
  \fn           int32_t iet_fft_stop(void)
  \brief        stop fft
  \return       RET_FFT_OK      stop successfully
*/
extern uint32_t iet_fft_stop(void);
/** @} */
#ifdef __cplusplus
}
#endif
/** @} */
/** @} */

#endif /* __IET_FFT_H__ */

/* --------------------------------- End Of File ------------------------------ */
