/*****************************************************************************
*  Copyright (C) 2020-2022 INTENGINE Limited. All rights reserved.
*
*  Licensed under the GNU Lesser General Public License, version 2.1,
*  not use this file except in compliance with the License.
*
*  You should have received a copy of the GNU General Public License
*  along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*  @file     iet_qspi_flash.h
*  @brief    QSPI-Flash Driver Head File for BACH2
*  @author   Heng.Luo
*  @email    hengluo@intenginetech.com
*  @version  1.0
*  @date     2021-04-28
*  @license  GNU Lesser General Public License(LGPL), version 2.1
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2021/04/28 | 1.0       | Heng.Luo       | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef IET_QSPI_FLASH_H_
#define IET_QSPI_FLASH_H_

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"
/**
 * \defgroup    DEVICE_IET_QSPI_FLASH IET QSPI_FLASH Driver Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for QSPI_FLASH controller (\ref iet_qspi_flash.h)
 * \details provide interfaces for QSPI_FLASH driver to implement
 * @{
 *
 * \file
 * \brief   QSPI_FLASH controller definitions
 * \details provide common definitions for QSPI_FLASH device,
 *  then software developer can develop QSPI_FLASH driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */

/**
 * \defgroup    DEVICE_IET_QSPI_FLASH_DEVCONFIG   IET QSPI_FLASH Driver Config
 * \ingroup DEVICE_IET_QSPI_FLASH
 * \brief   contains definitions for QSPI_FLASH driver config.
 * \details this config will be used in user implemented code, which was called
 *     Device Driver Implement Layer for QSPI_FLASH to realize in user code.
 * @{
 */
/**
 * \brief   IET QSPI_FLASH write tpye enum definition
 * \details define QSPI_FLASH_WRITE_CFG
 * @{
 */
typedef enum
{
    QSPI_FLASH_WRITE_INT_SINGLE,
    QSPI_FLASH_WRITE_INT_QUAD,
    QSPI_FLASH_WRITE_INT_DMA_SINGLE,
} QSPI_FLASH_WRITE_CFG;
/** @} */
typedef enum
{
    QSPI_FLASH_DMA_DISABLE = 0,
    QSPI_FLASH_DMA_ENABLE,
} QSPI_FLASH_DMA_CFG;
/**
 * \brief   IET QSPI_FLASH read tpye enum definition
 * \details define QSPI_FLASH_READ_CFG
 * @{
 */
typedef enum
{
    QSPI_FLASH_READ_SINGLE,
    QSPI_FLASH_READ_DUAL,
    QSPI_FLASH_READ_QUAD,
    QSPI_FLASH_READ_DMA_SINGLE,
    QSPI_FLASH_READ_DMA_DUAL,
    QSPI_FLASH_READ_DMA_QUAD,
} QSPI_FLASH_READ_CFG;
/** @} */

/**
 * \brief   IET QSPI_FLASH erase tpye enum definition
 * \details define QSPI_FLASH_ERASE_CFG
 * @{
 */
typedef enum
{
    QSPI_FLASH_ERASE_SE,
    QSPI_FLASH_ERASE_BE,
    QSPI_FLASH_ERASE_CE,
} QSPI_FLASH_ERASE_CFG;
/** @} */
/** @} */

/**
 * \defgroup    DEVICE_IET_QSPI_FLASH_CTRLCMD       IET QSPI_FLASH Driver Control Commands
 * \ingroup DEVICE_IET_QSPI_FLASH
 * \brief   Definitions for QSPI_FLASH driver control commands
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure or value, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref RET_FLASH_OK,       Control device successfully
 * @{
 */
/** Define QSPI_FLASH control commands for common usage */
#define DEV_SET_QSPI_FLASH_SYSCMD(cmd)  DEV_SET_SYSCMD(DEV_SYS_CMDBSE|(cmd))

/* ++++ Common commands for TIMER Device ++++ */
/**
 * Read QSPI_FLASH manufacturer id.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation :
 */
#define QSPI_FLASH_CMD_MFRID            DEV_SET_QSPI_FLASH_SYSCMD(0)

/**
 * Read QSPI_FLASH device id.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation :
 */
#define QSPI_FLASH_CMD_DEVID            DEV_SET_QSPI_FLASH_SYSCMD(1)

/**
 * Read QSPI_FLASH chip size.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation :
 */
#define QSPI_FLASH_CMD_CHIP_SIZE        DEV_SET_QSPI_FLASH_SYSCMD(4)

/**
 * Read QSPI_FLASH sector size.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation :
 */
#define QSPI_FLASH_CMD_SEC_SIZE         DEV_SET_QSPI_FLASH_SYSCMD(5)

/**
 * Read QSPI_FLASH block size.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation :
 */
#define QSPI_FLASH_CMD_BLK_SIZE         DEV_SET_QSPI_FLASH_SYSCMD(6)
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    DEVICE_IET_QSPI_FLASH_FUN IET QSPI_FLASH driver functions
 * \ingroup     DEVICE_IET_QSPI_FLASH
 * \brief       Definitions for QSPI_FLASH driver API functions
 * @{
 */

/**
  \fn           int32_t iet_qspi_flash_open(uint8_t dma_cfg)
  \brief        open qspi_flash,supported flash capacity specifications:2MB 4MB 8MB 16MB
  \param[in]    dma_cfg         DMA enable/disable
  \return       RET_FLASH_OK
*/
extern int32_t iet_qspi_flash_open(uint8_t dma_cfg);

/**
  \fn           void iet_qspi_flash_close(void)
  \brief        close qspi_flash
  \param[in]    NULL
  \return       RET_FLASH_OK
*/
extern int32_t iet_qspi_flash_close(void);

/**
  \fn           int32_t iet_qspi_flash_write(QSPI_FLASH_WRITE_CFG cfg, uint32_t dst, uint32_t src, uint32_t size)
  \brief        write size data form src to dst
  \param[in]    type            type of write
  \param[in]    dst             addr of destination must 4 Byte align
  \param[in]    src             addr of source must 4 Byte align
  \param[in]    size            size of transfer; units: byte; write size must 4 Byte align
  \return       RET_FLASH_OK
*/
extern int32_t iet_qspi_flash_write(QSPI_FLASH_WRITE_CFG cfg, uint32_t dst, uint32_t src, uint32_t size);

/**
  \fn           int32_t iet_qspi_flash_read(QSPI_FLASH_READ_CFG type, uint32_t src, uint32_t dst, uint32_t size)
  \brief        read size data form src to dst
  \param[in]    type            type of write
  \param[in]    src             addr of source must 4 Byte align
  \param[in]    dst             addr of destination must 4 Byte align when use QSPI_FLASH_READ_DMA_SINGLE/QSPI_FLASH_READ_DMA_QUAD
  \param[in]    size            size of transfer; units: byte; read size must 4 Byte align when use QSPI_FLASH_READ_DMA_SINGLE/QSPI_FLASH_READ_DMA_QUAD
  \return       RET_FLASH_OK
*/
extern int32_t iet_qspi_flash_read(QSPI_FLASH_READ_CFG type, uint32_t src, uint32_t dst, uint32_t size);

/**
  \fn           int32_t iet_qspi_flash_erase(QSPI_FLASH_ERASE_CFG cfg, uint32_t addr, uint32_t num)
  \brief        erase cnt type size of flash
  \param[in]    cfg            type of erase
  \param[in]    dst             addr of start erase, alignsize sector/block size
  \param[in]    cnt             erase cnt of sector/block/chip size
  \return       RET_FLASH_OK
*/
extern int32_t iet_qspi_flash_erase(QSPI_FLASH_ERASE_CFG cfg, uint32_t addr, uint32_t num);

/**
  \fn           int32_t iet_qspi_flash_control(uint32_t ctrl_cmd, void *param)
  \brief        qspi_flash control function
  \param[in]    ctrl_cmd        control command
  \param[in]    param           command params
  \return       RET_FLASH_OK
*/
extern int32_t iet_qspi_flash_control(uint32_t ctrl_cmd, void *param);

/**
  \fn           iet_qspi_flash_protect_256KB(FunctionalState status)
  \brief        enable protect range from flash_base_addr to flash_base_addr + 256KB;disable protect all
                supported flash capacity specifications:2MB 4MB 8MB 16MB
  \param[in]    status    ENABLE/DISABLE
  \return       RET_FLASH_OK/RET_FLASH_ERR/RET_FLASH_TIMEOUT
*/
extern int32_t iet_qspi_flash_protect_256KB(FunctionalState status);

/** @} */
#ifdef __cplusplus
}
#endif
/** @} */

#endif /* IET_QSPI_FLASH_H_ */

/* ------------------------------ End Of File ------------------------------ */
