/*****************************************************************************
*  Systick Controller Driver Head File for BACH2C.
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
*  @file     iet_systick.h
*  @brief    iet systick Controller Driver Head File
*  @author   
*  @email    
*  @version  1.0
*  @date     2018-11-02
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2019/02/11 | 1.1       |                | 
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __SYSTICK_BACH__
#define __SYSTICK_BACH__

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"

/**
 * \defgroup	DEVICE_IET_SYSTICK	IET SYSTICK Driver Interface
 * \ingroup	DEVICE_IET_DEF
 * \brief	definitions for systick controller (\ref iet_systick.h)
 * \details	provide interfaces for systick driver to implement
 * @{
 *
 * \file
 * \brief	systick device definitions
 * \details	provide common definitions for systick device,
 * 	then software developer can develop systick driver
 * 	following this definitions, and the applications
 * 	can directly call this definition to realize functions
 */

/**
 * \defgroup	DEVICE_IET_SYSTICK_DEVCONFIG	IET SYSTICK Driver Config
 * \ingroup	DEVICE_IET_SYSTICK
 * \brief	contains definitions of timer systick config.
 * \details	this config will be used in user implemented code, which was called
 *     Device Driver Implement Layer for systick to realize in user code.
 * @{
 */

/**
 * \brief	IET SYSTICK driver config definition
 * @{
 */
#define SYSTICK_TIME_TEST    1

#if SYSTICK_TIME_TEST
extern volatile uint32_t SysTickCnt_test;
#endif
/** @} */
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup	DEVICE_IET_SYSTICK_FUN IET systick driver functions
 * \ingroup	DEVICE_IET_SYSTICK
 * \brief	API functions for IET systick driver
 * @{
 */

/**
  \fn          	void iet_systick_delayms(uint32_t tick)
  \brief	delay function
  \param[in]	tick  delay ms
  \return	NULL
*/
extern void iet_systick_delayms(uint32_t tick);

/**
  \fn          	void systick_init(void)
  \brief	systick init
  \param[in]	NULL
  \return	NULL
*/
extern void iet_systick_init(void);
extern void TimingDelay_Decrement(void);


uint32_t iet_sw_timer_start(uint32_t* t);
uint32_t iet_sw_timer_read(uint32_t* t);
uint32_t iet_sw_timer_stop(uint32_t* t);



/** @} */
#ifdef __cplusplus
}
#endif
/** @} */

#endif /* __IET_SYSTICK_H__ */

