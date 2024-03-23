/*****************************************************************************
*  Heterogeneous-Multi-Core IPC Interupt Driver Head File for Bach.
*
*  Copyright (C) 2019, www.intenginetech.com.
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
*  @file      iet_ipc_int_riscv.h
*  @brief     IPC INT riscv Driver Head File
*  @author    hrwang
*  @email     hrwang@intenginetech.com
*  @version   1.0
*  @date      2019-04-03
*  @license   GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2019/10/12 | 1.0       | hrwang         | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/
#ifndef  IET_IPC_INT_RISCV_H_
#define  IET_IPC_INT_RISCV_H_

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"


#define IPC_INT_TO_HIFI_REG_ADDR       (BACH_SYS_BASE + 0x700 + 0x000C)
#define IPC_INT_CLR_HIFI_REG_ADDR      (BACH_SYS_BASE + 0x700 + 0x0040)
#define IPC_INT_VECTOR_RISCV_OFFSET    (7)

/* type defines -------------------------------------------------------------- */
typedef void(*IPC_INT_RISCV_Callback_Func)(void*p);


/**
  \fn         void iet_ipc_riscv_int_enable(uint16_t remote_proc,uint16_t vector)
  \brief      ipc riscv int enable function.
  \param[in]  remote_proc        PROC_HIFIA/PROC_HIFIB
  \param[in]  rx_chn             0~5
  \return     RET_IPC_NOTIFY_OK  successfully
*/

int32_t iet_ipc_riscv_int_enable(uint16_t remote_proc,uint16_t rx_chn);
/**
  \fn         void iet_ipc_riscv_int_disable(uint16_t remote_proc,uint16_t rx_chn)
  \brief      ipc int disable function.
  \param[in]  remote_proc        PROC_HIFIA/PROC_HIFIB
  \param[in]  rx_chn             0~5
  \return     RET_IPC_NOTIFY_OK  successfully
*/
int32_t iet_ipc_riscv_int_disable(uint16_t remote_proc,uint16_t rx_chn);

/**
  \fn         void iet_ipc_riscv_int_register(uint16_t remote_proc,uint16_t rx_chn,IPC_INT_RISCV_Callback_Func* func,void* para)
  \brief      ipc riscv int register function
  \param[in]  remote_proc       PROC_HIFIA/PROC_HIFIB
  \param[in]  rx_chn            0~5
  \param[in]  func              ipc int callback func
  \param[in]  para              ipc int callback para
  \return     RET_IPC_NOTIFY_OK successfully
*/
int32_t iet_ipc_riscv_int_register(uint16_t remote_proc,uint16_t rx_chn,IPC_INT_RISCV_Callback_Func* func,void* para);

/**
  \fn         void iet_ipc_riscv_int_unregister(uint16_t remote_proc,uint16_t rx_chn)
  \brief      ipc riscv int unregister function
  \param[in]  remote_proc       PROC_HIFIA/PROC_HIFIB
  \param[in]  rx_chn            0~5
  \return     RET_IPC_NOTIFY_OK successfully
*/
int32_t iet_ipc_riscv_int_unregister(uint16_t remote_proc,uint16_t rx_chn);

/**
  \fn         void iet_ipc_riscv_int_clear(uint16_t remote_proc,uint16_t rx_chn)
  \brief      ipc riscv int clear function
  \param[in]  remote_proc       PROC_HIFIA/PROC_HIFIB
  \param[in]  rx_chn            0~5
  \return     RET_IPC_NOTIFY_OK successfully
*/
int32_t iet_ipc_riscv_int_clear(uint16_t remote_proc,uint16_t rx_chn);

/**
  \fn         void iet_ipc_riscv_int_send(uint16_t remote_proc,uint16_t tx_chn)
  \brief      ipc riscv int send function
  \param[in]  remote_proc       PROC_HIFIA/PROC_HIFIB
  \param[in]  rx_chn            0~7
  \return     RET_IPC_NOTIFY_OK successfully
*/
int32_t iet_ipc_riscv_int_send(uint16_t remote_proc,uint16_t tx_chn);

#ifdef __cplusplus
extern "C" {
#endif


/** @} end of group */
#ifdef __cplusplus
}
#endif
/** @} */
#endif /* IET_IPC_INT_RISCV_H_ */

/* --------------------------------- End Of File ------------------------------ */


