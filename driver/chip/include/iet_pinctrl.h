/*****************************************************************************
*  PinCtrl Controller Driver Head File for Bach.
*
*  Copyright (C) 2018 ZengHua.Gao,  zhgao@intenginetech.com.
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
*  @file     iet_pinctrl.h
*  @brief    PinCtrl Controller Driver Head File
*  @author   ZengHua.Gao
*  @email    zhgao@intenginetech.com
*  @version  1.0
*  @date     2019-04-03
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2019/04/05 | 1.0   | ZengHua.Gao      | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/
#ifndef IET_PINCTRL_H_
#define IET_PINCTRL_H_

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"
/**
 * \defgroup    DEVICE_IET_PINCTRL  IET PINCTRL Device Driver Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for PINCTRL driver Interface (\ref iet_pinctrl.h)
 * \details provide interfaces for PINCTRL driver to implement
 * @{
 *
 * \file
 * \brief   PINCTRL device definitions
 * \details provide common definitions for PINCTRL device,
 *  then software developer can develop PINCTRL driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */

/**
 * \defgroup    DEVICE_IET_PINCTRL_DEVSTRUCT    IET PINCTRL Driver Config Structure
 * \ingroup DEVICE_IET_PINCTRL
 * \brief   contains definitions for PINCTRL driver config structure.
 * \details this structure will be used in user implemented code, which was called
 *     Device Driver Implement Layer for PINCTRL to realize in user code.
 * @{
 */
/**
 * \brief   IET PINCTRL group and pin config definition
 * @{
 */
/**
 * \brief   IET PINCTRL GROUP SHARE1 and SHARE2 definition
 * @{
 */
#define PINCTRL_GROUP_NUM (2)
#define PINCTRL_GROUP1_PIN (32)
#define PINCTRL_GROUP2_PIN (4)
#define PINCTRL_NUM ((PINCTRL_GROUP1_PIN) + (PINCTRL_GROUP2_PIN))

typedef enum
{
    IET_PINCTRL_GROUP0      = 0,    /*!< 0x50b00610 [0-6][8-31] */
    IET_PINCTRL_GROUP1      = 1,    /*!< 0x50b00614 [0-3] */
} PINCTRL_GROUP;

//#define IS_PINCTRL_GROUP(VAL) ((VAL == 0) || (VAL == 1))
typedef enum
{
    IET_PINCTRL_PWM1        = 0,
    IET_PINCTRL_DTESTO0,
    IET_PINCTRL_PWM0,
    IET_PINCTRL_GPIOA1,
    IET_PINCTRL_DTESTO1,
    IET_PINCTRL_I2S_SDI0,
    IET_PINCTRL_GPIOA2,
    IET_PINCTRL_PIN7,
    IET_PINCTRL_DTESTO2,
    IET_PINCTRL_I2S,
    IET_PINCTRL_GPIOA3,
    IET_PINCTRL_GPIOB0,
    IET_PINCTRL_DTESTO3,
    IET_PINCTRL_GPIOB1,
    IET_PINCTRL_DTESTO4,
    IET_PINCTRL_I2S_SDO0,
    IET_PINCTRL_GPIOB2,
    IET_PINCTRL_DTESTO5,
    IET_PINCTRL_MI2C,
    IET_PINCTRL_GPIOB3,
    IET_PINCTRL_DTESTO6,
    IET_PINCTRL_GPIOC0,
    IET_PINCTRL_DTESTO7,
    IET_PINCTRL_PDM_DATA1,
    IET_PINCTRL_GPIOC1,
    IET_PINCTRL_PDM_DATA2,
    IET_PINCTRL_GPIOC2,
    IET_PINCTRL_GPIOC3,
    IET_PINCTRL_UART0,
    IET_PINCTRL_GPIOD0,
    IET_PINCTRL_GPIOD1,
    IET_PINCTRL_UART0_TS,
} PINCTRL_SHARE1;
//#define IS_PINCTRL_SHARE1(GROUP, PIN) (((PIN) >= IET_PINCTRL_PWM1) && ((PIN) <= IET_PINCTRL_UART0_TS) && ((PIN) != IET_PINCTRL_PIN7))
typedef enum
{
    IET_PINCTRL_GPIOD2      = 0,
    IET_PINCTRL_GPIOD3      = 1,
    IET_PINCTRL_MI2C_BK     = 2,
    IET_PINCTRL_UART0_BK    = 3,
} PINCTRL_SHARE2;
typedef enum
{
    IET_BOTHEDAGE_GPIOA0    = 0,
    IET_BOTHEDAGE_GPIOA1    = 1,
} PINCTRL_SHARE3;
//#define IS_PINCTRL_SHARE2(PIN) (((PIN) >= IET_PINCTRL_GPIOD2) && ((PIN) <= IET_PINCTRL_UART0_BK))
#define IS_PINCTRL_GROUP_PIN(GROUP, PIN) (((GROUP == IET_PINCTRL_GROUP0) && (((PIN) >= IET_PINCTRL_PWM1) && ((PIN) <= IET_PINCTRL_UART0_TS) && ((PIN) != IET_PINCTRL_PIN7))) || \
    ((GROUP == IET_PINCTRL_GROUP1) && (((PIN) >= IET_PINCTRL_GPIOD2) && ((PIN) <= IET_PINCTRL_UART0_BK))))
/** @} */

/**
 * \brief   IET PINCTRL group and pin structure definition
 * \details define PINCTRL_Init_TypeDef
 * @{
 */
typedef struct
{
    uint8_t     group;            /*!< pin ctrl group number */
    uint8_t     pin;              /*!< pin ctrl pin number */
} PINCTRL_Init_TypeDef;
/** @} */
/** @} */

/**
 * \defgroup    DEVICE_IET_PINCTRL_CTRLCMD      IET PINCTRL Driver Io-Control Commands
 * \ingroup DEVICE_IET_PINCTRL
 * \brief   Definitions for PINCTRL driver io-control commands
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref RET_PINCTRL_OK,     Control device successfully
 * @{
 */
/** Define PINCTRL control commands for common usage */
#define DEV_PINCTRL_SYSCMD(cmd)     DEV_SET_SYSCMD(DEV_SYS_CMDBSE|(cmd))
/* ++++ Common commands for PINCTRL Device ++++ */
/** @} */

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \defgroup    DEVICE_IET_PINCTRL_FUN PinCtrl Driver Functions
 * \ingroup     DEVICE_IET_PINCTRL
 * \brief       Definitions for PinCtrl Driver API functions
 * @{
 */
/**
  \fn int32_t   iet_pinctrl_open(PINCTRL_Init_TypeDef* pinctrl_init)
  \brief        PINCTRL open function.
  \param[in]    pinctrl_init        PinCtrl group and pin number.
  \return       RET_PINCTRL_OK      Open successfully
  \return       RET_PINCTRL_OPEND   If device was opened before then just return RET_PINCTRL_OPEND
*/
extern int32_t  iet_pinctrl_open(PINCTRL_Init_TypeDef* pinctrl_init);

/**
  \fn int32_t   iet_pinctrl_close(PINCTRL_Init_TypeDef* pinctrl_init);
  \brief        PINCTRL close function.
  \param[in]    pinctrl_init        PinCtrl group and pin number.
  \return       RET_PINCTRL_OK      Close successfully
  \return       RET_PINCTRL_CLSED   Close failed
*/
extern int32_t  iet_pinctrl_close(PINCTRL_Init_TypeDef* pinctrl_init);

/**
  \fn int32_t   iet_pinctrl_unmux(PINCTRL_Init_TypeDef* pinctrl_init);
  \brief        PINCTRL unmux function.
  \param[in]    pinctrl_init        PinCtrl group and pin number.
  \return       RET_PINCTRL_OK      UnMux successfully
  \return       RET_PINCTRL_UNMUX   UnMux failed
*/
extern int32_t  iet_pinctrl_unmux(PINCTRL_Init_TypeDef* pinctrl_init);

/**
  \fn int32_t   iet_pinctrl_mux(PINCTRL_Init_TypeDef* pinctrl_init);
  \brief        PINCTRL mux function.
  \param[in]    pinctrl_init        PinCtrl group and pin number.
  \return       RET_PINCTRL_OK      Mux successfully
  \return       RET_PINCTRL_MUX     Mux failed
*/
extern int32_t  iet_pinctrl_mux(PINCTRL_Init_TypeDef* pinctrl_init);
/** @} end of group */
#ifdef __cplusplus
}
#endif
/** @} */
#endif /* IET_PINCTRL_H_ */
/* --------------------------------- End Of File ------------------------------ */
