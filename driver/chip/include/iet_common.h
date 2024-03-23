/*****************************************************************************
*  Copyright (C) 2020-2022 INTENGINE Limited. All rights reserved.
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
*  @file        iet_common.h
*  @brief       iet common File Head File for BACH2
*  @author      Zenghua.Gao
*  @email       zhgao@intenginetech.com
*  @version     1.0
*  @date        2020-8-12
*  @license     GNU Lesser General Public License(LGPL), version 2.1
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/08/12 | 1.0   | Zenghua.Gao      | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef IET_COMMON_H
#define IET_COMMON_H

/* Includes ------------------------------------------------------------------- */
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "core_riscv.h"
#include "iet_system_bach.h"

/**
 *
 * \defgroup    DEVICE_IET_DEF IET Driver Interface
*/
/**
 \mainpage BACH2 DEVICE Software Library
  *
  * Introduction
  * ------------
  *
  * This user manual describes the BACH2 DEVICE Software Library,
  *
  * Examples
  * --------
  *
  * The library ships with a number of examples which demonstrate how to use the library functions.
  *
  * Toolchain Support
  * ------------
  *
  * The library has been developed and tested with GCC_10.2.0
  * The library is being tested in GCC toolchains.
  *
  *
  * Copyright Notice
  * ------------
  *
  * Copyright (C) 2020-2022 INTENGINE Limited. All rights reserved.
  *
  * Website
  * ------------
  *
  * https://www.intenginetech.com/
*/
/**
 * \defgroup    DEVICE_IET_COMMON   IET COMMON Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for Common Interface (\ref iet_common.h)
 * \details provide interfaces for common interface to implement
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
 * \defgroup    DEVICE_IET_COMMON_ERROR_DEFINITION  Common Error Definition
 * \ingroup DEVICE_IET_COMMON
 * \brief   definitions for Common Error Definitions
 * \details these error definitions will be used in user and system implemented code, which was called
 *     Device Driver Implement Layer for return value to realize in user code.
 * @{
 */

/**
 * \name    Main Error Code Definitions
 * @{
 */
#define E_OK                        (0)         /*!< ok */
#define E_ERR                       (0x1)       /*!< error */
#define E_SYS                       (0x5)       /*!< system error */
#define E_OPNED                     (0x6)       /*!< device is opened */
#define E_CLSED                     (0x7)       /*!< device is closed */
#define E_ALLOPNED                  (0x8)       /*!< device is all opened */
#define E_NOSPT                     (0x9)       /*!< unsupported features */
#define E_RSFN                      (0xA)       /*!< reserved function code */
#define E_RSATR                     (0xB)       /*!< reserved attribute */
#define E_PAR                       (0x11)      /*!< parameter error */
#define E_ID                        (0x12)      /*!< invalid ID number */
#define E_WTIRE                     (0x13)      /*!< write error */
#define E_READ                      (0x14)      /*!< read error */
#define E_IO                        (0x15)      /*!< io-control command error */
#define E_CTX                       (0x19)      /*!< context error */
#define E_MACV                      (0x1A)      /*!< memory access violation */
#define E_OACV                      (0x1B)      /*!< object access violation */
#define E_ILUSE                     (0x1C)      /*!< illegal service call use */
#define E_NOMEM                     (0x21)      /*!< insufficient memory */
#define E_NOID                      (0x22)      /*!< no ID number available */
#define E_NORES                     (0x23)      /*!< no resource available */
#define E_OBJ                       (0x29)      /*!< object state error */
#define E_NOEXS                     (0x2A)      /*!< non-existent object */
#define E_QOVR                      (0x2B)      /*!< queue overflow */
#define E_RLWAI                     (0x31)      /*!< forced release from waiting */
#define E_TMOUT                     (0x32)      /*!< polling failure or timeout */
#define E_DLT                       (0x33)      /*!< waiting object deleted */
#define E_CLS                       (0x34)      /*!< waiting object state changed */
#define E_READY                     (0x35)      /*!< waiting object state ready */
#define E_SRC                       (0x36)      /*!< source error */
#define E_DST                       (0x37)      /*!< destination error */
#define E_SIZE                      (0x38)      /*!< size error */
#define E_WBLK                      (0x39)      /*!< non-blocking accepted */
#define E_BOVR                      (0x3A)      /*!< buffer overflow */
#define E_NOCB                      (0x3B)      /*!< unfound callback function */
#define E_CMD                       (0x3C)      /*!< error io-control command */
#define E_TRANSFER                  (0x3D)      /*!< dev is transfering */
#define E_MUX                       (0x3E)      /*!< mux error */
#define E_UNMUX                     (0x3F)      /*!< unmux error */
#define E_FREE                      (0x40)      /*!< free error */
#define E_LOCKED                    (0x41)      /*!< lock error */
#define E_RUNNING                   (0x42)      /*!< running error */
#define E_NOTRUN                    (0x43)      /*!< not running error */
#define E_DISABLE                   (0x44)      /*!< disable error */
#define E_VERIFY                    (0x45)      /*!< checksum or crc16 verify*/
#define E_MALLOC                    (0x46)      /*!< malloc memory failed*/
#define E_IO_BUSY                   (0x47)      /*!< IO state BUSY */
#define E_RANGE                     (0x48)      /*!< parameter out of range */
#define E_ADDR_NOACK                (0x49)      /*!< I2C Slave address is sent but not addressed by any slave devices */
#define E_DATA_NOACK                (0x4a)      /*!< I2C Data in transfer is not acked when it should be acked */
#define E_MSTSTOP                   (0x4b)      /*!< I2C Slave received a STOP condition from master device */
#define E_MSTSTOP_NOACK             (0x4c)      /*!< I2C Slave received a NOACK condition from master device */
#define E_ABORT_USRE                (0x4d)      /*!< I2C user abort tx */
#define E_TX_OVER                   (0x4e)      /*!< tx buffer over flower */
#define E_RX_OVER                   (0x4f)      /*!< Rx buffer over flower */
#define E_BAUD                      (0x50)      /*!< Communication Baud rate error*/
#define E_MST_RESTART               (0x51)      /*!< I2C Slave received a restart condition from master device */
#define E_DIRECT                    (0x52)      /*!< data stream direction error */
#define E_LOST_BUS                  (0x53)      /*!< Master or slave lost bus during operation */
#define E_AES_KEY                   (0x54)      /*!< Crypto error of AES key */
#define E_AES_CALC                  (0x55)      /*!< Crypto error of AES calculate */
#define E_MD5_CALC                  (0x56)      /*!< Crypto error of MD5 calculate */
#define E_PSRAM_MD                  (0x1000)    /*!< PSRAM ERR MD*/
/** @} */


/**
 * \name    Error Code MODULE Definitions
 * \brief
 * ERROR_CODE\n
 * int32_t     0xABCCDDDD\n
 * A:force==0x8     B:process_module  -CC:child_module  -DDDD:error code
 * @{
 */
#define DEV_SYS_CMDBSE              (0x00000000)        /*!< default system device control command base(defined by embARC) */
#define DEV_USR_CMDBSE              (0x70000000)        /*!< default user device control command base(defined by user) in user implementing */
#define DEV_SET_SYSCMD(cmd)         (DEV_SYS_CMDBSE|(cmd))  /*!< set device system control command */
#define DEV_SET_USRCMD(cmd)         (DEV_USR_CMDBSE|(cmd))  /*!< set device user control command */

#define IET_ERRR_BASE               (0x80000000)
#define IET_ERRR_SYS_BASE           (IET_ERRR_BASE)
#define IET_ERRR_DRV_BASE           (IET_ERRR_BASE|0x1000000)
#define IET_ERRR_MID_BASE           (IET_ERRR_BASE|0x2000000)
#define IET_ERRR_APP_BASE           (IET_ERRR_BASE|0x3000000)
#define IET_ERRR_BOOT_BASE          (IET_ERRR_BASE|0x4000000)
#define IET_ERRR_LOADER_BASE        (IET_ERRR_BASE|0x5000000)
#define IET_ERRR_DSP_BASE           (IET_ERRR_BASE|0x6000000)
#define IET_ERRR_USER_BASE          (IET_ERRR_BASE|0x7000000)

#define RET_UART_BASE               (IET_ERRR_DRV_BASE|0x10000)
#define RET_I2C_BASE                (IET_ERRR_DRV_BASE|0x20000)
#define RET_I2S_BASE                (IET_ERRR_DRV_BASE|0x30000)
#define RET_GPIO_BASE               (IET_ERRR_DRV_BASE|0x40000)
#define RET_PDM_BASE                (IET_ERRR_DRV_BASE|0x50000)
#define RET_FLASH_BASE              (IET_ERRR_DRV_BASE|0x60000)
#define RET_WDT_BASE                (IET_ERRR_DRV_BASE|0x70000)
#define RET_TIMER_BASE              (IET_ERRR_DRV_BASE|0x80000)
#define RET_SYSTICK_BASE              ((RET_TIMER_BASE)|0x1000)
#define RET_FFVAD_BASE              (IET_ERRR_DRV_BASE|0x90000)
#define RET_SS_BASE                   ((RET_FFVAD_BASE)|0x1000)
#define RET_PREP_BASE                 ((RET_FFVAD_BASE)|0x2000)
#define RET_FFT_BASE                  ((RET_FFVAD_BASE)|0x3000)
#define RET_MFCC_BASE                 ((RET_FFVAD_BASE)|0x4000)
#define RET_VAD_BASE                  ((RET_FFVAD_BASE)|0x5000)
#define RET_SPI_BASE                (IET_ERRR_DRV_BASE|0xa0000)
#define RET_IPC_BASE                (IET_ERRR_DRV_BASE|0xb0000)
#define RET_VPU_BASE                (IET_ERRR_DRV_BASE|0xc0000)
#define RET_ADC_BASE                (IET_ERRR_DRV_BASE|0xd0000)
#define RET_SARADC_BASE               ((RET_ADC_BASE)|0x1000)
#define RET_PVD_BASE                  ((RET_ADC_BASE)|0x2000)
#define RET_ALG_BASE                (IET_ERRR_DRV_BASE|0xe0000)
#define RET_DSMDAC_BASE             (IET_ERRR_DRV_BASE|0xf0000)
#define RET_DMA_BASE                (IET_ERRR_DRV_BASE|0x100000)
#define RET_PM_BASE                 (IET_ERRR_DRV_BASE|0x110000)
#define RET_PLL_BASE                (IET_ERRR_DRV_BASE|0x120000)
#define RET_PSRAM_BASE              (IET_ERRR_DRV_BASE|0x130000)
#define RET_TOUCH_BASE              (IET_ERRR_DRV_BASE|0x140000)
#define RET_CRYPTO_BASE             (IET_ERRR_DRV_BASE|0x150000)
#define RET_AEC_BASE                (IET_ERRR_DRV_BASE|0x160000)
#define RET_PAD_BASE                (IET_ERRR_DRV_BASE|0x170000)
/** @} */

/**
 * \name    USR Error Code Definitions
 * @{
 */
#define RET_USR_OK              DEV_SET_USRCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_USR_ERR             DEV_SET_USRCMD(IET_ERRR_USER_BASE|E_ERR)
#define RET_USR_PAR             DEV_SET_USRCMD(IET_ERRR_USER_BASE|E_PAR)
#define RET_USR_OPEND           DEV_SET_USRCMD(IET_ERRR_USER_BASE|E_OPNED)
#define RET_USR_CLSED           DEV_SET_USRCMD(IET_ERRR_USER_BASE|E_CLSED)
#define RET_USR_IO              DEV_SET_USRCMD(IET_ERRR_USER_BASE|E_IO)
#define RET_USR_READ            DEV_SET_USRCMD(IET_ERRR_USER_BASE|E_READ)
#define RET_USR_WRITE           DEV_SET_USRCMD(IET_ERRR_USER_BASE|E_WTIRE)
#define RET_USR_CTX             DEV_SET_USRCMD(IET_ERRR_USER_BASE|E_CTX)

/** @} */

/**
 * \name    UART Error Code Definitions
 * @{
 */
#define RET_UART_OK             DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_UART_ERR            DEV_SET_SYSCMD(RET_UART_BASE|E_ERR)
#define RET_UART_PAR            DEV_SET_SYSCMD(RET_UART_BASE|E_PAR)
#define RET_UART_OPEND          DEV_SET_SYSCMD(RET_UART_BASE|E_OPNED)
#define RET_UART_CLSED          DEV_SET_SYSCMD(RET_UART_BASE|E_CLSED)
#define RET_UART_IO             DEV_SET_SYSCMD(RET_UART_BASE|E_IO)
#define RET_UART_READ           DEV_SET_SYSCMD(RET_UART_BASE|E_READ)
#define RET_UART_WRITE          DEV_SET_SYSCMD(RET_UART_BASE|E_WTIRE)
#define RET_UART_CMD            DEV_SET_SYSCMD(RET_UART_BASE|E_CMD)
#define RET_UART_NOMEM          DEV_SET_SYSCMD(RET_UART_BASE|E_NOMEM)
#define RET_UART_BUSY           DEV_SET_SYSCMD(RET_UART_BASE|E_IO_BUSY)
#define RET_UART_TIMEOUT        DEV_SET_SYSCMD(RET_UART_BASE|E_TMOUT)
/** @} */

/**
 * \name    I2C Error Code Definitions
 * @{
 */
#define RET_I2C_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_I2C_PAR             DEV_SET_SYSCMD(RET_I2C_BASE|E_PAR)
#define RET_I2C_OPEND           DEV_SET_SYSCMD(RET_I2C_BASE|E_OPNED)
#define RET_I2C_CLSED           DEV_SET_SYSCMD(RET_I2C_BASE|E_CLSED)
#define RET_I2C_IO              DEV_SET_SYSCMD(RET_I2C_BASE|E_IO)
#define RET_I2C_READ            DEV_SET_SYSCMD(RET_I2C_BASE|E_READ)
#define RET_I2C_WRITE           DEV_SET_SYSCMD(RET_I2C_BASE|E_WTIRE)
#define RET_I2C_LOST_BUS        DEV_SET_SYSCMD(RET_I2C_BASE|E_LOST_BUS)
#define RET_I2C_ADDR_NOACK      DEV_SET_SYSCMD(RET_I2C_BASE|E_ADDR_NOACK)
#define RET_I2C_DATA_NOACK      DEV_SET_SYSCMD(RET_I2C_BASE|E_DATA_NOACK)
#define RET_I2C_MSTSTOP         DEV_SET_SYSCMD(RET_I2C_BASE|E_MSTSTOP)
#define RET_I2C_MSTSTOP_NOACK   DEV_SET_SYSCMD(RET_I2C_BASE|E_MSTSTOP_NOACK)
#define RET_I2C_MSTSTOP_RESTART DEV_SET_SYSCMD(RET_I2C_BASE|E_MST_RESTART)
#define RET_I2C_ABORT_USRE      DEV_SET_SYSCMD(RET_I2C_BASE|E_ABORT_USRE)
#define RET_I2C_UNDEF           DEV_SET_SYSCMD(RET_I2C_BASE|E_ERR)
#define RET_I2C_IO_BUSY         DEV_SET_SYSCMD(RET_I2C_BASE|E_IO_BUSY)
#define RET_I2C_TIMEOUT         DEV_SET_SYSCMD(RET_I2C_BASE|E_TMOUT)
#define RET_I2C_TX_OVER         DEV_SET_SYSCMD(RET_I2C_BASE|E_TX_OVER)
#define RET_I2C_RX_OVER         DEV_SET_SYSCMD(RET_I2C_BASE|E_RX_OVER)
#define RET_I2C_BAUD            DEV_SET_SYSCMD(RET_I2C_BASE|E_BAUD)
#define RET_I2C_DIRECT          DEV_SET_SYSCMD(RET_I2C_BASE|E_DIRECT)
/** @} */

/**
 * \name    I2S Error Code Definitions
 * @{
 */
#define RET_I2S_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_I2S_ERR             DEV_SET_SYSCMD(RET_I2S_BASE|E_ERR)
#define RET_I2S_PAR             DEV_SET_SYSCMD(RET_I2S_BASE|E_PAR)
#define RET_I2S_OPEND           DEV_SET_SYSCMD(RET_I2S_BASE|E_OPNED)
#define RET_I2S_CLSED           DEV_SET_SYSCMD(RET_I2S_BASE|E_CLSED)
#define RET_I2S_IO              DEV_SET_SYSCMD(RET_I2S_BASE|E_IO)
#define RET_I2S_NOTRUN          DEV_SET_SYSCMD(RET_I2S_BASE|E_NOTRUN)
#define RET_I2S_RUNNING         DEV_SET_SYSCMD(RET_I2S_BASE|E_RUNNING)
#define RET_I2S_DMA_TRANSFER    DEV_SET_SYSCMD(RET_I2S_BASE|E_TRANSFER)
#define RET_I2S_DMA_LOCKED      DEV_SET_SYSCMD(RET_I2S_BASE|E_LOCKED)
#define RET_I2S_TIMEOUT         DEV_SET_SYSCMD(RET_I2S_BASE|E_TMOUT)

/** @} */


/**
 * \name    CRYPTO Error Code Definitions
 * @{
 */
#define RET_CRYPTO_OK           DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_CRYPTO_PAR          DEV_SET_SYSCMD(RET_CRYPTO_BASE|E_PAR)
#define RET_CRYPTO_AES_CALC     DEV_SET_SYSCMD(RET_CRYPTO_BASE|E_AES_CALC)
#define RET_CRYPTO_MD5_CALC     DEV_SET_SYSCMD(RET_CRYPTO_BASE|E_MD5_CALC)
/** @} */

/**
 * \name    GPIO Error Code Definitions
 * @{
 */
#define RET_GPIO_OK             DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_GPIO_PAR            DEV_SET_SYSCMD(RET_GPIO_BASE|E_PAR)
#define RET_GPIO_OPEND          DEV_SET_SYSCMD(RET_GPIO_BASE|E_OPNED)
#define RET_GPIO_CLSED          DEV_SET_SYSCMD(RET_GPIO_BASE|E_CLSED)
#define RET_GPIO_IO             DEV_SET_SYSCMD(RET_GPIO_BASE|E_IO)
#define RET_GPIO_READ           DEV_SET_SYSCMD(RET_GPIO_BASE|E_READ)
#define RET_GPIO_WRITE          DEV_SET_SYSCMD(RET_GPIO_BASE|E_WTIRE)
/** @} */

/**
 * \name    VPU Error Code Definitions
 * @{
 */
#define RET_VPU_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_VPU_NOCB            DEV_SET_SYSCMD(RET_VPU_BASE|E_NOCB)
#define RET_VPU_CMD             DEV_SET_SYSCMD(RET_VPU_BASE|E_CMD)
#define RET_VPU_PAR             DEV_SET_SYSCMD(RET_VPU_BASE|E_PAR)
/** @} */

/**
 * \name    HXD019 Error Code Definitions
 * @{
 */
#define RET_HXD019_OK               DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
/** @} */

/**
 * \name    PDM Error Code Definitions
 * @{
 */
#define RET_PDM_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_PDM_PAR             DEV_SET_SYSCMD(RET_PDM_BASE|E_PAR)
#define RET_PDM_OPEND           DEV_SET_SYSCMD(RET_PDM_BASE|E_OPNED)
#define RET_PDM_CLSED           DEV_SET_SYSCMD(RET_PDM_BASE|E_CLSED)
#define RET_PDM_IO              DEV_SET_SYSCMD(RET_PDM_BASE|E_IO)
#define RET_PDM_NOTRUN          DEV_SET_SYSCMD(RET_PDM_BASE|E_NOTRUN)
#define RET_PDM_RUNNING         DEV_SET_SYSCMD(RET_PDM_BASE|E_RUNNING)
/** @} */

/**
 * \name    FLASH Error Code Definitions
 * @{
 */
#define RET_FLASH_OK            DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_FLASH_ERR           DEV_SET_SYSCMD(RET_FLASH_BASE|E_ERR)
#define RET_FLASH_PAR           DEV_SET_SYSCMD(RET_FLASH_BASE|E_PAR)
#define RET_FLASH_OPEND         DEV_SET_SYSCMD(RET_FLASH_BASE|E_OPNED)
#define RET_FLASH_CLSED         DEV_SET_SYSCMD(RET_FLASH_BASE|E_CLSED)
#define RET_FLASH_IO            DEV_SET_SYSCMD(RET_FLASH_BASE|E_IO)
#define RET_FLASH_READ          DEV_SET_SYSCMD(RET_FLASH_BASE|E_READ)
#define RET_FLASH_WRITE         DEV_SET_SYSCMD(RET_FLASH_BASE|E_WTIRE)
#define RET_FLASH_READY         DEV_SET_SYSCMD(RET_FLASH_BASE|E_READY)
#define RET_FLASH_CMD           DEV_SET_SYSCMD(RET_FLASH_BASE|E_CMD)
#define RET_FLASH_ADDRESS       DEV_SET_SYSCMD(RET_FLASH_BASE|E_NOMEM)
#define RET_FLASH_TIMEOUT       DEV_SET_SYSCMD(RET_FLASH_BASE|E_TMOUT)
#define RET_FLASH_BUSY          DEV_SET_SYSCMD(RET_FLASH_BASE|E_IO_BUSY)
/** @} */

/**
 * \name    AEC Error Code Definitions
 * @{
 */
#define RET_AEC_OK             DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_AEC_PAR            DEV_SET_SYSCMD(RET_AEC_BASE|E_PAR)
#define RET_AEC_OPEND          DEV_SET_SYSCMD(RET_AEC_BASE|E_OPNED)
#define RET_AEC_CLSED          DEV_SET_SYSCMD(RET_AEC_BASE|E_CLSED)
#define RET_AEC_IO             DEV_SET_SYSCMD(RET_AEC_BASE|E_IO)
#define RET_AEC_READ           DEV_SET_SYSCMD(RET_AEC_BASE|E_READ)
#define RET_AEC_WRITE          DEV_SET_SYSCMD(RET_AEC_BASE|E_WTIRE)
/** @} */

/**
 * \name    TOUCH Error Code Definitions
 * @{
 */
#define RET_TOUCH_OK            DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_TOUCH_ERR           DEV_SET_SYSCMD(RET_TOUCH_BASE|E_ERR)
#define RET_TOUCH_PAR           DEV_SET_SYSCMD(RET_TOUCH_BASE|E_PAR)
#define RET_TOUCH_OPEND         DEV_SET_SYSCMD(RET_TOUCH_BASE|E_OPNED)
#define RET_TOUCH_CLSED         DEV_SET_SYSCMD(RET_TOUCH_BASE|E_CLSED)
#define RET_TOUCH_IO            DEV_SET_SYSCMD(RET_TOUCH_BASE|E_IO)
#define RET_TOUCH_CMD           DEV_SET_SYSCMD(RET_TOUCH_BASE|E_CMD)
/** @} */

/**
 * \name    PSRAM Error Code Definitions
 * @{
 */
#define RET_PSRAM_OK            DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_PSRAM_ERR           DEV_SET_SYSCMD(RET_PSRAM_BASE|E_ERR)
#define RET_PSRAM_PAR           DEV_SET_SYSCMD(RET_PSRAM_BASE|E_PAR)
#define RET_PSRAM_TMOUT         DEV_SET_SYSCMD(RET_PSRAM_BASE|E_TMOUT)
#define RET_PSRAM_1XXX          DEV_SET_SYSCMD(RET_PSRAM_BASE|E_PSRAM_MD)
/** @} */

/**
 * \name    WDT Error Code Definitions
 * @{
 */
#define RET_WDT_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_WDT_PAR             DEV_SET_SYSCMD(RET_WDT_BASE|E_PAR)
#define RET_WDT_OPEND           DEV_SET_SYSCMD(RET_WDT_BASE|E_OPNED)
#define RET_WDT_CLSED           DEV_SET_SYSCMD(RET_WDT_BASE|E_CLSED)
#define RET_WDT_IO              DEV_SET_SYSCMD(RET_WDT_BASE|E_IO)
#define RET_WDT_CMD             DEV_SET_SYSCMD(RET_WDT_BASE|E_CMD)
/** @} */

/**
 * \name    TIMER Error Code Definitions
 * @{
 */
#define RET_TIMER_OK            DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_TIMER_PAR           DEV_SET_SYSCMD(RET_TIMER_BASE|E_PAR)
#define RET_TIMER_RANGE         DEV_SET_SYSCMD(RET_TIMER_BASE|E_RANGE)
#define RET_TIMER_OPEND         DEV_SET_SYSCMD(RET_TIMER_BASE|E_OPNED)
#define RET_TIMER_CLSED         DEV_SET_SYSCMD(RET_TIMER_BASE|E_CLSED)
#define RET_TIMER_IO            DEV_SET_SYSCMD(RET_TIMER_BASE|E_IO)
#define RET_TIMER_CMD           DEV_SET_SYSCMD(RET_TIMER_BASE|E_CMD)
#define RET_SYSTICK_OK          DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_SYSTICK_PAR         DEV_SET_SYSCMD(RET_SYSTICK_BASE|E_PAR)
/** @} */

/**
 * \name    FF Error Code Definitions
 * @{
 */
#define RET_FFVAD_OK            DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_FFVAD_PAR           DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_PAR)
#define RET_FFVAD_OPEND         DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OPNED)
#define RET_FFVAD_CLSED         DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_CLSED)
#define RET_FFVAD_IO            DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_IO)
#define RET_FFVAD_NOTRUN        DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_NOTRUN)
#define RET_FFVAD_RUNNING       DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_RUNNING)
#define RET_FFVAD_DISABLE       DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_DISABLE)
#define RET_SS_OK               DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_SS_PAR              DEV_SET_SYSCMD(RET_SS_BASE|E_PAR)
#define RET_SS_OPEND            DEV_SET_SYSCMD(RET_SS_BASE|E_OPNED)
#define RET_SS_CLSED            DEV_SET_SYSCMD(RET_SS_BASE|E_CLSED)
#define RET_SS_IO               DEV_SET_SYSCMD(RET_SS_BASE|E_IO)
#define RET_SS_NOTRUN           DEV_SET_SYSCMD(RET_SS_BASE|E_NOTRUN)
#define RET_SS_RUNNING          DEV_SET_SYSCMD(RET_SS_BASE|E_RUNNING)
#define RET_SS_DISABLE          DEV_SET_SYSCMD(RET_SS_BASE|E_DISABLE)
#define RET_PREP_OK             DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_PREP_PAR            DEV_SET_SYSCMD(RET_PREP_BASE|E_PAR)
#define RET_PREP_OPEND          DEV_SET_SYSCMD(RET_PREP_BASE|E_OPNED)
#define RET_PREP_CLSED          DEV_SET_SYSCMD(RET_PREP_BASE|E_CLSED)
#define RET_PREP_IO             DEV_SET_SYSCMD(RET_PREP_BASE|E_IO)
#define RET_PREP_NOTRUN         DEV_SET_SYSCMD(RET_PREP_BASE|E_NOTRUN)
#define RET_PREP_RUNNING        DEV_SET_SYSCMD(RET_PREP_BASE|E_RUNNING)
#define RET_FFT_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_FFT_PAR             DEV_SET_SYSCMD(RET_FFT_BASE|E_PAR)
#define RET_FFT_OPEND           DEV_SET_SYSCMD(RET_FFT_BASE|E_OPNED)
#define RET_FFT_CLSED           DEV_SET_SYSCMD(RET_FFT_BASE|E_CLSED)
#define RET_FFT_IO              DEV_SET_SYSCMD(RET_FFT_BASE|E_IO)
#define RET_FFT_NOTRUN          DEV_SET_SYSCMD(RET_FFT_BASE|E_NOTRUN)
#define RET_FFT_RUNNING         DEV_SET_SYSCMD(RET_FFT_BASE|E_RUNNING)
#define RET_MFCC_OK             DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_MFCC_PAR            DEV_SET_SYSCMD(RET_MFCC_BASE|E_PAR)
#define RET_MFCC_OPEND          DEV_SET_SYSCMD(RET_MFCC_BASE|E_OPNED)
#define RET_MFCC_CLSED          DEV_SET_SYSCMD(RET_MFCC_BASE|E_CLSED)
#define RET_MFCC_IO             DEV_SET_SYSCMD(RET_MFCC_BASE|E_IO)
#define RET_MFCC_NOTRUN         DEV_SET_SYSCMD(RET_MFCC_BASE|E_NOTRUN)
#define RET_MFCC_RUNNING        DEV_SET_SYSCMD(RET_MFCC_BASE|E_RUNNING)
#define RET_VAD_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_VAD_PAR             DEV_SET_SYSCMD(RET_VAD_BASE|E_PAR)
#define RET_VAD_OPEND           DEV_SET_SYSCMD(RET_VAD_BASE|E_OPNED)
#define RET_VAD_CLSED           DEV_SET_SYSCMD(RET_VAD_BASE|E_CLSED)
#define RET_VAD_IO              DEV_SET_SYSCMD(RET_VAD_BASE|E_IO)
#define RET_VAD_NOTRUN          DEV_SET_SYSCMD(RET_VAD_BASE|E_NOTRUN)
#define RET_VAD_RUNNING         DEV_SET_SYSCMD(RET_VAD_BASE|E_RUNNING)
#define RET_VAD_DISABLE         DEV_SET_SYSCMD(RET_VAD_BASE|E_DISABLE)
/** @} */

/**
 * \name    DSMDAC Error Code Definitions
 * @{
 */
#define RET_DSMDAC_OK           DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_DSMDAC_PAR          DEV_SET_SYSCMD(RET_DSMDAC_BASE|E_PAR)
#define RET_DSMDAC_OPEND        DEV_SET_SYSCMD(RET_DSMDAC_BASE|E_OPNED)
#define RET_DSMDAC_CLSED        DEV_SET_SYSCMD(RET_DSMDAC_BASE|E_CLSED)
#define RET_DSMDAC_IO           DEV_SET_SYSCMD(RET_DSMDAC_BASE|E_IO)
#define RET_DSMDAC_NOTRUN       DEV_SET_SYSCMD(RET_DSMDAC_BASE|E_NOTRUN)
#define RET_DSMDAC_RUNNING      DEV_SET_SYSCMD(RET_DSMDAC_BASE|E_RUNNING)
/** @} */

/**
 * \name    DMA Error Code Definitions
 * @{
 */
#define RET_DMA_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_DMA_ERR             DEV_SET_SYSCMD(RET_DMA_BASE|E_ERR)
#define RET_DMA_PAR             DEV_SET_SYSCMD(RET_DMA_BASE|E_PAR)
#define RET_DMA_OPEND           DEV_SET_SYSCMD(RET_DMA_BASE|E_OPNED)
#define RET_DMA_CLSED           DEV_SET_SYSCMD(RET_DMA_BASE|E_CLSED)
#define RET_DMA_IO              DEV_SET_SYSCMD(RET_DMA_BASE|E_IO)
#define RET_DMA_NOTRUN          DEV_SET_SYSCMD(RET_DMA_BASE|E_NOTRUN)
#define RET_DMA_RUNNING         DEV_SET_SYSCMD(RET_DMA_BASE|E_RUNNING)
#define RET_DMA_NORES           DEV_SET_SYSCMD(RET_DMA_BASE|E_NORES)
/** @} */

/**
 * \name    SPI Error Code Definitions
 * @{
 */
#define RET_SPI_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_SPI_ERR             DEV_SET_SYSCMD(RET_SPI_BASE|E_ERR)
#define RET_SPI_PAR             DEV_SET_SYSCMD(RET_SPI_BASE|E_PAR)
#define RET_SPI_OPEND           DEV_SET_SYSCMD(RET_SPI_BASE|E_OPNED)
#define RET_SPI_CLSED           DEV_SET_SYSCMD(RET_SPI_BASE|E_CLSED)
#define RET_SPI_TMOUT           DEV_SET_SYSCMD(RET_SPI_BASE|E_TMOUT)
#define RET_SPI_SIZE            DEV_SET_SYSCMD(RET_SPI_BASE|E_SIZE)
#define RET_SPI_CMD             DEV_SET_SYSCMD(RET_SPI_BASE|E_CMD)
#define RET_SPI_BUSY            DEV_SET_SYSCMD(RET_SPI_BASE|E_IO_BUSY)
#define RET_SPI_RX_OVER         DEV_SET_SYSCMD(RET_SPI_BASE|E_RX_OVER)
#define RET_SPI_TX_OVER         DEV_SET_SYSCMD(RET_SPI_BASE|E_TX_OVER)
/** @} */

/**
 * \name    IPC Error Code Definitions
 * @{
 */
#define RET_IPC_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_IPC_ERR             DEV_SET_SYSCMD(RET_IPC_BASE|E_ERR)
#define RET_IPC_PAR             DEV_SET_SYSCMD(RET_IPC_BASE|E_PAR)
#define RET_IPC_OPEND           DEV_SET_SYSCMD(RET_IPC_BASE|E_OPNED)
#define RET_IPC_E_CLSED         DEV_SET_SYSCMD(RET_IPC_BASE|E_CLSED)
#define RET_IPC_LOCK_OK         DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_IPC_LOCK_ERR        DEV_SET_SYSCMD(RET_IPC_BASE|E_LOCKED)
/** @} */

/**
 * \name    ALG Error Code Definitions
 * @{
 */
#define RET_ALG_OK             DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_ALG_ERR            DEV_SET_SYSCMD(RET_ALG_BASE|E_ERR)
#define RET_ALG_PAR            DEV_SET_SYSCMD(RET_ALG_BASE|E_PAR)
#define RET_ALG_OPEND          DEV_SET_SYSCMD(RET_ALG_BASE|E_OPNED)
#define RET_ALG_CLSED          DEV_SET_SYSCMD(RET_ALG_BASE|E_CLSED)
/** @} */
/**
 * \name    ADC Error Code Definitions
 * @{
 */
#define RET_ADC_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_ADC_PAR             DEV_SET_SYSCMD(RET_ADC_BASE|E_PAR)
#define RET_ADC_OPEND           DEV_SET_SYSCMD(RET_ADC_BASE|E_OPNED)
#define RET_ADC_CLSED           DEV_SET_SYSCMD(RET_ADC_BASE|E_CLSED)
#define RET_ADC_IO              DEV_SET_SYSCMD(RET_ADC_BASE|E_IO)
#define RET_ADC_NOTRUN          DEV_SET_SYSCMD(RET_ADC_BASE|E_NOTRUN)
#define RET_ADC_RUNNING         DEV_SET_SYSCMD(RET_ADC_BASE|E_RUNNING)
#define RET_ADC_DMA_TRANSFER    DEV_SET_SYSCMD(RET_ADC_BASE|E_TRANSFER)
#define RET_SARADC_OK           DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_SARADC_PAR          DEV_SET_SYSCMD(RET_SARADC_BASE|E_PAR)
#define RET_SARADC_OPEND        DEV_SET_SYSCMD(RET_SARADC_BASE|E_OPNED)
#define RET_SARADC_CLSED        DEV_SET_SYSCMD(RET_SARADC_BASE|E_CLSED)
#define RET_SARADC_IO           DEV_SET_SYSCMD(RET_SARADC_BASE|E_IO)
#define RET_SARADC_NOTRUN       DEV_SET_SYSCMD(RET_SARADC_BASE|E_NOTRUN)
#define RET_SARADC_RUNNING      DEV_SET_SYSCMD(RET_SARADC_BASE|E_RUNNING)
#define RET_SARADC_BUSY         DEV_SET_SYSCMD(RET_SARADC_BASE|E_IO_BUSY)
#define RET_PVD_OK              DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_PVD_PAR             DEV_SET_SYSCMD(RET_PVD_BASE|E_PAR)
#define RET_PVD_OPEND           DEV_SET_SYSCMD(RET_PVD_BASE|E_OPNED)
#define RET_PVD_CLSED           DEV_SET_SYSCMD(RET_PVD_BASE|E_CLSED)
#define RET_PVD_IO              DEV_SET_SYSCMD(RET_PVD_BASE|E_IO)
#define RET_PVD_NOTRUN          DEV_SET_SYSCMD(RET_PVD_BASE|E_NOTRUN)
#define RET_PVD_RUNNING         DEV_SET_SYSCMD(RET_PVD_BASE|E_RUNNING)
/** @} */
/**
 * \name    PAD Error Code Definitions
 * @{
 */
#define RET_PAD_OK             DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_PAD_PAR            DEV_SET_SYSCMD(RET_PAD_BASE|E_PAR)
#define RET_PAD_READ           DEV_SET_SYSCMD(RET_PAD_BASE|E_READ)
#define RET_PAD_WRITE          DEV_SET_SYSCMD(RET_PAD_BASE|E_WTIRE)
/** @} */

/**
 * \name    Power/CLOCK management Error Code Definitions
 * @{
 */
#define RET_PM_OK               DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_PM_PAR              DEV_SET_SYSCMD(RET_PM_BASE|E_PAR)
/** @} */

/**
 * \name    PLL Error Code Definitions
 * @{
 */
#define RET_PLL_OK               DEV_SET_SYSCMD(DEV_SYS_CMDBSE|E_OK)
#define RET_PLL_PAR              DEV_SET_SYSCMD(RET_PLL_BASE|E_PAR)
/** @} */

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif
/** @} */

/**
 * \defgroup    DEVICE_IET_COMMON_DEVCONFIG IET Common Config
 * \ingroup DEVICE_IET_COMMON
 * \brief   contains definitions of common config.
 * \details this config will be used in user implemented code, which was called
 *     Device Driver Implement Layer for common to realize in user code.
 * @{
 */


/** Define whether use param check */
#define USE_FULL_ASSERT

/** Define whether use uart print message */
#define DEBUG_MSG

/** Define whether use uart print debug message */
#define DEBUG_DBG

/** Define whether use uart print error message */
#define DEBUG_ERR

#ifndef MAX_LOG_LENGTH
/** Define use uart print len if not define of make */
#define MAX_LOG_LENGTH   256
#endif

/* Uncomment the line below to expanse the "assert_param" macro in the
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed.
  *   If expr is true, it returns no value.
  */
#define assert_param(expr) ((expr) ? (void)0 : ({iet_printf_msg("Wrong parameters value: file %s on line %d\r\n", (uint8_t *)__FILE__, __LINE__);return E_ERR;}))

/**
  * @brief  The check_param macro is used for function's parameters check.
  * @param  expr
  * @param  err
  *   If expr is false, return err
  *   line number of the call that failed.
  *   If expr is true, it returns no value.

  */
#define check_param(expr, err) ((expr) ? (void)0 : ({return err;}))
#else
#define uart_assert_param(expr) ((void)0)
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */


#ifndef RAMDOM_SPI_ADDR
#define RAMDOM_SPI_ADDR         (0x10000000)
#endif
#ifndef IET_FLASH_BASE_ADDR
#define IET_FLASH_BASE_ADDR     (0x00000000)
#endif
#ifndef IET_ARM_RAM_BASE_ADDR
#define IET_ARM_RAM_BASE_ADDR   (0x00000000)
#endif
#ifndef IET_ARM_RAM_SIZE
#define IET_ARM_RAM_SIZE        (0x40000)
#endif
#ifndef IET_VPU_RAM_BASE_ADDR
#define IET_VPU_RAM_BASE_ADDR   (0xA0000000)
#endif
#ifndef IET_VPU_RAM_SIZE
#define IET_VPU_RAM_SIZE        (0x200000)
#endif
#ifndef IET_VPU_LMEM_BASE_ADDR
#define IET_VPU_LMEM_BASE_ADDR  (0xA0200000)
#endif
#ifndef IET_VPU_LMEM_SIZE
#define IET_VPU_LMEM_SIZE       (0x4000)
#endif
#ifndef IET_VPU_CMEM_BASE_ADDR
#define IET_VPU_CMEM_BASE_ADDR  (0xA0208000)
#endif
#ifndef IET_VPU_CMEM_SIZE
#define IET_VPU_CMEM_SIZE       (0x1000)
#endif
#define IS_ARM_RAM_ADDR(ADDR, SIZE)     ((((uint32_t )ADDR) < (IET_ARM_RAM_BASE_ADDR + IET_ARM_RAM_SIZE)) && ((((uint32_t )ADDR) + ((uint32_t )SIZE)) <= (IET_ARM_RAM_BASE_ADDR + IET_ARM_RAM_SIZE)))
#define IS_VPU_RAM_ADDR(ADDR, SIZE)     ((((uint32_t )ADDR) >= IET_VPU_RAM_BASE_ADDR) && (((uint32_t )ADDR) < (IET_VPU_RAM_BASE_ADDR + IET_VPU_RAM_SIZE)) && ((((uint32_t )ADDR) + ((uint32_t )SIZE)) <= (IET_VPU_RAM_BASE_ADDR + IET_VPU_RAM_SIZE)))
#define IS_VPU_LMEM_ADDR(ADDR, SIZE)    ((((uint32_t )ADDR) >= IET_VPU_LMEM_BASE_ADDR) && (((uint32_t )ADDR) < (IET_VPU_LMEM_BASE_ADDR + IET_VPU_LMEM_SIZE)) && ((((uint32_t )ADDR) + ((uint32_t )SIZE)) <= (IET_VPU_LMEM_BASE_ADDR + IET_VPU_LMEM_SIZE)))
#define IS_VPU_CMEM_ADDR(ADDR, SIZE)    ((((uint32_t )ADDR) >= IET_VPU_CMEM_BASE_ADDR) && (((uint32_t )ADDR) < (IET_VPU_CMEM_BASE_ADDR + IET_VPU_CMEM_SIZE)) && ((((uint32_t )ADDR) + ((uint32_t )SIZE)) <= (IET_VPU_CMEM_BASE_ADDR + IET_VPU_CMEM_SIZE)))
#define IS_VALID_VAL(VAL)               ((VAL) != NULL)
#define IS_OPENED_FLAG(FLAG)            ((FLAG) == 1)
#define IS_LENGTH_VAL(VAL)            ((VAL) != 0)
#define IS_ALIGN_4BYTE(ADDR)          (((ADDR)&0x00000003) == 0)

#define DEV_INTNO_INVALID       (0xFFFFFFFF)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define swap_32(num) (((num)>>24)|(((num)&0x00ff0000)>>8)|(((num)&0x0000ff00)<<8)|((num)<<24))

#define CONV2VOID(param)            ((void *)(long)(param)) //convert param into void * type

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/** IET i2s-bclk config definition for wm8978 */
#define BACH2_AUDIO_BCLK_512       (0xff001f)
/** IET i2s-bclk config definition for adc/dac/ac107 */
#define BACH2_AUDIO_BCLK_1024      (0x7f001f)


/** Definitions for AUDIO Parameter Check*/
#define IS_AUDIO_LENGTH_VAL(VAL)   (((VAL) > 16) && ((VAL) < 0xffff))


/**
  * @brief  IET Lock structures definition
  * @{
  */
typedef enum
{
    IET_UNLOCKED = 0x00U,
    IET_LOCKED   = 0x01U,
} IET_LockTypeDef;
/** @} */

#define __IET_LOCK(__DEVICE__)                                                \
                                do{                                           \
                                    if((__DEVICE__)->lock == IET_LOCKED)      \
                                    {                                         \
                                       return E_LOCKED;                      \
                                    }                                         \
                                    else                                      \
                                    {                                         \
                                       (__DEVICE__)->lock = IET_LOCKED;       \
                                    }                                         \
                                  }while (0U)

#define __IET_UNLOCK(__DEVICE__)                                              \
                                do{                                           \
                                    (__DEVICE__)->lock = IET_UNLOCKED;        \
                                }while (0U)

/**
 * \brief   IET AUDIO access type enum definition
 * \details define AUDIO_ACCESS_TYPE
 * @{
 */
typedef enum
{
    AUDIO_ACCESS_NONE,
    AUDIO_ACCESS_INT,
    AUDIO_ACCESS_DMA
} AUDIO_ACCESS_TYPE;
/** @} */
#define IS_AUDIO_ACCESS_TYPE(TYPE) (((TYPE) == AUDIO_ACCESS_INT) || ((TYPE) == AUDIO_ACCESS_DMA))

/**
 * \brief   IET AUDIO operation type enum definition
 * \details define AUDIO_OP_TYPE
 * @{
 */
typedef enum
{
    AUDIO_OP_NONE,
    AUDIO_OP_RECORD,
    AUDIO_OP_PLAY,
    AUDIO_OP_RP,
} AUDIO_OP_TYPE;
/** @} */
#define IS_AUDIO_OP_TYPE(TYPE) (((TYPE) == AUDIO_OP_RECORD) || ((TYPE) == AUDIO_OP_PLAY) || ((TYPE) == AUDIO_OP_RP))

/**
 * \brief   IET AUDIO channel mode enum definition
 * \details define AUDIO_CH_MODE
 * @{
 */
typedef enum
{
    AUDIO_CH_NONE,
    AUDIO_CH_LEFT,
    AUDIO_CH_RIGHT,
    AUDIO_CH_LR,
} AUDIO_CH_MODE;
/** @} */
#define IS_AUDIO_CH_MODE(MODE) (((MODE) == AUDIO_CH_LEFT) || ((MODE) == AUDIO_CH_RIGHT) || ((MODE) == AUDIO_CH_LR))

/**
 * \brief   IET AUDIO bclk mode enum definition
 * \details define AUDIO_BCLK_MODE
 * @{
 */
typedef enum
{
    AUDIO_BCLK_1024,
    AUDIO_BCLK_512,
} AUDIO_BCLK_MODE;
/** @} */
#define IS_AUDIO_BCLK_MODE(MODE) (((MODE) == AUDIO_BCLK_1024) || ((MODE) == AUDIO_BCLK_512))

/**
 * \brief   IET AUDIO i2s mode enum definition
 * \details define AUDIO_I2S_MODE
 * @{
 */
typedef enum
{
    AUDIO_I2S0,  /* no default connection hardware */
    AUDIO_I2S1,  /* no default connection hardware */
    AUDIO_I2S2,  /* default connect DSMDAC */
} AUDIO_I2S_CHANNEL;
/** @} */
#define IS_AUDIO_I2S_CHANNEL(CHANNEL) ((CHANNEL) == AUDIO_I2S0 || (CHANNEL) == AUDIO_I2S1 || (CHANNEL) == AUDIO_I2S2)

/**
 * \brief   IET AUDIO i2s Rx mode enum definition
 * \details define AUDIO_I2S_RX
 * @{
 */
typedef enum
{
    AUDIO_I2S0_RX,  /* no default connection hardware */
    AUDIO_I2S1_RX,  /* no default connection hardware */
} AUDIO_I2S_RX_CHANNEL;
/** @} */
#define IS_AUDIO_I2S_RX_CHANNEL(CHANNEL) ((CHANNEL) == AUDIO_I2S0_RX || (CHANNEL) == AUDIO_I2S1_RX)

/**
 * \brief   IET AUDIO i2s mode enum definition
 * \details define AUDIO_I2S_TX
 * @{
 */
typedef enum
{
    AUDIO_I2S0_TX,  /* no default connection hardware */
    AUDIO_I2S1_TX,  /* no default connection hardware */
    AUDIO_I2S2_TX,  /* default connect DSMDAC */
} AUDIO_I2S_TX_CHANNEL;
/** @} */
#define IS_AUDIO_I2S_TX_CHANNEL(CHANNEL) ((CHANNEL) == AUDIO_I2S0_TX || (CHANNEL) == AUDIO_I2S1_TX || (CHANNEL) == AUDIO_I2S2_TX)


/**
 * \brief   IET AUDIO codec name enum definition
 * \details define AUDIO_CODEC_NAME
 * @{
 */
typedef enum
{
    AUDIO_CODEC_DSMDAC = AUDIO_I2S2_TX,
    AUDIO_CODEC_ADC,
    AUDIO_CODEC_PDM,
} AUDIO_CODEC_NAME;
/** @} */
#define IS_AUDIO_CODEC_NAME(NAME) (((NAME) == AUDIO_CODEC_DSMDAC) || ((NAME) == AUDIO_CODEC_ADC) || ((NAME) == AUDIO_CODEC_PDM))

/**
 * \brief   IET MODULE role mode enum definition
 * \details define MODULE_ROLE_MODE
 * @{
 */
typedef enum
{
    MODULE_ROLE_MASTER,
    MODULE_ROLE_SLAVE,
} MODULE_ROLE_MODE;
/** @} */
#define IS_MODULE_ROLE_MODE(MODE) (((MODE) == MODULE_ROLE_MASTER) || ((MODE) == MODULE_ROLE_SLAVE))

/**
 * \brief   IET MODULE number val enum definition
 * \details define MODULE_NUMBER_VAL
 * @{
 */
typedef enum
{
    MODULE_NUMBER_0     = 0x0,
    MODULE_NUMBER_1     = 0x1,
    MODULE_NUMBER_2     = 0x2,
    MODULE_NUMBER_3     = 0x3,
    MODULE_NUMBER_4     = 0x4,
    MODULE_NUMBER_5     = 0x5,
    MODULE_NUMBER_6     = 0x6,
    MODULE_NUMBER_7     = 0x7,
    MODULE_NUMBER_8     = 0x8,
    MODULE_NUMBER_128   = 0x80,
    MODULE_NUMBER_MAX   = 0xffffffff,
} MODULE_NUMBER_VAL;
/** @} */
/**
 * \brief   IET pll clock val enum definition
 * \details define PLL_CLK_VAL
 * @{
 */
typedef enum
{
    PLL_CLK_24      = 24576000,
    PLL_CLK_50      = 50000000,
    PLL_CLK_80      = 80000000,
    PLL_CLK_40      = 40960000,
    PLL_CLK_81      = 81920000,
    PLL_CLK_86      = 86016000,
    PLL_CLK_90      = 90112000,
    PLL_CLK_92      = 92160000,
    PLL_CLK_98      = 98304000,
    PLL_CLK_104     = 104448000,
    PLL_CLK_106     = 106496000,
    PLL_CLK_110     = 110592000,
    PLL_CLK_114     = 114688000,
    PLL_CLK_116     = 116736000,
    PLL_CLK_122     = 122880000,
    PLL_CLK_131     = 131072000,
    PLL_CLK_135     = 135168000,
    PLL_CLK_139     = 139264000,
    PLL_CLK_147     = 147456000,
    PLL_CLK_155     = 155648000,
    PLL_CLK_163     = 163840000,
    PLL_CLK_172     = 172032000,
    PLL_CLK_184     = 184320000,
    PLL_CLK_196     = 196608000,
} PLL_CLK_VAL;
/** @} */

/**
 * \brief   IET qspi clock division enum definition
 * \details define QSPI_CLK_DIV
 * @{
 */
typedef enum
{
    QSPI_CLK_DIV_1        = 0,/*!< qspi_clk = pll_clk */
    QSPI_CLK_DIV_2IN3     = 1,/*!< qspi_clk = pll_clk*2/3 */
    QSPI_CLK_DIV_2        = 2,/*!< qspi_clk = pll_clk/2 */
    QSPI_CLK_DIV_3        = 3,/*!< qspi_clk = pll_clk/3 */
} QSPI_CLK_DIV;
/** @} */

/**
 * \brief   IET periph clock division enum definition
 * \details define PERIPH_CLK_DIV
 * @{
 */
typedef enum
{
    PERIPH_CLK_DIV_1        = 0,
    PERIPH_CLK_DIV_2        = 1,
    PERIPH_CLK_DIV_4        = 3,
} PERIPH_CLK_DIV;
/** @} */

/**
 * \brief   IET PRINT MSK DEFINE
 * \details define iet_print_mask_e
 * @{
 */
typedef enum
{
    IET_PRINT_NONE              = 0x0,
    IET_PRINT_ERROR             = 0x1,
    IET_PRINT_LOG               = 0x2,
    IET_PRINT_DRV_KEY           = 0x10,
    IET_PRINT_DRV_DEBUG         = 0x20,
    IET_PRINT_DRV_INFO          = 0x40,
    IET_PRINT_ALG_DBG           = 0x80,
    IET_PRINT_MIDWARE_KEY       = 0x100,
    IET_PRINT_MIDWARE_DEBUG     = 0x200,
    IET_PRINT_MIDWARE_INFO      = 0x400,
    IET_PRINT_APP_KEY           = 0x1000,
    IET_PRINT_APP_DEBUG         = 0x2000,
    IET_PRINT_APP_INFO          = 0x4000,
    IET_PRINT_UPGRADE_KEY       = 0x10000,
    IET_PRINT_UPGRADE_DEBUG     = 0x20000,
    IET_PRINT_UPGRADE_INFO      = 0x40000,
    IET_PRINT_USER_DEFINE       = 0x100000,
    IET_PRINT_DISABLE           = 0x80000000,
} iet_print_mask_e;
/** @} */

/**
 * \name   Macro Definitions for driver status
 * @{
 */
#define IET_MODULE_CLSED                (MODULE_NUMBER_0)
#define IET_MODULE_OPEND                (MODULE_NUMBER_1)
#define IET_MODULE_NOTRUN               (MODULE_NUMBER_0)
#define IET_MODULE_RUNNING              (MODULE_NUMBER_1)
#define IS_MODULE_VALID_VAL(VAL)        ((VAL) != NULL)
#define IS_MODULE_OPEND_FLAG(FLAG)      ((FLAG) == IET_MODULE_OPEND)
#define IS_MODULE_CLSED_FLAG(FLAG)      ((FLAG) == IET_MODULE_CLSED)
#define IS_MODULE_RUNNING_FLAG(FLAG)    ((FLAG) == IET_MODULE_RUNNING)
#define IS_MODULE_NOTRUN_FLAG(FLAG)     ((FLAG) == IET_MODULE_NOTRUN)
#define IS_MODULE_LENGTH_VAL(VAL)       ((VAL) != MODULE_NUMBER_0)
/** @} */

#define IET_COMMON_PRINTF_MASK_DEFAULT      (IET_PRINT_ERROR|IET_PRINT_LOG|IET_PRINT_DRV_KEY|IET_PRINT_MIDWARE_KEY|IET_PRINT_APP_KEY|IET_PRINT_UPGRADE_KEY);


/**
 * \brief
 * \details
 * @{
 */
typedef enum
{
    PROC_NONE  = 0,
    PROC_RISCV = 1,
    PROC_HIFI  = 2,
} PROC_ENUM;
/** @} */

/**
 * \brief   Device callback function typedef
 * \details This is usually used in device callback settings.
 */
typedef void (*INT_HANDLER)(void);


/**
 * \brief   Functional State Definition
 * \details define FunctionalState
 * @{
 */
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
/** @} */
#define PARAM_FUNCTIONALSTATE(State) ((State==DISABLE) || (State==ENABLE))

/**
 * \brief   Boolean Type definition
 * \details define Bool Type definition
 * @{
 */
typedef enum {FALSE = 0, TRUE = !FALSE} Bool;
/** @} */

/**
 * \brief   IET Common Device Buffer structure definition
 * @{
 */
typedef struct
{
    void *buf;      /*!< buffer pointer */
    void *rbuf;      /*!< read buffer from master pointer */
    void *wbuf;      /*!< write buffer to master pointer */
    uint32_t len;   /*!< buffer length in bytes */
    uint32_t min_len;   /*!< buffer length in bytes */
    volatile uint32_t ofs;  /*!< current offset in buffer */
} DEV_BUFFER;
/** @} */

/**
 * \brief Definitions for pad config
 * @{
 */
union PAD_CTRL
{
    uint32_t raw;
    struct
    {
        uint8_t pull_down: 1;   /*!< Pad control. Pull-down enable */
        uint8_t drive_str: 2;   /*!< Pad control. Drive strength */
        uint8_t pull_up: 1;     /*!< Pad control. Pull-up enable */
        uint8_t Schmitt: 1;     /*!< Pad control. Schmitt enable */
    } bits;
};
/** @} */

/**
 * \brief   IET system clock structure definition
 * @{
 */
typedef struct
{
    PLL_CLK_VAL pll_clk;  /*!< pll clock */
    uint16_t mcu_div;     /*!< mcu clock divider,muc clock = PLL_CLK_VAL/mcu_div,mcu_div recommend value 2^N (N:0-15) */
    uint8_t periph_div : 2;   /*!< periph clock divider,periph clock = muc clock / periph_div,periph_div = PERIPH_CLK_DIV */
    uint8_t qspi_div : 2;     /*!< periph clock divider,periph clock = pll_clk / periph_div,periph_div = QSPI_CLK_DIV */
} IET_SYSCLK_TypeDef;
/** @} */

/** @} */



#ifdef __cplusplus
extern "C" {
#endif
/**
 * \defgroup    DEVICE_IET_COMMON_FUN IET Common functions
 * \ingroup     DEVICE_IET_COMMON
 * \brief       Definitions for Common driver API functions
 * @{
 */

/**
  \fn           iet_read_dat32(uint32_t addr)
  \brief        read a data from addr
  \param[in]    addr
  \return       uint32_t data
*/
extern uint32_t iet_read_dat32(uint32_t addr);

/**
  \fn           iet_read_dat8(uint32_t addr)
  \brief        read a data from addr
  \param[in]    addr
  \return       uint8_t data
*/
extern uint8_t iet_read_dat8(uint32_t addr);

/**
  \fn           iet_write_dat32(uint32_t addr, uint32_t data);
  \brief        write a data from addr
  \param[in]    addr
  \param[in]    data
  \return       NULL
*/
extern void iet_write_dat32(uint32_t addr, uint32_t data);
/**
  \fn           iet_write_dat8(uint32_t addr, uint32_t data);
  \brief        write a data from addr
  \param[in]    addr
  \param[in]    data
  \return       NULL
*/
extern void iet_write_dat8(uint32_t addr, uint8_t data);

/**
  \fn           uint32_t iet_get_pll_clk()
  \brief        get pll clock
  \param[in]    NULL
  \param[in]    NULL
  \return       pll clock
*/
uint32_t iet_get_pll_clk();

/**
  \fn           uint32_t iet_get_sys_clk()
  \brief        get system clock
  \param[in]    NULL
  \param[in]    NULL
  \return       system clock
*/
uint32_t iet_get_sys_clk();

/**
   \fn           uint32_t iet_get_periph_clk()
    \brief        get periph clock
    \param[in]    NULL
    \param[in]    NULL
    \return       periph clock
  */
uint32_t iet_get_periph_clk();

/**
   \fn           uint32_t iet_get_qspi_clk()
    \brief        get qspi clock
    \param[in]    NULL
    \param[in]    NULL
    \return       qspi clock
  */
uint32_t iet_get_qspi_clk();

/**
  \fn           iet_printf(int32_t i4Mask, const char * format, ...)
  \brief        iet log printf function
  \param[0]     i4Mask print mask of iet_print_mask_e
  \param[1]     format each output item and variable parameter
  \retval       NONE
  \note[note0]  control by DEBUG_MSG
  \note[note1]  init iet_uart first
  \note[note2]  log max length reference MAX_LOG_LENGTH
*/
void iet_print(int32_t mask, const char * format, ...);

/**
  \fn           iet_print_check(int32_t mask)
  \brief        check print mask
  \param        mask print mask of iet_print_mask_e
  \retval       1 print enable, 0 print disable
*/
uint8_t iet_print_check(int32_t mask);

/**
  \fn           iet_print_set_mask(int32_t mask)
  \brief        set iet log print mask, mask==0 means set default value
  \param        mask print mask of iet_print_mask_e
  \retval       NONE
*/
void iet_print_set_mask(int32_t mask);

/**
  \fn           iet_print_get_mask(void)
  \brief        get iet log print mask
  \param        mask print mask of iet_print_mask_e
  \retval       log print mask value
*/
int32_t iet_print_get_mask(void);

/**
  \fn           iet_print_get_port(void)
  \brief        get iet log uart port
  \retval       driver log uart port
*/
uint8_t iet_print_get_port(void);

/**
  \fn           iet_print_set_prot(void)
  \brief        set iet log uart port
  \param        [IN] driver log uart port
  \retval       VOID
*/
void iet_print_set_port(uint8_t ui1_port);

/**
  \fn           iet_printf_enable(void)
  \brief        Enable iet_printf_*
  \return       NULL
*/
void iet_printf_enable(void);

/**
  \fn           iet_printf_disable(void)
  \brief        Disable iet_printf_*
  \return       NULL
*/
void iet_printf_disable(void);

/**
  \fn           iet_get_driver_version(uint8_t *pBuf, uint32_t ui4Len)
  \brief        get driver version
  \param[in]    ver pointer to driver version string
  \return       NULL
*/
int32_t iet_get_driver_version(uint8_t *pBuf, uint32_t ui4Len);

/**
  \fn           iet_ffvad_write_reg(uint32_t addr, uint32_t data)
  \brief        write ffvad register
  \param[in]    addr
  \param[in]    data
  \return       NULL
*/
void iet_ffvad_write_reg(uint32_t addr, uint32_t data);
/**
  \fn           iet_ffvad_read_reg(uint32_t addr)
  \brief        read ffvad register addr
  \param[in]    addr
  \return       uint32t data
*/
uint32_t iet_ffvad_read_reg(uint32_t addr);
/**
  \fn           iet_init_pll(IET_SYSCLK_TypeDef *sysclk_config)
  \brief        Initialization system clock
  \param[in]    sysclk_config Pointer to a IET_SYSCLK_TypeDef that contains
                        the configuration information for the system clock config.
                Examples config:
                APP     IET_SYSCLK_TypeDef sysclk = {PLL_CLK_131, 1, PERIPH_CLK_DIV_4, QSPI_CLK_DIV_1};
                APP     IET_SYSCLK_TypeDef sysclk = {PLL_CLK_184, 1, PERIPH_CLK_DIV_4, QSPI_CLK_DIV_2IN3};
                bootloader IET_SYSCLK_TypeDef sysclk = {PLL_CLK_131, 1, PERIPH_CLK_DIV_4, QSPI_CLK_DIV_2};
                standby IET_SYSCLK_TypeDef sysclk = {PLL_CLK_24, 64, PERIPH_CLK_DIV_4, QSPI_CLK_DIV_1};
  \return       RET_PLL_OK
  \return       RET_PLL_PAR ,system on 24MHz
*/
int32_t iet_init_pll(IET_SYSCLK_TypeDef *sysclk_config);

/**
  \fn           iet_qspi_flash_get_base_addr();
  \brief        get flash base address
  \param[in]    NULL
  \return       flash_base_addr
*/
uint32_t iet_qspi_flash_get_base_addr();

/**
 * \brief   IET ring block fifo structure definition
 * \details define iet_rbf
 */
struct iet_rbf
{
    uint8_t *buf;
    uint8_t *write_ptr;
    uint8_t *read_ptr;
    uint32_t buf_size;
    uint32_t block_size;
    uint16_t blk_max_num;
    uint16_t free_blk_num;
};


typedef struct iet_rbf *iet_rbf_t;

/**
  \fn           iet_rbf_init(iet_rbf_t rbf, uint8_t *buf, uint32_t blk_size, uint16_t blk_max_num)
  \brief        Initialization ring block fifo
  \param[in]    rbf rbf point to iet_rbf struct
  \param[in]    buf ring block fifo data buffer
  \param[in]    blk_size ring single block size
  \param[in]    blk_max_num ring block total number
  \return       int32_t
*/
int32_t iet_rbf_init(iet_rbf_t rbf, uint8_t *buf, uint32_t blk_size, uint16_t blk_max_num);
/**
  \fn           iet_rbf_push(iet_rbf_t rbf)
  \brief        return uint8_t address to push data
  \param[in]    rbf point to iet_rbf struct
  \return       uint8_t *
*/
uint8_t *iet_rbf_push(iet_rbf_t rbf);
/**
  \fn           iet_rbf_pop(iet_rbf_t rbf)
  \brief        return uint8_t address to push data
  \param[in]    rbf point to iet_rbf struct
  \return       uint8_t *
*/
uint8_t *iet_rbf_pop(iet_rbf_t rbf);
/**
  \fn           iet_rbf_reset(iet_rbf_t rbf)
  \brief        reset ring buffer
  \param[in]    rbf point to iet_rbf struct
  \return       NULL
*/
void iet_rbf_reset(iet_rbf_t rbf);


/** print [MSG][fun_name][line_num] fmt */
#define iet_printf_msg(fmt, ...) do {iet_print(IET_PRINT_LOG,       "[MSG][%s][%d] ", __FUNCTION__, __LINE__);iet_print(IET_PRINT_LOG, fmt, ##__VA_ARGS__);}while(0)
/** print [DBG][fun_name][line_num] fmt */
#define iet_printf_dbg(fmt, ...) do {iet_print(IET_PRINT_DRV_DEBUG, "[DBG][%s][%d] ", __FUNCTION__, __LINE__);iet_print(IET_PRINT_DRV_DEBUG, fmt, ##__VA_ARGS__);}while(0)
/** print [ERR][fun_name][line_num] fmt */
#define iet_printf_err(fmt, ...) do {iet_print(IET_PRINT_ERROR,     "[ERR][%s][%d] ", __FUNCTION__, __LINE__);iet_print(IET_PRINT_ERROR, fmt, ##__VA_ARGS__);}while(0)
/** print */
#define iet_printf(fmt, ...)     iet_print(IET_PRINT_LOG,       fmt, ##__VA_ARGS__)

/** @} */
#ifdef __cplusplus
}
#endif
/** @} */

#endif /* IET_COMMON_H */

/* --------------------------------- End Of File ------------------------------ */
