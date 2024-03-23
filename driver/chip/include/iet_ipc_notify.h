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
*  @file     iet_ipc_notify.h
*  @brief    IPC notify Driver Head File
*  @author   hrwang
*  @email    hrwang@intenginetech.com
*  @version  1.0
*  @date     2019-10-08
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>         | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2019/10/12     |      1.0  |   hrwang       | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef _IET_IPC_NOTIFY_H_
#define _IET_IPC_NOTIFY_H_

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"

#define __IPC_NOTIFY_RISCV_ENABLE__
//#define __IPC_NOTIFY_HIFI_ENABLE__

typedef enum
{
    RISCV_INT_FROM_HIFI_CH0 = 0x0000,
    RISCV_INT_FROM_HIFI_CH1 = 0x0001,
    RISCV_INT_FROM_HIFI_CH2 = 0x0002,
    RISCV_INT_FROM_HIFI_CH3 = 0x0003,
    RISCV_INT_FROM_HIFI_CH4 = 0x0004,
    RISCV_INT_FROM_HIFI_CH5 = 0x0005,
} RISCV_INT_FROM_HIFI_ENUM;

// M3 IPC TX INT
typedef enum
{
    RISCV_INT_TO_HIFI_CH0   = 0x0000,
    RISCV_INT_TO_HIFI_CH1   = 0x0001,
    RISCV_INT_TO_HIFI_CH2   = 0x0002,
    RISCV_INT_TO_HIFI_CH3   = 0x0003,
    RISCV_INT_TO_HIFI_CH4   = 0x0004,
    RISCV_INT_TO_HIFI_CH5   = 0x0005,
    RISCV_INT_TO_HIFI_CH6   = 0x0006,
    RISCV_INT_TO_HIFI_CH7   = 0x0007,
} RISCV_INT_TO_HIFI_ENUM;

#ifdef __IPC_NOTIFY_RISCV_ENABLE__
#define IPC_NOTIFY_RX_MAX_NUM            (6)
#define IPC_NOTIFY_TX_MAX_NUM            (8)
#else
#define IPC_NOTIFY_RX_MAX_NUM            (6)
#define IPC_NOTIFY_TX_MAX_NUM            (5)
#endif

#define IS_IPC_NOTIFY_RX_NUM(NUM)        (((NUM) >= 0) && ((NUM) < IPC_NOTIFY_RX_MAX_NUM))
#define IS_IPC_NOTIFY_TX_NUM(NUM)        (((NUM) >= 0) && ((NUM) < IPC_NOTIFY_TX_MAX_NUM))


typedef void  (*IPC_NOTIFY_Callback_Func) (void* p);

/* IPC_NOTIFY_Func_TypeDef */
typedef struct
{
    int32_t (*int_enable)(uint16_t remote_proc,uint16_t vector);
    int32_t (*int_disable)(uint16_t remote_proc,uint16_t vector);
    int32_t (*int_register)(uint16_t remote_proc,uint16_t vector,IPC_NOTIFY_Callback_Func func,void* para);
    int32_t (*int_unregister)(uint16_t remote_proc,uint16_t vector);
    int32_t (*int_clear)(uint16_t remote_proc,uint16_t vector);
    int32_t (*int_send)(uint16_t remote_proc,uint16_t vector);
} IPC_NOTIFY_Func_TypeDef;

/* IPC_Init_TypeDef */
typedef struct
{
    IPC_NOTIFY_Func_TypeDef int_func;
} IPC_NOTIFY_Init_TypeDef;

/* IPC_Init_TypeDef */
typedef struct
{
    uint16_t local_proc;
    uint16_t remote_proc;
    uint16_t tx_chn;
    uint16_t rx_chn;

    IPC_NOTIFY_Callback_Func func;
} IPC_NOTIFY_TypeDef;

/*
 *************************************************************************
 *                      Module functions
 *************************************************************************
 */

/**
  \fn           int32_t iet_ipc_notify_init(IPC_NOTIFY_Init_TypeDef* ipc_notify_init);
  \brief        ipc notify init function.
  \param[in]    ipc_notify_init    ipc notify init handle
  \return       RET_IPC_NOTIFY_OK  successfully
*/
extern int32_t iet_ipc_notify_init(IPC_NOTIFY_Init_TypeDef* ipc_notify_init);

/**
  \fn           int32_t iet_ipc_notify_deinit(void);
  \brief        ipc notify deinit function.
  \param[in]    void
  \return       RET_IPC_NOTIFY_OK  successfully
*/
extern int32_t iet_ipc_notify_deinit(void);


/**
  \fn           int32_t iet_ipc_notify_open(IPC_NOTIFY_TypeDef* ipc_notify);
  \brief        ipc notify open function.
  \param[in]    ipc_notify        ipc notify handle
  \return       RET_IPC_NOTIFY_OK successfully
*/
extern int32_t iet_ipc_notify_open(IPC_NOTIFY_TypeDef* ipc_notify);

/**
  \fn           int32_t iet_ipc_notify_close(IPC_NOTIFY_TypeDef* ipc_notify);
  \brief        ipc notify close function.
  \param[in]    ipc_notify             ipc notify handle
  \return       RET_IPC_NOTIFY_OK      successfully
*/
extern int32_t iet_ipc_notify_close(IPC_NOTIFY_TypeDef* ipc_notify);

/**
  \fn           int32_t iet_ipc_notify_enable(IPC_INT_TypeDef* ipc_notify);
  \brief        ipc notify enable function.
  \param[in]    ipc_notify             ipc notify handle
  \return       RET_IPC_NOTIFY_OK      successfully
*/
extern int32_t iet_ipc_notify_enable(IPC_NOTIFY_TypeDef* ipc_notify);


/**
  \fn           int32_t int32_t iet_ipc_notify_disable(IPC_NOTIFY_TypeDef* ipc_notify);
  \brief        ipc notify disable function.
  \param[in]    ipc_notify             ipc notify handle
  \return       RET_IPC_NOTIFY_OK      successfully
*/
extern int32_t iet_ipc_notify_disable(IPC_NOTIFY_TypeDef* ipc_notify);

/**
  \fn           int32_t int32_t iet_ipc_notify_clear(IPC_NOTIFY_TypeDef* ipc_notify);
  \brief        ipc notify clear function.
  \param[in]    ipc_notify             ipc notify handle
  \return       RET_IPC_NOTIFY_OK      successfully
*/
extern int32_t iet_ipc_notify_clear(IPC_NOTIFY_TypeDef* ipc_notify);

/**
  \fn           int32_t iet_ipc_notify_register(IPC_NOTIFY_TypeDef* ipc_notify);
  \brief        ipc notify register function.
  \param[in]    ipc_notify             ipc notify handle
  \return       RET_IPC_NOTIFY_OK      successfully
*/
extern int32_t iet_ipc_notify_register(IPC_NOTIFY_TypeDef* ipc_notify,IPC_NOTIFY_Callback_Func func,void* para);


/**
  \fn           int32_t iet_ipc_notify_unregister(IPC_NOTIFY_TypeDef* ipc_notify);
  \brief        ipc notify unregister function.
  \param[in]    ipc_notify             ipc notify handle
  \return       RET_IPC_NOTIFY_OK      successfully
*/
extern int32_t iet_ipc_notify_unregister(IPC_NOTIFY_TypeDef* ipc_notify);


/**
  \fn          int32_t iet_ipc_notify_send(IPC_NOTIFY_TypeDef* ipc_notify);
  \brief       ipc notify send function.
  \param[in]   ipc_notify             ipc notify handle
  \return      RET_IPC_NOTIFY_OK      successfully
*/
extern int32_t iet_ipc_notify_send(IPC_NOTIFY_TypeDef* ipc_int);


#ifdef __cplusplus
extern "C" {
#endif


/** @} end of group */
#ifdef __cplusplus
}
#endif
/** @} */
#endif /* IET_IPC_NOTIFY_H_ */

/* --------------------------------- End Of File ------------------------------ */


