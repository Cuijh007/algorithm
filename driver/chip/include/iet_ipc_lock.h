/*****************************************************************************
*  Heterogeneous Multi Core IPC Lock Controller Driver Head File for Bach.
*
*  Copyright (C) 2019  www.intenginetech.com.
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
*  @file     iet_ipc_lock.h
*  @brief    IPC lock controller driver Head File
*  @author   hrwang
*  @email    hrwang@intenginetech.com
*  @version  1.0
*  @date     2019-10-16
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2019/10/16 | 1.0       | hrwang         | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/
#ifndef IET_IPC_LOCK_H_
#define IET_IPC_LOCK_H_

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"
/**
 * \defgroup DEVICE_IET_IPC_LOCK    IET IPC LOCK Device Driver Interface
 * \ingroup  DEVICE_IET_DEF
 * \brief    definitions for ipc lock driver Interface (\ref iet_ipc_lock.h)
 * \details  provide interfaces for ipc lock driver to implement
 * @{
 *
 * \file
 * \brief    ipc lock device definitions
 * \details  provide common definitions for ipc lock device,
 *  then software developer can develop ipc lock driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */

/**
 * \defgroup DEVICE_IET_IPC_LOCK_DEVSTRUCT  IET IPC Lock Driver Config Structure
 * \ingroup  DEVICE_IET_IPC_LOCK
 * \brief    contains definitions for ipc lock driver config structure.
 * \details this structure will be used in user implemented code, which was called
 *     Device Driver Implement Layer for ipc lock to realize in user code.
 * @{
 */
/**
 * \brief   IET IPC LOCK config definition
 * @{
 */
/**
 * \brief   IET IPC LOCK num definition
 * @{
 */
#define IPC_LOCK_REG_BASE_ADDR       (0x12000000UL)
#define IPC_LOCK_REG_CONFIG_ADDR     (IPC_LOCK_REG_BASE_ADDR + 0x0000)
#define IPC_LOCK_REG_STATUS_ADDR     (IPC_LOCK_REG_BASE_ADDR + 0x1000)


#define IPC_LOCK_NUM                (64)
#define IS_IPC_LOCK_NUM(NUM)        (((NUM) >= 0) && ((NUM) < IPC_LOCK_NUM))
/** @} */
/**
 * \brief   IET IPC Lock read return value
 * @{
 */
#define IPC_LOCKED_NONE              (0)
#define IPC_LOCKED_BY_RISCV          (1)
#define IPC_LOCKED_BY_HIFI           (4)
#define IS_VALID_LOCK_VAL(NUM)       (((NUM) == IPC_LOCKED_BY_RISCV) || ((NUM) == IPC_LOCKED_BY_HIFI))
/** @} */



/**
 * \brief   IET IPC Lock structure definition
 * \details define IPC_Init_TypeDef
 * @{
 */
typedef struct
{
    uint8_t     id;                         /*!< ipc lock number */
} IPC_LOCK_TypeDef;



/** @} */
/** @} */

/**
 * \defgroup   DEVICE_IET_IPC_CTRLCMD       IET IPC Lock Driver Io-Control Commands
 * \ingroup    DEVICE_IET_IPC_LOCK
 * \brief      Definitions for ipc lock driver io-control commands
 * \details    These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref RET_IPC_LOCK_OK,        Control device successfully
 * @{
 */
/** Define IPC Lock control commands for common usage */
#define DEV_IPC_LOCK_SYSCMD(cmd)        DEV_SET_SYSCMD(DEV_SYS_CMDBSE|(cmd))
/* ++++ Common commands for IPC LOCK Device ++++ */
/** @} */

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \defgroup    DEVICE_IET_IPC_LOCK_FUN IPC Lock Driver Functions
 * \ingroup     DEVICE_IET_IPC_LOCK
 * \brief       Definitions for IPC Lock Driver API functions
 * @{
 */

/**
  \fn int32_t   iet_ipc_lock_open(IPC_LOCK_TypeDef* ipc_lock);
  \brief        ipc lock open function.
  \param[in]    ipc_lock                ipc lock handle.
  \return       RET_IPC_LOCK_OK         lock successfully
  \return       RET_IPC_LOCK_ACQUIRE    other core lock
*/
extern int32_t  iet_ipc_lock_open(IPC_LOCK_TypeDef* ipc_lock);

/**
  \fn int32_t   iet_ipc_lock_close(IPC_LOCK_TypeDef* ipc_lock);
  \brief        ipc lock close function.
  \param[in]    ipc_lock                ipc lock handle.
  \return       RET_IPC_LOCK_OK         lock successfully
  \return       RET_IPC_LOCK_ACQUIRE    other core lock
*/
extern int32_t  iet_ipc_lock_close(IPC_LOCK_TypeDef* ipc_lock);


/**
  \fn int32_t   iet_ipc_lock_acquire(IPC_LOCK_TypeDef* ipc_lock);
  \brief        ipc lock acquire function.
  \param[in]    ipc_lock                ipc lock handle.
  \return       RET_IPC_LOCK_OK         lock successfully
  \return       RET_IPC_LOCK_ACQUIRE    other core lock
*/
extern int32_t  iet_ipc_lock_acquire(IPC_LOCK_TypeDef* ipc_lock);

/**
  \fn int32_t   iet_ipc_lock_release(IPC_LOCK_TypeDef* ipc_lock);
  \brief        ipc lock release function.
  \param[in]    ipc_lock                ipc lock handle.
  \return       RET_IPC_LOCK_OK         release successfully
  \return       RET_IPC_LOCK_RELEASE    release failed
*/
extern int32_t  iet_ipc_lock_release(IPC_LOCK_TypeDef* ipc_lock);

/**
  \fn int32_t   iet_ipc_lock_check(IPC_LOCK_Config_TypeDef* ipc_lock);
  \brief        ipc lock check function.
  \param[in]    ipc_lock                      ipc lock handle.
  \return       PROC_RISCV/PROC_HIFIA/PROC_HIFIB processor id locked by
  \return       PROC_NONE                     not locked by processor
*/
extern int32_t  iet_ipc_lock_check(IPC_LOCK_TypeDef* ipc_lock);

/** @} end of group */
#ifdef __cplusplus
}RISCV
#endif
/** @} */
#endif /* IET_IPC_LOCK_H_ */


/* --------------------------------- End Of File ------------------------------ */

