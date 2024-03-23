/*****************************************************************************
*  Heterogeneous-Multi-Core IPC Share Memory Driver Head File for Bach.
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
*  @file      iet_ipc_share_mem.h
*  @brief     IPC Share Memory Driver Head File
*  @author    hrwang
*  @email     hrwang@intenginetech.com
*  @version   1.0
*  @date      2019-10-12
*  @license   GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2019/10/12 | 1.0       |  hrwang        | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/
#ifndef  IET_IPC_SHARE_MEM_H_
#define  IET_IPC_SHARE_MEM_H_

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"

/*define*/
#define IPC_SHARE_MEM_TABLE_ADDR         (uint32_t*)0x00080200   //size:32 * 4 * 2 bytes

#define IPC_SHARE_MEM_NUM                (32)
#define IS_IPC_SHARE_MEM_NUM(NUM)        (((NUM) >= 0) && ((NUM) < IPC_SHARE_MEM_NUM))


/* IPC_MEM_Func_TypeDef */
typedef struct
{
    void* (*mem_alloc)(uint32_t size);
    void (*mem_free)(void* p);
} IPC_MEM_Func_TypeDef;


typedef struct
{
    IPC_MEM_Func_TypeDef   mem_func;

} IPC_SHARE_MEM_Init_TypeDef;


/* IPC_Init_TypeDef */
typedef struct
{
    uint32_t  addr;
    uint32_t  size;
} IPC_SHARE_MEM_TypeDef;



/**
  \fn           int32_t iet_ipc_share_mem_init(IPC_SHARE_MEM_Init_TypeDef* share_mem_init)
  \brief        ipc share mem init function.
  \param[in]    share_mem_init  mem init handle
  \return       RET_IPC_SHARE_MEM_OK      init successfully
*/
extern int32_t  iet_ipc_share_mem_init(IPC_SHARE_MEM_Init_TypeDef* share_mem_init);

/**
  \fn           int32_t iet_ipc_share_mem_deinit(void);
  \brief        ipc share mem deinit function.
  \param[in]    void
  \return       RET_IPC_SHARE_MEM_OK      deinit successfully
*/
extern int32_t  iet_ipc_share_mem_deinit(void);


/**
  \fn           int32_t iet_ipc_share_mem_open(uint32_t index);
  \brief        ipc share mem open function.
  \param[in]    index                     share mem index
  \return       RET_IPC_SHARE_MEM_OK      open successfully
  \return       RET_IPC_SHARE_MEM_OPEND   already opened
*/
extern int32_t  iet_ipc_share_mem_open(uint32_t index);

/**
  \fn           int32_t iet_ipc_share_mem_close(uint32_t index);
  \brief        ipc mem free function.
  \param[in]    p                         share mem pointer
  \return       RET_IPC_SHARE_MEM_OK      successfully
*/
extern int32_t  iet_ipc_share_mem_close(uint32_t index);

/**
  \fn           int32_t iet_ipc_share_mem_cfg_set(uint32_t index,IPC_SHARE_MEM_TypeDef* share_mem);
  \brief        set ipc share mem cfg function.
  \param[in]    index                     share mem index
  \param[in]    share_mem                 share mem pointer
  \return       RET_IPC_SHARE_MEM_OK      successfully
*/
extern uint32_t iet_ipc_share_mem_cfg_set(uint32_t index,IPC_SHARE_MEM_TypeDef* share_mem);

/**
  \fn           int32_t iet_ipc_share_mem_cfg_get(uint32_t index,IPC_SHARE_MEM_TypeDef** p);
  \brief        get ipc share mem config function.
  \param[in]    index                     share mem index
  \param[in]    p                         pointer to share mem pointer
  \return       RET_IPC_SHARE_MEM_OK      successfully
*/
extern uint32_t iet_ipc_share_mem_cfg_get(uint32_t index,IPC_SHARE_MEM_TypeDef** p);

#ifdef __cplusplus
extern "C" {
#endif


/** @} end of group */
#ifdef __cplusplus
}
#endif
/** @} */
#endif /* IET_IPC_SHARE_MEM_H_ */

/* --------------------------------- End Of File ------------------------------ */

