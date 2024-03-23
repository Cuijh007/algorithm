/*****************************************************************************
*  VPU Controller Driver Head File for Bach.
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
*  @file     iet_vpu.h
*  @brief    iet vpu Controller Driver Head File
*  @author   yang.fan
*  @email    yang.fan@intenginetech.com
*  @version  1.0
*  @date     2019-05-17
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __VPU_BACH_H__
#define __VPU_BACH_H__

#include "iet_common.h"
typedef enum
{
    VPU_INT,
    VPU_POLL,
} VPU_Mode_TypeDef;

#define IS_VPU_MODE(MODE) (((MODE) == VPU_INT)  || ((MODE) == VPU_POLL))

#define DEV_SET_VPU_SYSCMD(cmd)     DEV_SET_SYSCMD(DEV_SYS_CMDBSE|(cmd))

#define VPU_CMD_ISR                 DEV_SET_VPU_SYSCMD(0)
#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t iet_vpu_init(VPU_Mode_TypeDef mode);
extern uint32_t iet_vpu_start(void);
extern uint32_t iet_vpu_stop(void);
extern uint32_t iet_vpu_if_done(void);
extern uint32_t iet_vpu_control(uint32_t ctrl_cmd, void *param);

#ifdef __cplusplus
}
#endif

#endif /* __IET_VPU_H__ */

