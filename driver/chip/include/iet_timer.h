/*****************************************************************************
*  Timer Controller Driver Head File for Chopin
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
*  @file     iet_timer.h
*  @brief    iet timer Controller Driver Head File
*  @author   
*  @email    
*  @version  
*  @date     
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2018/11/02 | 1.0       |                | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __IET_TIMER_H__
#define __IET_TIMER_H__

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"
/**
 * \defgroup	DEVICE_IET_TIMER	IET TIMER Driver Interface
 * \ingroup	DEVICE_IET_DEF
 * \brief	definitions for timer controller (\ref iet_timer.h)
 * \details	provide interfaces for timer driver to implement
 * @{
 *
 * \file
 * \brief	timer device definitions
 * \details	provide common definitions for timer device,
 * 	then software developer can develop timer driver
 * 	following this definitions, and the applications
 * 	can directly call this definition to realize functions
 */

/**
 * \defgroup	DEVICE_IET_TIMER_DEVCONFIG	IET TIMER Driver Config
 * \ingroup	DEVICE_IET_TIMER
 * \brief	contains definitions of timer driver config.
 * \details	this config will be used in user implemented code, which was called
 *     Device Driver Implement Layer for timer to realize in user code.
 * @{
 */

/**
 * \brief	IET TIMER driver config definition
 * @{
 */

#define TIMER_NUM   8

/** @} */

/**
 * \brief	IET TIMER number enum definition
 * \details	define TIMER_Num_TypeDef
 * @{
 */
typedef enum
{
    TIMER_NUM_0,
    TIMER_NUM_1,
    TIMER_NUM_2,
    TIMER_NUM_3,
    TIMER_NUM_4,
    TIMER_NUM_5,
    TIMER_NUM_6,
    TIMER_NUM_7,
    
}TIMER_Num_TypeDef;

/** @} */
#define IS_TIMER_NUM(NUM) (((NUM) == TIMER_NUM_0) || ((NUM) == TIMER_NUM_1) || ((NUM) == TIMER_NUM_2) || ((NUM) == TIMER_NUM_3)|| ((NUM) == TIMER_NUM_4)|| ((NUM) == TIMER_NUM_5)|| ((NUM) == TIMER_NUM_6)|| ((NUM) == TIMER_NUM_7))

#define IS_TIMER_NUM_ALL(NUM) (((NUM) == TIMER_NUM_0) || ((NUM) == TIMER_NUM_1) || ((NUM) == TIMER_NUM_2) || ((NUM) == TIMER_NUM_3)|| ((NUM) == TIMER_NUM_4)|| ((NUM) == TIMER_NUM_5)|| ((NUM) == TIMER_NUM_6)|| ((NUM) == TIMER_NUM_7))

/**
 * \brief	IET TIMER tpye enum definition
 * \details	define TIMER_Type_TypeDef
 * @{
 */
typedef enum
{
    TIMER_TYPE_TIMER,
    TIMER_TYPE_PWM,
}TIMER_Type_TypeDef;
/** @} */

#define IS_TIMER_TYPE(TYPE) (((TYPE) == TIMER_TYPE_TIMER) || ((TYPE) == TIMER_TYPE_PWM))

/**
 * \brief	IET TIMER mode enum definition
 * \details	define TIMER_Mode_TypeDef
 * @{
 */
typedef enum
{
    TIMER_FREE_RUNNING,
    TIMER_USER_DEFINED,
}TIMER_Mode_TypeDef;
	
/** @} */
#define IS_TIMER_MODE(MODE) (((MODE) == TIMER_FREE_RUNNING) || ((MODE) == TIMER_USER_DEFINED))

/**
 * \brief	IET TIMER pwm control structure definition
 * \details	define TIMER_Pwm_TypeDef
 * @{
 */
typedef struct {
    uint32_t     period;
    uint32_t     low;
} TIMER_Pwm_TypeDef;

/** @} */
/** @} */

/**
 * \defgroup	DEVICE_IET_TIMER_CTRLCMD		IET TIMER Driver Io-Control Commands
 * \ingroup	DEVICE_IET_TIMER
 * \brief	Definitions for timer driver io-control commands
 * \details	These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure or value, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref RET_TIMER_OK,		Control device successfully
 * @{
 */
/** Define TIMER control commands for common usage */
#define DEV_SET_TIMER_SYSCMD(cmd)   DEV_SET_SYSCMD(DEV_SYS_CMDBSE|(cmd))

/* ++++ Common commands for TIMER Device ++++ */
/**
 * Set TIMER interupt callback funtion.
 * - Param type : void *
 * - Param usage : void *funtion_name
 * - Return value explanation : 
 */
#define TIMER_CMD_ISR               DEV_SET_TIMER_SYSCMD(0)

/**
 * Set TIMER interupt config.
 * - Param type : FunctionalState 
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define TIMER_CMD_INT               DEV_SET_TIMER_SYSCMD(1)

/**
 * Set TIMER interupt clear.
 * - Param type : NULL
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define TIMER_CMD_INT_CLEAR         DEV_SET_TIMER_SYSCMD(2)

/**
 * Get TIMER interupt status, except extra_timer.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define TIMER_CMD_INT_STATUS        DEV_SET_TIMER_SYSCMD(3)

/**
 * Get TIMER interupt raw status, except extra_timer.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define TIMER_CMD_INT_RAW_STATUS    DEV_SET_TIMER_SYSCMD(4)

/**
 * Set TIMER mode, except extra_timer.
 * - Param type : TIMER_Mode_TypeDef
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define TIMER_CMD_MODE              DEV_SET_TIMER_SYSCMD(5)

/**
 * Set TIMER load count, units: number of clock, extra timer: time = clock * 256.
 * Only use of TIMER_TYPE.
 * - Param type : uint32_t
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define TIMER_CMD_LOAD_COUNT        DEV_SET_TIMER_SYSCMD(6)


/**
 * Get TIMER current value, except extra_timer.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define TIMER_CMD_CURRENT_VALUE     DEV_SET_TIMER_SYSCMD(8)

/**
 * Set TIMER interupt config (level polarity debounce settings and so on), except extra_timer.
 * - Param type : TIMER_Pwm_TypeDef *
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define TIMER_CMD_PWM               DEV_SET_TIMER_SYSCMD(9)
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup	DEVICE_IET_TIMER_FUN IET timer driver functions
 * \ingroup	    DEVICE_IET_TIMER
 * \brief		API functions for IET timer driver
 * @{
 */
 
/**
  \fn          	uint32_t iet_timer_open(TIMER_Num_TypeDef n_timer, TIMER_Type_TypeDef type)
  \brief		open timer
  \param[in]	n_timer         number of timer.
  \param[in]	type            run of tpye.
  \return		RET_TIMER_OK
*/
extern uint32_t iet_timer_open(TIMER_Num_TypeDef n_timer, TIMER_Type_TypeDef type);

/**
  \fn          	uint32_t iet_timer_close(TIMER_Num_TypeDef n_timer)
  \brief		close timer
  \param[in]	n_timer         number of timer.
  \return		RET_TIMER_OK
*/
extern uint32_t iet_timer_close(TIMER_Num_TypeDef n_timer);

/**
  \fn          	uint32_t iet_timer_start(TIMER_Num_TypeDef n_timer)
  \brief		start timer
  \param[in]	n_timer         number of timer.
  \return		RET_TIMER_OK
*/
extern uint32_t iet_timer_start(TIMER_Num_TypeDef n_timer);

/**
  \fn          	uint32_t iet_timer_stop(TIMER_Num_TypeDef n_timer)
  \brief		stop timer
  \param[in]	n_timer         number of timer.
  \return		RET_TIMER_OK
*/
extern uint32_t iet_timer_stop(TIMER_Num_TypeDef n_timer);

/**
  \fn          	uint32_t iet_timer_control(TIMER_Num_TypeDef n_timer, uint32_t ctrl_cmd, void *param)
  \brief		timer control function
  \param[in]	n_timer         number of timer.
  \param[in]    ctrl_cmd		io-control command.
  \param[in]   	param			command params.
  \return		RET_TIMER_OK
*/
extern uint32_t iet_timer_control(TIMER_Num_TypeDef n_timer, uint32_t ctrl_cmd, void *param);

/** @} */
#ifdef __cplusplus
}
#endif
/** @} */

#endif /* __IET_TIMER_H__ */

