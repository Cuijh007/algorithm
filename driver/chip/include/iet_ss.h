/*****************************************************************************
*  SS Controller Driver Head File for Bach2c.
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
*  @file     iet_ss.h
*  @brief    iet SS Controller Driver Head File
*  @author   ZengHua.Gao
*  @email    zhgao@intenginetech.com
*  @version  1.0
*  @date     2020-04-21
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/04/21 |    1.0    |  ZengHua.Gao   | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __IET_SS_H__
#define __IET_SS_H__

#include "iet_common.h"

/**
 * \defgroup    DEVICE_IET_SS  IET SS Controller Driver Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for SS Controller Driver (\ref iet_ss.h)
 * \details provide interfaces for ss driver to implement
 * @{
 *
 * \file
 * \brief   ss controller definitions
 * \details provide common definitions for ss controller,
 *  then software developer can develop ss driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */
/**
 * \brief   Definitions for SS driver
 * @{
 */
/**
 * \brief   Macro definitions for SS driver and app
 * @{
 */
#define IET_SS_CH_NUM                   (0)     /*!< bit 0: Input audio channel number */
#define IET_SS_CH_NUM_MASK              (0xf << IET_SS_CH_NUM) /*!< width:4 */
#define IET_SS_CH_NUM_VAL(x)            ((x & 0xf) << IET_SS_CH_NUM)

#define IET_SS_CH_SEL                   (4)     /*!< bit 4: Audio channel select, selected channel will be used to run VAD */
#define IET_SS_CH_SEL_MASK              (0xf << IET_SS_CH_SEL) /*!< width:4 */
#define IET_SS_CH_SEL_VAL(x)            ((x & 0xf) << IET_SS_CH_SEL)

#define IET_SS_WRITE_BUF                (8)     /*!< bit 8: Enable APB sample writing to sample buffer [0:disable; 1:enable] */
#define IET_SS_WRITE_BUF_DIS            (0)     /*!< Enable APB sample writing to sample buffer */
#define IET_SS_WRITE_BUF_EN             (1)     /*!< Disable APB sample writing to sample buffer */
#define IET_SS_WRITE_BUF_MASK           (1 << IET_SS_WRITE_BUF) /*!< width:1 */

#define IET_SS_BIT_REV                  (9)     /*!< bit 9: Bit reverse the 32bit audio sample data [0:disable; 1:enable] */
#define IET_SS_BIT_REV_DIS              (0)     /*!< Enable Bit reverse the 32bit audio sample data */
#define IET_SS_BIT_REV_EN               (1)     /*!< Disable Bit reverse the 32bit audio sample data */
#define IET_SS_BIT_REV_MASK             (1 << IET_SS_BIT_REV) /*!< width:1 */

#define IET_SS_SHIFT                    (10)    /*!< bit 10: Right shift the bit reversed audio sample data [0:disable; 1:enable] */
#define IET_SS_SHIFT_DIS                (0)     /*!< Enable Right shift the bit reversed audio sample data */
#define IET_SS_SHIFT_EN                 (1)     /*!< Disable Right shift the bit reversed audio sample data */
#define IET_SS_SHIFT_MASK               (1 << IET_SS_SHIFT) /*!< width:5 */
#define IET_SS_SHIFT_VAL(x)             ((x & 0x1f) << IET_SS_SHIFT)

#define IET_SS_SAMPLE_RING_CUR          (0)    /*!< bit 0: Current sample buffer pointer which hardware written to, will be updated on every audio frame available */
#define IET_SS_SAMPLE_RING_CUR_MASK     (0x3ffff << IET_SS_SAMPLE_RING_CUR) /*!< width:18 */

#define IET_SS_INTR_STATUS              (0)     /*!< bit 0: frame saved interrupt status */
#define IET_SS_INTR_STATUS_MASK         (1 << IET_SS_INTR_STATUS) /*!< width:1 */
/** @} */
/**
 * \brief   IET SS Channel Number enum definition
 * \details define IET_SS_CH_NUM_VAL
 * @{
 */
typedef enum
{
    IET_SS_CH_NUM_1 = 1,    /*!< ss channel number 1 */
    IET_SS_CH_NUM_2 = 2,    /*!< ss channel number 2 */
    IET_SS_CH_NUM_3 = 3,    /*!< ss channel number 3 */
    IET_SS_CH_NUM_4 = 4,    /*!< ss channel number 4 */
} IET_SS_CH_NUM_VAL;
/** @} */
/**
 * \brief   IET SS Select Channel enum definition
 * \details define IET_SS_CH_NUM_VAL
 * @{
 */
typedef enum
{
    IET_SS_CH_SEL_0 = 0,    /*!< ss select channel 0 */
    IET_SS_CH_SLE_1 = 1,    /*!< ss select channel 1 */
    IET_SS_CH_SEL_2 = 2,    /*!< ss select channel 2 */
    IET_SS_CH_SEL_3 = 3,    /*!< ss select channel 3 */
} IET_SS_CH_SEL_VAL;
/** @} */

/**
 * \brief   Definitions for SS param check
 * @{
 */
#define IS_IET_SS_CHNUM(VAL)            ((VAL == IET_SS_CH_NUM_1) || (VAL == IET_SS_CH_NUM_2) || (VAL == IET_SS_CH_NUM_3) || (VAL == IET_SS_CH_NUM_4))
#define IS_IET_SS_CHSEL(VAL)            ((VAL == IET_SS_CH_SEL_0) || (VAL == IET_SS_CH_SEL_1) || (VAL == IET_SS_CH_SEL_2) || (VAL == IET_SS_CH_SEL_3))
#define IS_IET_SS_WRITE_BUF(VAL)        ((VAL == IET_SS_WRITE_BUF_DIS) || (VAL == IET_SS_WRITE_BUF_EN))
#define IS_IET_SS_BIT_REV(VAL)          ((VAL == IET_SS_BIT_REV_DIS) || (VAL == IET_SS_BIT_REV_EN))
#define IS_IET_SS_SHIFT(VAL)            ((VAL == IET_SS_SHIFT_DIS) || (VAL == IET_SS_SHIFT_EN))
#define IS_IET_SS_ADDR(VAL)             IS_VALID_VAL(VAL)
#define IS_IET_SS_LENGHT(VAL)           IS_LENGTH_VAL(VAL)
/** @} */
/**
 * \brief   IET SS config structure definition
 * @{
 */
typedef struct
{
    int16_t    *addr;          /*!< ss golden data address */
    uint32_t    len;            /*!< ss golden data len */
} IET_SS_GoldenData_TypeDef;
/** @} */

/**
 * \brief   IET SS config structure definition
 * @{
 */
typedef struct
{
    uint32_t    mode;           /*!< ss operation mode [IET_SS_WRITE_BUF_DIS/IET_SS_WRITE_BUF_EN] */
    uint32_t    num;            /*!< ss Input audio channel number [IET_SS_CH_NUM] */
    uint32_t    ch;             /*!< ss Audio channel select [IET_SS_CH_SEL] */
    uint32_t    bitrev;         /*!< ss Bit reverse the 32bit audio sample data [IET_SS_BIT_REV_DIS/IET_SS_BIT_REV_EN] */
    uint32_t    shift;          /*!< ss Right shift the bit reversed audio sample data [IET_SS_SHIFT_DIS/IET_SS_BIT_REV_EN] */
    uint16_t    *addr;          /*!< ss outputbuffer address */
    uint32_t    len;            /*!< ss outputbuffer len */
    void*       callback;       /*!< ss callback */
} IET_SS_Config_TypeDef;
/** @} */
/** @} */

/**
 * \defgroup    DEVICE_IET_SS_CTRLCMD      SS Device Control Commands
 * \ingroup DEVICE_IET_SS
 * \brief   Definitions for SS Driver Control Command
 * \details These commands defined here can be used in user code directly
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - RET_SS_OK,      Control device successfully
 *   - RET_SS_OPNED,   Device is opened, The second time can not be opened
 *   - RET_SS_PAR,     Parameter is not valid for current control command
 *   - RET_SS_IO,      Control command is not supported or not valid
 * @{
 */
/** Define SS control commands for common usage */
#define DEV_SET_SS_SYSCMD(cmd)     DEV_SET_SYSCMD(0x00030000|(cmd))
/** Define SS control commands for master usage */
#define DEV_SET_SS_MST_SYSCMD(cmd) DEV_SET_SYSCMD(0x00034000|(cmd))
/** Define SS control commands for slave usage */
#define DEV_SET_SS_SLV_SYSCMD(cmd) DEV_SET_SYSCMD(0x00038000|(cmd))

/* ++++ Common commands for SS dirver ++++ */
/**
 * Set SS dirver config
 * - Param type : IET_SS_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_SS_CMD_S_CONFIG             DEV_SET_SS_SYSCMD(0)

/**
 * Get SS dirver config
 * - Param type : IET_SS_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_SS_CMD_G_CONFIG             DEV_SET_SS_SYSCMD(1)

/**
 * Get SS dirver intr status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_SS_CMD_G_INTR_STATUS        DEV_SET_SS_SYSCMD(2)

/**
 * Get SS output-ring-buffer
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_SS_CMD_G_RING_BUF           DEV_SET_SS_SYSCMD(3)

/**
 * Write SS Dolden Data to 0x40 register
 * - Param type : IET_SS_GoldenData_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_SS_CMD_W_GOLDEN_DATA        DEV_SET_SS_SYSCMD(4)
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    DEVICE_IET_SS_FUN SS Driver Functions
 * \ingroup     DEVICE_IET_SS
 * \brief       Definitions for SS Driver API functions
 * @{
 */
/**
  \fn           uint32_t iet_ss_open(IET_SS_Config_TypeDef *ss_config)
  \brief        open iet ss
  \param[in]    ss_config      ss config struct IET_SS_Config_TypeDef
  \return       RET_SS_OK      open successfully without any issues
  \return       RET_SS_OPNED   if device was opened before with different parameters, return RET_SS_OPNED
  \return       RET_SS_PAR     parameter is not valid
 */
extern uint32_t iet_ss_open(IET_SS_Config_TypeDef *ss_config);

/**
  \fn           int32_t iet_ss_close(void)
  \brief        close iet ss
  \return       RET_SS_OK      close successfully
*/
extern uint32_t iet_ss_close(void);

/**
  \fn           iet_ss_control(uint32_t ctrl_cmd, void *param)
  \brief        control ss by control command [optional]
  \param[in]    ctrl_cmd        ref DEVICE_IET_SS_CTRLCMD "control command", to change or get some thing related to ss
  \param[in,out]
                param           parameters that maybe argument of the command, or return values of the command, must not be null
  \return       RET_SS_OK      control device successfully
  \return       RET_SS_PAR     parameter is not valid for current control command
  \return       RET_SS_IO      control command is not supported or not valid
*/
extern uint32_t iet_ss_control(uint32_t ctrl_cmd, void *param);

/**
  \fn           int32_t iet_ss_start(void)
  \brief        start ss
  \return       RET_SS_OK      start successfully
*/
extern uint32_t iet_ss_start(void);

/**
  \fn           int32_t iet_ss_stop(void)
  \brief        stop ss
  \return       RET_SS_OK      stop successfully
*/
extern uint32_t iet_ss_stop(void);
/** @} */
#ifdef __cplusplus
}
#endif
/** @} */
/** @} */

#endif /* __IET_SS_H__ */

/* --------------------------------- End Of File ------------------------------ */
