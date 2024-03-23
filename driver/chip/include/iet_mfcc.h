/*****************************************************************************
*  MFCC Controller Driver Head File for Bach2c.
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
*  @file     iet_mfcc.h
*  @brief    iet MFCC Controller Driver Head File
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
*  2020/03/30 | 1.0       | ZengHua.Gao    | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __IET_MFCC_H__
#define __IET_MFCC_H__
#include "iet_common.h"

/**
 * \defgroup    DEVICE_IET_MFCC  IET MFCC Controller Driver Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for MFCC Controller Driver (\ref iet_mfcc.h)
 * \details provide interfaces for mfcc driver to implement
 * @{
 *
 * \file
 * \brief   mfcc controller definitions
 * \details provide common definitions for mfcc controller,
 *  then software developer can develop mfcc driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */
/**
 * \brief   Definitions for MFCC modules.
 * @{
 */
/**
 * \brief   Macro Definitions for MFCC driver and app.
 * @{
 */
#define IET_MFCC_OUTBUF_BLOCK           (15)    /*!< bit 15: Number of fbank output ring buffer, every buffer size is 104 bytes */
#define IET_MFCC_OUTBUF_BLOCK_MASK      (7 << IET_MFCC_OUTBUF_BLOCK) /*!< width:3 */
#define IET_MFCC_OUTBUF_BLOCK_VAL(x)    ((x & 0x7) << IET_MFCC_OUTBUF_BLOCK)


#define IET_MFCC_WRITE_TO_VAD           (18)    /*!< bit 18: Control writing fbank data to VAD module [0:enable; 1:disable] */
#define IET_MFCC_WRITE_TO_VAD_DIS       (0)     /*!< NOT Write MFCC output data to VAD module */
#define IET_MFCC_WRITE_TO_VAD_EN        (1)     /*!< Write MFCC output data to VAD module */
#define IET_MFCC_WRITE_TO_VAD_MASK      (1 << IET_MFCC_WRITE_TO_VAD) /*!< width:1 */

#define IET_MFCC_WRITE_TO_MEM           (19)    /*!< bit 19: Control writing fbank data to system memory [0:enable; 1:disable] */
#define IET_MFCC_WRITE_TO_MEM_DIS       (0)     /*!< NOT Write MFCC output data to memory */
#define IET_MFCC_WRITE_TO_MEM_EN        (1)     /*!< Write MFCC output data to memory */
#define IET_MFCC_WRITE_TO_MEM_MASK      (1 << IET_MFCC_WRITE_TO_MEM) /*!< width:1 */

#define IET_MFCC_FFT_BYPASS             (28)    /*!< bit 28: Control fft bypass or not [0:not bypass; 1:bypass] */
#define IET_MFCC_FFT_BYPASS_DIS         (0)     /*!< NOT fft bypass */
#define IET_MFCC_FFT_BYPASS_EN          (1)     /*!< fft bypass */
#define IET_MFCC_FFT_BYPASS_MASK        (1 << IET_MFCC_FFT_BYPASS) /*!< width:1 */

#define IET_MFCC_FFT_ORDER              (29)    /*!< bit 29: Control fft result order [0:reversed; 1:natural] */
#define IET_MFCC_FFT_ORDER_REV          (0)     /*!< fft result reversed order */
#define IET_MFCC_FFT_ORDER_NATURAL      (1)     /*!< fft result natural order */
#define IET_MFCC_FFT_ORDER_MASK         (1 << IET_MFCC_FFT_ORDER) /*!< width:1 */

#define IET_MFCC_MODE                   (2)     /*!< bit 2: MFCC module working mode [0: manual; 1: auto] */
#define IET_MFCC_MODE_MANUA             (0)     /*!< MFCC module working mode is manual */
#define IET_MFCC_MODE_AUTO              (1)     /*!< MFCC module working mode is auto */
#define IET_FFT_MFCC_MODE_MFCC          (2)     /*!< FFT module use by MFCC module */
#define IET_MFCC_MODE_MASK              (1 << IET_MFCC_MODE) /*!< width:1 */

#define IET_START_MFCC                  (6)     /*!< bit 6: Writing 1 to start MFCC module */
#define IET_START_MFCC_EN               (1)     /*!< Writing 1 to start MFCC module */
#define IET_START_MFCC_MASK             (1 << IET_START_MFCC) /*!< width:1 */

#define IET_MFCC_CUR_RING_BUF           (8)    /*!< bit 8: Current buffer pointer which hardware written to, will be updated on every frame fbank  available */
#define IET_MFCC_CUR_RING_BUF_MASK      (7 << IET_MFCC_CUR_RING_BUF) /*!< width:3 */
#define IET_MFCC_CUR_RING_BUF_VAL(x)    ((x & 0x7) << IET_MFCC_CUR_RING_BUF) /*!< width:3 */

#define IET_MFCC_STATUS                 (3)     /*!< bit 3: MFCC module status [0:busy; 1:idle] */
#define IET_MFCC_STATUS_MASK            (1 << IET_MFCC_STATUS) /*!< width:1 */
#define IET_MFCC_INTR_STATUS            (3)     /*!< bit 3: post-process done interrupt status */
#define IET_MFCC_INTR_STATUS_MASK       (1 << IET_MFCC_INTR_STATUS) /*!< width:1 */
/** @} */

/**
 * \brief   Definitions for MFCC param check
 * @{
 */
#define IS_IET_MFCC_MODE(VAL)           ((VAL == IET_MFCC_MODE_AUTO) || (VAL == IET_MFCC_MODE_MANUA))
#define IS_IET_MFCC_VAD(VAL)            ((VAL == IET_MFCC_WRITE_TO_VAD_EN) || (VAL == IET_MFCC_WRITE_TO_VAD_DIS))
#define IS_IET_MFCC_MEM(VAL)            ((VAL == IET_MFCC_WRITE_TO_MEM_EN) || (VAL == IET_MFCC_WRITE_TO_MEM_DIS))
#define IS_IET_MFCC_BYPASS(VAL)         ((VAL == IET_MFCC_FFT_BYPASS_EN) || (VAL == IET_MFCC_FFT_BYPASS_DIS))
#define IS_IET_MFCC_ORDER(VAL)          ((VAL == IET_MFCC_FFT_ORDER_REV) || (VAL == IET_MFCC_FFT_ORDER_NATURAL))
#define IS_IET_MFCC_ADDR(VAL)           IS_VALID_VAL(VAL)
#define IS_IET_MFCC_LEN(VAL)            IS_LENGTH_VAL(VAL)
/** @} */
/**
 * \brief   IET MFCC config structure definition
 * @{
 */
typedef struct
{
    uint32_t mode;              /*!< mfcc operation mode [IET_MFCC_MODE_AUTO/IET_MFCC_MODE_MANUA] */
    uint32_t cfg;               /*!< mfcc configuration parameters buffer address, set to [19:2] of hardware address */
    uint32_t *in;                /*!< mfcc inputbuffer address [fft-outbuffer] */
    uint32_t out;               /*!< mfcc outputbuffer address */
    uint32_t block;             /*!< mfcc output buffer number, every buffer size is 104 bytes (26 integers) */
    uint32_t vad;               /*!< mfcc outputbuffer to vad [IET_MFCC_WRITE_TO_VAD_EN/IET_MFCC_WRITE_TO_VAD_DIS] */
    uint32_t mem;               /*!< mfcc outputbuffer to mem [IET_MFCC_WRITE_TO_MEM_EN/IET_MFCC_WRITE_TO_MEM_DIS] */
    uint32_t bypass;            /*!< mfcc fft bypass or not [IET_MFCC_FFT_BYPASS_EN/IET_MFCC_FFT_BYPASS_DIS] */
    uint32_t order;             /*!< mfcc outputbuffer to mem [IET_MFCC_FFT_ORDER_REV/IET_MFCC_FFT_ORDER_NATURAL] */
    void *callback;             /*!< mfcc callback */
} IET_MFCC_Config_TypeDef;
/** @} */
/** @} */

/**
 * \defgroup    DEVICE_IET_MFCC_CTRLCMD      MFCC Device Control Commands
 * \ingroup DEVICE_IET_MFCC
 * \brief   Definitions for MFCC Driver Control Command
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - RET_MFCC_OK,      Control device successfully
 *   - RET_MFCC_OPEND,   Device is opened, The second time can not be opened.
 *   - RET_MFCC_PAR,     Parameter is not valid for current control command
 *   - RET_MFCC_IO,      Control command is not supported or not valid
 * @{
 */
/** Define MFCC control commands for common usage */
#define DEV_SET_MFCC_SYSCMD(cmd)     DEV_SET_SYSCMD(0x00030000|(cmd))
/** Define MFCC control commands for master usage */
#define DEV_SET_MFCC_MST_SYSCMD(cmd) DEV_SET_SYSCMD(0x00034000|(cmd))
/** Define MFCC control commands for slave usage */
#define DEV_SET_MFCC_SLV_SYSCMD(cmd) DEV_SET_SYSCMD(0x00038000|(cmd))

/* ++++ Common commands for MFCC Device ++++ */
/**
 * Set MFCC driver config
 * - Param type : IET_MFCC_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_MFCC_CMD_S_CONFIG            DEV_SET_MFCC_SYSCMD(0)

/**
 * Get MFCC driver config
 * - Param type : IET_MFCC_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_MFCC_CMD_G_CONFIG            DEV_SET_MFCC_SYSCMD(1)

/**
 * Get MFCC driver status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_MFCC_CMD_G_STATUS            DEV_SET_MFCC_SYSCMD(2)

/**
 *  Get MFCC driver intr status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_MFCC_CMD_G_INTR_STATUS       DEV_SET_MFCC_SYSCMD(3)

/**
 * Get MFCC outbuffer ring buf
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_MFCC_CMD_G_RING_BUF          DEV_SET_MFCC_SYSCMD(4)
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    DEVICE_IET_MFCC_FUN MFCC Driver Functions
 * \ingroup     DEVICE_IET_MFCC
 * \brief       Definitions for MFCC Driver API functions
 * @{
 */
/**
  \fn           uint32_t iet_mfcc_open(IET_MFCC_Config_TypeDef *mfcc_config)
  \brief        open iet mfcc
  \param[in]    mfcc_config      mfcc config struct IET_MFCC_Config_TypeDef
  \return       RET_MFCC_OK      open successfully without any issues
  \return       RET_MFCC_OPNED   if device was opened before with different parameters, return RET_MFCC_OPNED
  \return       RET_MFCC_PAR     parameter is not valid
 */
extern uint32_t iet_mfcc_open(IET_MFCC_Config_TypeDef *mfcc_config);

/**
  \fn           int32_t iet_mfcc_close(void)
  \brief        close iet mfcc
  \return       RET_MFCC_OK      close successfully
*/
extern uint32_t iet_mfcc_close(void);

/**
  \fn           iet_mfcc_control(uint32_t ctrl_cmd, void *param)
  \brief        control mfcc by control command
  \param[in]    ctrl_cmd        ref DEVICE_IET_MFCC_CTRLCMD "control command", to change or get some thing related to mfcc
  \param[in,out]
                param           parameters that maybe argument of the command, or return values of the command, must not be null
  \return       RET_MFCC_OK     control device successfully
  \return       RET_MFCC_PAR    parameter is not valid for current control command
  \return       RET_MFCC_IO     control command is not supported or not valid
*/
extern uint32_t iet_mfcc_control(uint32_t ctrl_cmd, void *param);

/**
  \fn           int32_t iet_mfcc_start(void)
  \brief        start mfcc
  \return       RET_MFCC_OK     start successfully
*/
extern uint32_t iet_mfcc_start(void);

/**
  \fn           int32_t iet_mfcc_stop(void)
  \brief        stop mfcc
  \return       RET_MFCC_OK     stop successfully
*/
extern uint32_t iet_mfcc_stop(void);
/** @} */
#ifdef __cplusplus
}
#endif
/** @} */
/** @} */

#endif /* __IET_MFCC_H__ */

/* --------------------------------- End Of File ------------------------------ */
