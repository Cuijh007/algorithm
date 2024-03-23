/******************************************************************************
*  Middle ware common head File for Chopin.
*
*  Copyright (C) 2019-2020 IET Limited. All rights reserved.
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
*  @file     mid_common.h
*  @brief    ASR Post Process Module Head File
*  @author   Zhiyun.Ling
*  @email    Zhiyun.Ling@intenginetech.com
*  @version  1.0
*  @date     2019-12-17
*  @license  GNU General Public License (GPL)
*
*------------------------------------------------------------------------------
*  Remark         : Description
*------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*------------------------------------------------------------------------------
*  2019/12/17 | 1.0       | Zhiyun.Ling    | Create file
*
*******************************************************************************/

#ifndef __MID_COMMON_H__
#define __MID_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************* Include *************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "iet_common.h"

/**
 * \defgroup    MID_COMMON   MID COMMON Interface
 * \ingroup IET_MIDDLEWARE
 * \brief   definitions for Common Interface (\ref mid_common.h)
 * \details provide interfaces for middle ware common interface to implement
 * @{
 *
 * \file
 * \brief   common interface definitions
 * \details provide common definitions for common interface,
 *  then software developer can develop common interface
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */

/**
 * \defgroup    MID_COMMON_ERROR_DEFINITION  Common Error Definition
 * \ingroup MID_COMMON
 * \brief   definitions for Common Error Definitions
 * \details these error definitions will be used in user and system implemented code, which was called
 *     Device Driver Implement Layer for return value to realize in user code.
 * @{
 */
/**
 * \name    Main Error Code Definitions
 * @{
 */
#define E_WEIGHT    (0x242)      /*!< weight error                 */
#define E_SKIP      (0x243)      /*!< skip post-processing operations*/
/** @} */

#define DEV_MID_CMDBSE          (0x60000000)            /*!< default middleware control command base    */
#define DEV_SET_MIDCMD(cmd)     (DEV_MID_CMDBSE|(cmd))  /*!< set middleware control command             */

#define RET_SYS_BASE            (0x00000000)
#define RET_USR_BASE            (0x80000000)
#define MID_NPU_BASE            (0x00000200)
#define MID_PP_BASE             (0x00000300)
#define MID_SRC_BASE            (0x00000400)
#define MID_MEM_BASE            (0x00000500)
#define MID_FEAUTRE_BASE        (0x00000600)

/**
 * \name    MID NPU control Error Code Definitions
 * @{
 */
#define MID_NPU_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define MID_NPU_SKIP            DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_SKIP)
#define MID_NPU_ERR             DEV_SET_MIDCMD(MID_NPU_BASE|E_ERR)
#define MID_NPU_PAR             DEV_SET_MIDCMD(MID_NPU_BASE|E_PAR)
#define MID_NPU_OPEND           DEV_SET_MIDCMD(MID_NPU_BASE|E_OPNED)
#define MID_NPU_CLSED           DEV_SET_MIDCMD(MID_NPU_BASE|E_CLSED)
#define MID_NPU_WEIGHT          DEV_SET_MIDCMD(MID_NPU_BASE|E_WEIGHT)
/** @} */

/**
 * \name    MID post process Error Code Definitions
 * @{
 */
#define MID_PP_OK               DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define MID_PP_ERR              DEV_SET_MIDCMD(MID_PP_BASE|E_ERR)
#define MID_PP_PAR              DEV_SET_MIDCMD(MID_PP_BASE|E_PAR)
#define MID_PP_READY            DEV_SET_MIDCMD(MID_PP_BASE|E_READY)
/** @} */

/**
 * \name    MID feature process Error Code Definitions
 * @{
 */
#define MID_FEATURE_OK          DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define MID_FEATURE_ERR         DEV_SET_MIDCMD(MID_FEAUTRE_BASE|E_ERR)
#define MID_FEATURE_PAR         DEV_SET_MIDCMD(MID_FEAUTRE_BASE|E_PAR)
#define MID_FEATURE_READY       DEV_SET_MIDCMD(MID_FEAUTRE_BASE|E_READY)
/** @} */


/**
 * \name    MID source Error Code Definitions
 * @{
 */
#define MID_SOURCE_OK           DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define MID_SOURCE_ERR          DEV_SET_MIDCMD(MID_SRC_BASE|E_ERR)
#define MID_SOURCE_PAR          DEV_SET_MIDCMD(MID_SRC_BASE|E_PAR)
/** @} */

/**
 * \name    MID memory Error Code Definitions
 * @{
 */
#define MID_MEMORY_OK           DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define MID_MEMORY_ERR          DEV_SET_MIDCMD(MID_MEM_BASE|E_ERR)
#define MID_MEMORY_PAR          DEV_SET_MIDCMD(MID_MEM_BASE|E_PAR)
#define MID_MEMORY_QOVR         DEV_SET_MIDCMD(MID_MEM_BASE|E_QOVR)
#define MID_MEMORY_NORES        DEV_SET_MIDCMD(MID_MEM_BASE|E_NORES)
#define MID_MEMORY_NOMEM        DEV_SET_MIDCMD(MID_MEM_BASE|E_NOMEM)
/** @} */
/** @} */

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif


#ifdef __cplusplus
}
#endif
/** @} */

#endif /* __MID_COMMON_H__ */

/* --------------------------------- End Of File ------------------------------ */
