/*****************************************************************************
*  HIFI Controller Driver Head File for Bach.
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
*  @file     iet_hifi.h
*  @brief    HIFI Controller Driver Head File
*  @author
*  @email
*  @version  1.0
*  @date
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>       | <Version> | <Author>         | <Description>
*----------------------------------------------------------------------------
*  2019/10/16 | 1.0         |                  | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef IET_HIFI_H_
#define IET_HIFI_H_

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"
/**
 * \defgroup DEVICE_IET_HIFI    IET HIFI Device Driver Interface
 * \ingroup  DEVICE_IET_DEF
 * \brief    definitions for HIFI driver Interface (\ref iet_hifi.h)
 * \details  provide interfaces for HIFI driver to implement
 * @{
 *
 * \file
 * \brief   HIFI device definitions
 * \details provide common definitions for HIFI device,
 *  then software developer can develop HIFI driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */

#define HIFI_REG_BASE_ADDR          (0x51B00000)
#define HIFI_A_REG_ADDR             (HIFI_REG_BASE_ADDR + 0x700)
#define HIFI_B_REG_ADDR             (HIFI_REG_BASE_ADDR + 0x760)


/**
 * \defgroup    DEVICE_IET_HIFI_DEVSTRUCT   IET HIFI Driver Config Structure
 * \ingroup DEVICE_IET_HIFI
 * \brief   contains definitions for HIFI driver config structure.
 * \details this structure will be used in user implemented code, which was called
 *     Device Driver Implement Layer for HIFI to realize in user code.
 * @{
 */
/**
 * \brief   IET HIFI group and config definition
 * @{
 */
/**
 * \brief   IET HIFI GROUP and PIN definition
 * @{
 */
#define HIFI_GROUP_NUM          (2)

typedef enum
{
    IET_HIFI_GROUP_A        = 0,
    IET_HIFI_GROUP_B        = 1,
} HIFI_GROUP;

#define IS_HIFI_GROUP(VAL) ((VAL == IET_HIFI_GROUP_A) || (VAL == IET_HIFI_GROUP_B))

typedef enum
{
    FW_SOURCE_SRAM =0,
    FW_SOURCE_FLASH =1,
} HIFI_FW_SOURCE;

typedef struct
{
    uint32_t  config;                   /* Offset: 0x000 (R/W)  hifi4 config Register */
    uint32_t  pfault_info;              /* Offset: 0x004 (R/W)  hifi4 PFaultInfo value Register */
    uint32_t  reset_vec;                /* Offset: 0x008 (R/W)  hifi4 resetvec address Register */
    uint8_t   cm3_int;                  /* Offset: 0x00c (R/W)  hifi4 Interrupt set Register */
    uint32_t  trace[12];                /* Offset: 0x010 - 0x3c (R/W) Register */
    uint8_t   clr_int;                  /* Offset: 0x040 (R/W)  hifi4 Interrupt clear Register */
} HIFI_Reg_TypeDef;



/**
 * \brief   IET HIFI Init structure definition
 * \details define HIFI_Init_TypeDef
 * @{
 */
typedef struct
{
    uint32_t     group;                   /*!< hifi group number */
    uint32_t     base_vec;                /*!< hifi base vector */
    uint32_t     reset_vec;               /*!< hifi reset vector */
    HIFI_Reg_TypeDef* reg;                /*!< hifi config */
} HIFI_TypeDef;
/** @} */



/** @} */

/**
 * \defgroup    DEVICE_IET_HIFI_CTRLCMD     IET HIFI Driver Io-Control Commands
 * \ingroup DEVICE_IET_HIFI
 * \brief   Definitions for HIFI driver io-control commands
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref RET_HIFI_OK,        Control device successfully
 * @{
 */
/** Define HIFI control commands for common usage */
#define DEV_HIFI_SYSCMD(cmd)        DEV_SET_SYSCMD(DEV_SYS_CMDBSE|(cmd))

/* ++++ Common commands for HIFI Device ++++ */

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \defgroup    DEVICE_IET_HIFI_FUN  Driver Functions
 * \ingroup     DEVICE_IET_HIFI
 * \brief       Definitions for HIFI Driver API functions
 * @{
 */
/**
  \fn int32_t   iet_hifi_open(HIFI_TypeDef* hifi)
  \brief        HIFI open function.
  \param[in]    hifi                 HIFI handle.
  \return       RET_HIFI_OK          open successfully
  \return       RET_HIFI_OPEND       If device was opened before then just return RET_HIFI_OPEND
*/
extern int32_t  iet_hifi_open(HIFI_TypeDef* hifi);

/**
  \fn int32_t   iet_hifi_close(HIFI_TypeDef* hifi);
  \brief        HIFI close function.
  \param[in]    hifi                 HIFI handle
  \return       RET_HIFI_OK          close successfully
  \return       RET_HIFI_CLSED       close failed
*/
extern int32_t  iet_hifi_close(HIFI_TypeDef* hifi);


/**
  \fn int32_t   iet_hifi_reset_vec_cfg(HIFI_TypeDef* hifi, uint32_t addr);
  \brief        HIFI vector modify function.
  \param[in]    hifi                HIFI handle.
  \param[in]    uint32_t            vetcor address
  \return       RET_HIFI_OK         successfully
*/
extern int32_t  iet_hifi_reset_vec_cfg(HIFI_TypeDef* hifi, uint32_t addr);


/**
  \fn int32_t   iet_hifi_fw_load(HIFI_TypeDef* hifi,uint32_t source, uint8_t* code,uint32_t len);
  \brief        HIFI load firmware function.
  \param[in]    hifi                HIFI handle.
  \param[in]    uint32_t            FW_SOURCE_SRAM or FW_SOURCE_FLASH
  \param[in]    uint8_t*            firmware data
  \param[in]    uint32_t            firware length
  \return       RET_HIFI_OK         successfully
*/
extern int32_t iet_hifi_fw_load(HIFI_TypeDef* hifi,uint32_t source, uint8_t* code,uint32_t len);

/**
  \fn int32_t   iet_hifi_reset(HIFI_TypeDef* hifi);
  \brief        HIFI reset function.
  \param[in]    hifi                HIFI handle.
  \return       RET_HIFI_OK         successfully
*/
extern int32_t  iet_hifi_reset(HIFI_TypeDef* hifi);

/**
  \fn int32_t   iet_hifi_halt(HIFI_TypeDef* hifi);
  \brief        HIFI reset function.
  \param[in]    hifi                HIFI handle.
  \return       RET_HIFI_OK         successfully
*/
extern int32_t  iet_hifi_halt(HIFI_TypeDef* hifi);


/**
  \fn int32_t   iet_hifi_resume(HIFI_TypeDef* hifi);
  \brief        HIFI reset function.
  \param[in]    hifi                HIFI handle.
  \return       RET_HIFI_OK         successfully
*/
extern int32_t  iet_hifi_resume(HIFI_TypeDef* hifi);



/** @} end of group */
#ifdef __cplusplus
}
#endif
/** @} */
#endif /* IET_HIFI_H_ */
/* --------------------------------- End Of File ------------------------------ */
