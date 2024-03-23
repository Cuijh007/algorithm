/******************************************************************************
*  MID Memory Module Head File for Bach.
*
*  Copyright (C) 2020-2021 IET Limited. All rights reserved.
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
*  @file     mid_mem.h
*  @brief    MID memory Module Head File
*  @author   Zhiyun.Ling
*  @email    Zhiyun.Ling@intenginetech.com
*  @version  1.0
*  @date     2020-06-08
*  @license  GNU General Public License (GPL)
*
*------------------------------------------------------------------------------
*  Remark         : Description
*------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*------------------------------------------------------------------------------
*  2020/06/08 | 1.0       | Zhiyun.Ling    | Create file
*
******************************************************************************/

#ifndef __MID_MEMORY_H__
#define __MID_MEMORY_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************* Include *************************************/

/**
 * \defgroup    MID_MEMORY    MID memory module
 * \ingroup IET_MIDDLEWARE
 * \brief   definitions for memory API (\ref mid_mem.h)
 * \details provide interfaces for memory to implement
 * @{
 *
 * \file
 * \brief   memory module definitions
 * \details provide common definitions for memory module,
 *  then software developer can develop memory
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */

/**
 * \defgroup    MID_MEMORY_DEFINE  memory module definitions
 * \ingroup MID_MEMORY
 * \brief   contains definitions of memory module.
 * \details this definitions will be used in user implemented code.
 * @{
 */

/** @} */


/**
 * \defgroup    MID_MEMORY_FUNCTION memory functions
 * \ingroup     MID_MEMORY
 * \brief       API functions for memory module
 * @{
 */

/**
  \fn           void *mid_mem_malloc(uint32_t mem_size);
  \brief        Allocate memory buffer
  \param[in]    mem_size    the size of memory buffer to allocation
  \return       !NULL   the pointer of memory buffer
*/
extern void    *mid_mem_malloc(uint32_t mem_size);

/**
  \fn           void mid_mem_free(void *mem_buffer)
  \brief        Free the memory buffer
  \param[in]    mem_buffer  pointer to the memory buffer
  \return       None
*/
extern void     mid_mem_free(void *mem_buffer);
/** @} */

#ifdef __cplusplus
}
#endif
/** @} */

#endif /* __MID_MEMORY_H__ */
