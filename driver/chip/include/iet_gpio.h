/*****************************************************************************
*  GPIO Controller Driver Head File for Bach.
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
*  @file     iet_gpio.h
*  @brief    iet gpio File
*  @author   ZengHua.Gao
*  @email    zhgao@intenginetech.com
*  @version  1.0
*  @date     2018-8-28
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2018/08/28 | 1.0   | ZengHua.Gao      | Create file
*  2019/01/21 | 1.1   | ZengHua.Gao      | New driver framework
*----------------------------------------------------------------------------
*
*****************************************************************************/


#ifndef IET_GPIO_H_
#define IET_GPIO_H_

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"
/**
 * \defgroup    DEVICE_IET_GPIO IET GPIO Device Driver Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for gpio driver Interface (\ref iet_gpio.h)
 * \details provide interfaces for gpio driver to implement
 * @{
 *
 * \file
 * \brief   gpio device definitions
 * \details provide common definitions for gpio device,
 *  then software developer can develop gpio driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */

/**
 * \defgroup    DEVICE_IET_GPIO_DEVSTRUCT   IET GPIO Driver Config Structure
 * \ingroup DEVICE_IET_GPIO
 * \brief   contains definitions for gpio driver config structure.
 * \details this structure will be used in user implemented code, which was called
 *     Device Driver Implement Layer for gpio to realize in user code.
 * @{
 */
/**
 * \brief   IET GPIO group and pin config definition
 * @{
 */
#define GPIO_GROUP_NUM  5
#define GPIO_PIN_NUM    4

typedef enum
{
    IET_GPIO_GROUPA=0,
    IET_GPIO_GROUPB,
    IET_GPIO_GROUPC,
    IET_GPIO_GROUPD,
    IET_GPIO_GROUPE,
} GPIO_GROUP;
#define IS_GPIO_GROUP(NUM) (((NUM) == IET_GPIO_GROUPA) || ((NUM) == IET_GPIO_GROUPB) || ((NUM) == IET_GPIO_GROUPC)|| ((NUM) == IET_GPIO_GROUPD) || ((NUM) == IET_GPIO_GROUPE))

typedef enum
{
    IET_GPIO_PIN0=0,
    IET_GPIO_PIN1,
    IET_GPIO_PIN2,
    IET_GPIO_PIN3,
} GPIO_PIN;
#define IS_GPIO_PIN(NUM) (((NUM) == IET_GPIO_PIN0) || ((NUM) == IET_GPIO_PIN1) || ((NUM) == IET_GPIO_PIN2) || ((NUM) == IET_GPIO_PIN3))

#define IET_GPIO_DR_HIGH                TRUE
#define IET_GPIO_DR_LOW                 FALSE

#define IET_GPIO_DDR_OUTPUT             TRUE
#define IET_GPIO_DDR_INPUT              FALSE

#define IET_GPIO_CTL_HARDWARE           TRUE
#define IET_GPIO_CTL_SOFTWARE           FALSE

#define IET_GPIO_OPEND                  TRUE
#define IET_GPIO_NOTOPEND               FALSE

#define IET_GPIO_INT_ENABLE             ENABLE
#define IET_GPIO_INT_DISABLE            DISABLE

#define IET_GPIO_INT_CLEAR              ENABLE

#define IET_GPIO_LEVEL_ENABLE           ENABLE
#define IET_GPIO_LEVEL_DISABLE          DISABLE

#define IET_GPIO_POLARITY_ENABLE        ENABLE
#define IET_GPIO_POLARITY_DISABLE       DISABLE

#define IET_GPIO_BOTHEDAGE_ENABLE       ENABLE
#define IET_GPIO_BOTHEDAGE_DISABLE      DISABLE
/** @} */

/**
 * \brief   IET GPIO group and pin structure definition
 * \details define GPIO_Init_TypeDef
 * @{
 */
typedef struct
{
    uint8_t     gpio_group;              /*!< gpio group number */
    uint8_t     gpio_pin;                /*!< gpio pin number */
} GPIO_Init_TypeDef;
/** @} */

/**
 * \brief   IET GPIO config structure definition
 * \details define GPIO_Config_TypeDef
 * @{
 */
typedef struct
{
    uint32_t    gpio_dr;                 /*!< gpio data value [high low]*/
    uint32_t    gpio_ddr;                /*!< gpio Direction config [input output]*/
    uint32_t    gpio_ctl;                /*!< gpio source control [software hardware]*/
} GPIO_Config_TypeDef;
/** @} */

/**
 * \brief   IET GPIO interrupt config structure definition
 * \details define GPIO_Int_TypeDef
 * @{
 */
typedef struct
{
    uint8_t     gpio_int_flag;           /*!< gpio Interrupt enable/disable */
    uint8_t     gpio_int_level;          /*!< gpio Interrupt level/edge */
    uint8_t     gpio_int_polarity;       /*!< gpio Interrupt high/low */
    uint8_t     gpio_int_bothedge;       /*!< gpio Interrupt both edge */
    void*       gpio_int_callback;       /*!< gpio int callback function */
} GPIO_Int_TypeDef;
/** @} */
/** @} */

/**
 * \defgroup    DEVICE_IET_GPIO_CTRLCMD     IET GPIO Driver Io-Control Commands
 * \ingroup DEVICE_IET_GPIO
 * \brief   Definitions for gpio driver io-control commands
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref RET_GPIO_OK,        Control device successfully
 * @{
 */
/** Define GPIO control commands for common usage */
#define DEV_GPIO_SYSCMD(cmd)        DEV_SET_SYSCMD(DEV_SYS_CMDBSE|(cmd))

/* ++++ Common commands for GPIO Device ++++ */
/**
 * Set GPIO interupt config (level polarity debounce settings and so on).
 * - Param type : GPIO_Int_TypeDef *
 * - Param usage : GPIO interupt config
 * - Return value explanation :
 */
#define GPIO_CMD_SET_INT_CONFIG         DEV_GPIO_SYSCMD(0)
/**
 * Start GPIO interrupt.
 * - Param type : GPIO_Int_TypeDef *
 * - Param usage : NULL
 * - Return value explanation :
 */
#define GPIO_CMD_INT_ENABLE             DEV_GPIO_SYSCMD(1)

/**
 * Stop GPIO interrupt.
 * - Param type : GPIO_Int_TypeDef *
 * - Param usage : NULL
 * - Return value explanation :
 */
#define GPIO_CMD_INT_DISABLE            DEV_GPIO_SYSCMD(2)
/**
 * Clear GPIO interrupt.
 * - Param type : GPIO_Int_TypeDef *
 * - Param usage : NULL
 * - Return value explanation :
 */
#define GPIO_CMD_INT_CLEAR              DEV_GPIO_SYSCMD(3)
/** @} */

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \defgroup    device_iet_gpio IET GPIO Driver Functions
 * \brief       Definitions for gpio driver functions
 * @{
 */

/**
  \fn           int32_t iet_gpio_open(GPIO_Init_TypeDef* gpio_init, GPIO_Config_TypeDef* gpio_config)
  \brief        gpio open function.
  \param[in]    gpio_init       gpio group and pin number.
  \param[in]    gpio_config     gpio initstruct.
  \return       RET_GPIO_OK         Open successfully
  \return       RET_GPIO_OPEND      If device was opened before then just return \ref RET_GPIO_OPEND
*/
extern int32_t iet_gpio_open(GPIO_Init_TypeDef* gpio_init, GPIO_Config_TypeDef* gpio_config);

/**
  \fn           int32_t iet_gpio_close(GPIO_Init_TypeDef* gpio_init);
  \brief        gpio close function.
  \param[in]    gpio_init       gpio group and pin number.
  \return       RET_GPIO_OK     Close successfully
*/
extern int32_t iet_gpio_close(GPIO_Init_TypeDef* gpio_init);

/**
  \fn           iet_gpio_read(GPIO_Init_TypeDef* gpio_init, uint32_t *val);
  \brief        gpio read function.
  \param[in]    gpio_init       gpio group and pin number.
  \param[out]   val             output value.
  \return       RET_GPIO_OK     Read successfully
*/
extern int32_t iet_gpio_read(GPIO_Init_TypeDef* gpio_init, uint32_t *val);

/**
  \fn           iet_gpio_write(GPIO_Init_TypeDef* gpio_init, uint32_t val);
  \brief        gpio write function.
  \param[in]    gpio_init       gpio group and pin number.
  \param[out]   val             input value.
  \return       RET_GPIO_OK     Write successfully
*/
extern int32_t iet_gpio_write(GPIO_Init_TypeDef* gpio_init, uint32_t val);

/**
  \fn           int32_t iet_gpio_control(GPIO_Init_TypeDef* gpio_init, uint32_t ctrl_cmd, void *param);
  \brief        gpio control function.
  \param[in]    gpio_init       gpio group and pin number.
  \param[in]    ctrl_cmd            io-control command.
  \param[in]    param               command params.
  \return       RET_GPIO_OK     Open successfully
  \return       RET_GPIO_PAR    Parameter is not valid for current control command
  \return       RET_GPIO_IO     Control command is not supported or not valid
*/
extern int32_t iet_gpio_control(GPIO_Init_TypeDef* gpio_init, uint32_t ctrl_cmd, void *param);

/** @} end of group */
#ifdef __cplusplus
}
#endif
/** @} */
#endif /* IET_GPIO_H_ */
/* --------------------------------- End Of File ------------------------------ */
