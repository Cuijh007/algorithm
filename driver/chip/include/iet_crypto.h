/*****************************************************************************
*  Crypto Driver Head File for BACH2C
*
*  Copyright (C) 2018-2020 IET Limited. All rights reserved.
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
*  @file     iet_crypto.h
*  @brief    iet Crypto Driver Head File
*  @author   hongren.wang
*  @email    hrwang@intenginetech.com
*  @version  1.0
*  @date     2020-03-27
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/03/27 |    1.0    |   hongren.wang | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __IET_CRYPTO_H__
#define __IET_CRYPTO_H__

#include "iet_common.h"

/**
 * \defgroup    DEVICE_IET_CRYPTO  IET Crypto Driver Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for Crypto Driver (\ref iet_crypto.h)
 * \details provide interfaces for crypto driver to implement
 * @{
 *
 * \file
 * \brief   crypto controller definitions
 * \details provide common definitions for crypto,
 *  then software developer can develop crypto driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */
/**
 * \brief   Definitions for Crypto modules.
 * @{
 */
/**
 * \brief   Macro Definitions for CRYPTO driver and app
 * @{
 */

#define IET_CRYPTO_START                 (0)     /*!< bit 0: Write 1 to start crypto module */
#define IET_CRYPTO_START_EN              (0)     /*!< Write 1 to start Crypto module */
#define IET_CRYPTO_START_MASK            (0x00000001 << IET_CRYPTO_START) /*!< width:1 */

#define IET_CRYPTO_STATUS                (1)     /*!< bit 1: crypto module status [0:busy; 1:ready] */
#define IET_CRYPTO_STATUS_MASK           (0x00000001 << IET_CRYPTO_STATUS) /*!< width:1 */

#define IET_CRYPTO_AES_FUNC              (2)     /*!< bit 2: crypto aes function:[0:encrypt 1:decrypt] */
#define IET_CRYPTO_FUNC_ENCRYPT          (0)     /*!< aes function is encrypt */
#define IET_CRYPTO_FUNC_DECRYPT          (1)     /*!< aes function is decrypt */
#define IET_CRYPTO_AES_FUNC_MASK         (0x00000001 << IET_CRYPTO_AES_FUNC) /*!< width:1 */

#define IET_CRYPTO_INT_VECTOR            (3)     /*!< bit 3: crypto int vector use */
#define IET_CRYPTO_INT_VECTOR_PREV       (0)     /*!< use previous AES output as initial vector */
#define IET_CRYPTO_INT_VECTOR_IV         (1)     /*!< use init vection 0-3 as initial vector */
#define IET_CRYPTO_INT_VECTOR_SEL_MASK   (0x00000001 << IET_CRYPTO_INT_VECTOR) /*!< width:1 */

#define IET_CRYPTO_AES_MODE              (4)     /*!< bit 4: crypto aes mode (0:ECB;1:CBC;2:CTR) */
#define IET_CRYPTO_AES_MODE_ECB          (0)     /*!< crypto aes mode is ECB */
#define IET_CRYPTO_AES_MODE_CBC          (1)     /*!< crypto aes mode is CBC */
#define IET_CRYPTO_AES_MODE_CTR          (2)     /*!< crypto aes mode is CTR */
#define IET_CRYPTO_AES_MODE_MASK         (0x00000003 << IET_CRYPTO_AES_MODE) /*!< width:2 */

#define IET_CRYPTO_AES_KEY_LEN           (8)     /*!< bit 6 : crypto aes key length */
#define IET_CRYPTO_AES_KEY_LEN_128       (0)     /*!< crypto aes key length is 128 */
#define IET_CRYPTO_AES_KEY_LEN_192       (1)     /*!< crypto aes key length is 192 */
#define IET_CRYPTO_AES_KEY_LEN_256       (2)     /*!< crypto aes key length is 256 */
#define IET_CRYPTO_AES_KEY_LEN_MASK      (0x00000003 << IET_CRYPTO_AES_KEY_LEN) /*!< width:2 */

#define IET_CRYPTO_MODE                  (10)     /*!< bit 10: crypto mode */
#define IET_CRYPTO_MODE_AES              (0)      /*!< crypto mode is AES */
#define IET_CRYPTO_MODE_MD5              (1)      /*!< crypto mode is MD5 */
#define IET_CRYPTO_MODE_MASK             (0x0000000F << IET_CRYPTO_MODE) /*!< width:4 */

#define IET_CRYPTO_AES_BLOCK_NUM         (16)     /*!< bit 1: pre-process done interrupt status */
#define IET_CRYPTO_AES_BLOCK_NUM_MASK    (0x0000FFFF << IET_CRYPTO_AES_BLOCK_NUM) /*!< width:16 */


#define IET_CRYPTO_MD5_PHASE             (0)     /*!< bit 0: md5 phase */
#define IET_CRYPTO_MD5_PHASE_MASK        (0x00000003 << IET_CRYPTO_MD5_PHASE) /*!< width:2 */
#define IET_CRYPTO_MD5_MSG_LEN           (2)     /*!< bit 2: md5 message length */
#define IET_CRYPTO_MD5_MSG_LEN_MASK      (0x00FFFFFF << IET_CRYPTO_MD5_MSG_LEN) /*!< width:24 */




/** @} */
/**
 * \brief   Definitions for crypto param check
 * @{
 */
#define IS_IET_CRYPTO_MODE(VAL)          ((VAL == IET_CRYPTO_FUNC_ENCRYPT) || (VAL == IET_CRYPTO_FUNC_ENCRYPT))
#define IS_IET_CRYPTO_ADDR(VAL)          IS_VALID_VAL(VAL)
/** @} */

/**
 * \brief   IET Crypto config structure definition
 * @{
 */
typedef struct
{
    uint8_t  crypto_mode;         /*!<  */
    uint32_t aes_func;            /*!<  */
    uint32_t aes_int_vector_use;  /*!<  */
    uint32_t aes_int_vector[4];   /*!<  */
    uint32_t aes_mode;            /*!<  */
    uint8_t  aes_key_len;         /*!<  */
    uint8_t  aes_clock_num;       /*!<  */
    uint8_t  aes_out[4];          /*!<  */


    uint8_t  md5_phase;           /*!<  */ 
    uint8_t  md5_msg_len;         /*!<  */

    uint32_t *in;                 /*!<  input buffer address */
    uint32_t *out;                /*!<  output buffer address */

    void*    callback;             /*!< crypto callback fun */
} IET_CRYPTO_Config_TypeDef;
/** @} */
/** @} */

/**
 * \defgroup    DEVICE_IET_CRYPTO_CTRLCMD      CRYPTO Drive Control Commands
 * \ingroup DEVICE_IET_CRYPTO
 * \brief   Definitions for CRYPTO Driver Control Command
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/uint32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - RET_CRYPTO_OK,      Control device successfully
 *   - RET_CRYPTO_OPNED,   Device is opened, The second time can not be opened.
 *   - RET_CRYPTO_PAR,     Parameter is not valid for current control command
 *   - RET_CRYPTO_IO,      Control command is not supported or not valid
 * @{
 */
/** Define CRYPTO control commands for common usage */
#define DEV_SET_CRYPTO_SYSCMD(cmd)         DEV_SET_SYSCMD(cmd)

/* ++++ Common commands for CRYPTO driver ++++ */
/**
 * Set CRYPTO driver config
 * - Param type : IET_CRYPTO_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_CRYPTO_CMD_S_CONFIG            DEV_SET_CRYPTO_SYSCMD(0)

/**
 * Get CRYPTO driver config
 * - Param type : IET_CRYPTO_Config_TypeDef
 * - Param usage :
 * - Return value explanation :
 */
#define IET_CRYPTO_CMD_G_CONFIG            DEV_SET_CRYPTO_SYSCMD(1)

/**
 * Get CRYPTO driver status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_CRYPTO_CMD_G_STATUS            DEV_SET_CRYPTO_SYSCMD(2)

/**
 * Get CRYPTO driver intr status
 * - Param type : void*
 * - Param usage :
 * - Return value explanation :
 */
#define IET_CRYPTO_CMD_G_INTR_STATUS       DEV_SET_CRYPTO_SYSCMD(3)

/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    DEVICE_IET_CRYPTO_FUN CRYPTO Driver Functions
 * \ingroup     DEVICE_IET_CRYPTO
 * \brief       Definitions for crypto Driver API functions
 * @{
 */
/**
  \fn           uint32_t iet_crypto_open(IET_CRYPTO_Config_TypeDef *crypto_config)
  \brief        open iet crypto
  \param[in]    crypto_config      crypto config struct IET_CRYPTO_Config_TypeDef
  \return       RET_CRYPTO_OK      open successfully without any issues
  \return       RET_CRYPTO_OPNED   if device was opened before with different parameters, return RET_CRYPTO_OPNED
  \return       RET_CRYPTO_PAR     parameter is not valid
 */
extern uint32_t iet_crypto_open(IET_CRYPTO_Config_TypeDef *crypto_config);

/**
  \fn           int32_t iet_crypto_close(void)
  \brief        close iet crypto
  \return       RET_CRYPTO_OK      close successfully
*/
extern uint32_t iet_crypto_close(void);

/**
  \fn           iet_crypto_control(uint32_t ctrl_cmd, void *param)
  \brief        control crypto by control command [optional]
  \param[in]    ctrl_cmd        ref DEVICE_IET_CRYPTO_CTRLCMD "control command", to change or get some thing related to prep
  \param[in,out]
                param           parameters that maybe argument of the command, or return values of the command, must not be null
  \return       RET_CRYPTO_OK     control device successfully
  \return       RET_CRYPTO_PAR    parameter is not valid for current control command
  \return       RET_CRYPTO_IO     control command is not supported or not valid
*/
extern uint32_t iet_crypto_control(uint32_t ctrl_cmd, void *param);

/**
  \fn           int32_t iet_crypto_start(void)
  \brief        start crypto
  \return       RET_CRYPTO_OK     start successfully
*/
extern uint32_t iet_crypto_start(void);
/** @} */
#ifdef __cplusplus
}
#endif
/** @} */
/** @} */

#endif /* __IET_CRYPTO_H__ */

/* --------------------------------- End Of File ------------------------------ */
