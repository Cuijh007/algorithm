/*****************************************************************************
*  Spi Controller Driver Head File for CHOPIN.
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
*  @file     iet_spi.h
*  @brief    iet spi Controller Driver Head File
*  @author   yang.fan
*  @email    yangfan@intenginetech.com
*  @version  1.0
*  @date     2020-01-14
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/01/14 | 1.0       | Yang.fan       | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/
#ifndef __IET_SPI_H__
#define __IET_SPI_H__

#include "iet_common.h"

/**
 * \defgroup    DEVICE_IET_SPI  IET SPI Controller Interface
 * \ingroup DEVICE_IET_DEF
 * \brief   definitions for spi controller (\ref iet_spi.h)
 * \details provide interfaces for spi driver to implement
 * @{
 *
 * \file
 * \brief   spi controller definitions
 * \details provide common definitions for spi controller,
 *  then software developer can develop spi driver
 *  following this definitions, and the applications
 *  can directly call this definition to realize functions
 */

/**
 * \brief   IET SPI Status definition
 * @{
 */
#define SPI_BUSY        (1<<0)      /*!<  SSI Busy Flag */
#define SPI_TFNF        (1<<1)      /*!<  Transmit FIFO Not Full */
#define SPI_TFE         (1<<2)      /*!<  Transmit FIFO Empty */
#define SPI_RFNE        (1<<3)      /*!<  Receive FIFO Not Empty */
#define SPI_RFF         (1<<4)      /*!<  Receive FIFO Full */
#define SPI_TXE         (1<<5)      /*!<  Transmission Error */
#define SPI_DCOL        (1<<6)      /*!<  Data Collision Error */
/** @} */

/**
 * \brief   IET SPI Shift Register Loop enum definition
 * \details define SPI_SHIFT_LOOP_TypeDef
 * @{
 */
typedef enum
{
    SPI_LOOP_DISABLE,
    SPI_LOOP_ENABLE,
} SPI_SHIFT_LOOP_TypeDef;
/** @} */
#define IS_SPI_LOOP(MODE) (((MODE) == SPI_LOOP_DISABLE) || ((MODE) == SPI_LOOP_ENABLE))

/**
 * \brief   IET SPI Transfer Mode enum definition
 * \details define SPI_TRANSFER_MODE_TypeDef
 * @{
 */
typedef enum
{
    TRANSMIT_AND_RECEIVE,
    TRANSMIT_ONLY,
    RECEIVE_ONLY,
    EEPROM_READ,
} SPI_TRANSFER_MODE_TypeDef;
/** @} */
#define IS_SPI_TRANSFER_MODE(MODE) (((MODE) == TRANSMIT_AND_RECEIVE) || ((MODE) == TRANSMIT_ONLY) || ((MODE) == RECEIVE_ONLY) || ((MODE) == EEPROM_READ))

/**
 * \brief   IET SPI Serial Clock Polarity enum definition
 * \details define SPI_SCPOL_TypeDef
 * @{
 */
typedef enum
{
    SCLK_LOW,
    SCLK_HIGH,
} SPI_SCPOL_TypeDef;
/** @} */
#define IS_SPI_SCPOL(MODE) (((MODE) == SCLK_LOW) || ((MODE) == SCLK_HIGH))

/**
 * \brief   IET SPI Serial Clock Phase enum definition
 * \details define SPI_SCPH_TypeDef
 * @{
 */
typedef enum
{
    SCPH_MIDDLE,
    SCPH_START,
} SPI_SCPH_TypeDef;
/** @} */
#define IS_SPI_SCPH(MODE) (((MODE) == SCPH_MIDDLE) || ((MODE) == SCPH_START))

/**
 * \brief   IET SPI Frame Format enum definition
 * \details define SPI_FRF_TypeDef
 * @{
 */
typedef enum
{
    MOTOROLA_SPI,
} SPI_FRF_TypeDef;
/** @} */
#define IS_SPI_FRF(MODE) (((MODE) == MOTOROLA_SPI))

/**
 * \brief   IET SPI Data Frame Size enum definition
 * \details define SPI_DFS_TypeDef
 * @{
 */
typedef enum
{
    SERIAL_DATA_4_BIT = 0X3,
    SERIAL_DATA_5_BIT,
    SERIAL_DATA_6_BIT,
    SERIAL_DATA_7_BIT,
    SERIAL_DATA_8_BIT,
    SERIAL_DATA_9_BIT,
    SERIAL_DATA_10_BIT,
    SERIAL_DATA_11_BIT,
    SERIAL_DATA_12_BIT,
    SERIAL_DATA_13_BIT,
    SERIAL_DATA_14_BIT,
    SERIAL_DATA_15_BIT,
    SERIAL_DATA_16_BIT,
} SPI_DFS_TypeDef;
/** @} */
#define IS_SPI_DFS(DFS) (((DFS) >= SERIAL_DATA_4_BIT) && ((DFS) <= SERIAL_DATA_16_BIT))

/**
 * \brief   IET SPI Transfer Type enum definition
 * \details define SPI_Transfer_Type_TypeDef
 * @{
 */
typedef enum
{
    SPI_DEFAULT_TYPE,
    SPI_INT_TYPE,
    SPI_INT_WAIT_TYPE,
    SPI_DMA_TYPE,
    SPI_DMA_WAIT_TYPE,
} SPI_Transfer_Type_TypeDef;
/** @} */
#define IS_SPI_TRANS_TYPE(TYPE) (((TYPE) == SPI_DEFAULT_TYPE) || ((TYPE) == SPI_INT_TYPE) || ((TYPE) == SPI_INT_WAIT_TYPE) || ((TYPE) == SPI_DMA_TYPE) || ((TYPE) == SPI_DMA_WAIT_TYPE))

/**
 * \brief   IET SPI Transfer Type enum definition
 * \details define SPI_Transfer_Type_TypeDef
 * @{
 */
typedef enum
{
    SPI_CS_SOFT,
    SPI_CS_HARD,
} SPI_CS_Ctrl_TypeDef;
/** @} */
#define IS_SPI_CS(MODE) (((MODE) == SPI_CS_SOFT) || ((MODE) == SPI_CS_HARD))

/**
 * \brief   IET SPI Transfer Type enum definition
 * \details define SPI_Attribute_TypeDef
 * @{
 */
typedef enum
{
    SPI_MASTER,
    SPI_SLAVE,
} SPI_Attribute_TypeDef;
/** @} */
#define IS_SPI_ATT(MODE) (((MODE) == SPI_MASTER) || ((MODE) == SPI_SLAVE))

/**
 * \brief   IET SPI initialization structure definition
 * \details define SPI_Init_TypeDef
 * @{
 */
typedef struct
{
    uint8_t     srl;                /*!< Shift Register Loop */
    uint8_t     attribute;          /*!< SPI Attribute SPI_MASTER or SPI_SLAVE */
    uint8_t     tmod;               /*!< Transfer Mode */
    uint8_t     scpol;              /*!< Serial Clock Polarity */
    uint8_t     scph;               /*!< Serial Clock Phase */
    uint8_t     frf;                /*!< Frame Format */
    uint8_t     dfs;                /*!< Data Frame Size : default is 16bit */
    uint16_t    baudr;              /*!< Baud Rate Select : Divider of pclk: 2-65534 ; default is 2 */
    uint8_t     rsd;                /*!< Receive Data (rxd) Sample Delay : 4-6 ; defalut is 4 */
    uint8_t     csctrl;             /*!< Cs control: soft or hard */
    void*       callback_fun;       /*!< spi callback fun */
} SPI_Init_TypeDef;
/** @} */

/**
 * \defgroup    DEVICE_IET_SPI_CTRLCMD      IET SPI Driver Io-Control Commands
 * \ingroup DEVICE_IET_SPI
 * \brief   Definitions for spi driver io-control commands
 * \details These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure or value, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref RET_SPI_OK,     Control device successfully
 * @{
 */
/** Define SPI control commands for common usage */
#define DEV_SET_SPI_SYSCMD(cmd)   DEV_SET_SYSCMD(DEV_SYS_CMDBSE|(cmd))

/* ++++ Common commands for SPI Device ++++ */
/**
 * Set SPI interupt callback funtion.
 * - Param type : void *
 * - Param usage : void *funtion_name
 * - Return value explanation :
 */
#define SPI_CMD_ICB               DEV_SET_SPI_SYSCMD(0)

/**
 * Set SPI transfer mode.
 * - Param type : SPI_TRANSFER_MODE_TypeDef
 * - Param usage : NULL
 * - Return value explanation :
 */
#define SPI_CMD_TMOD              DEV_SET_SPI_SYSCMD(1)

/**
 * Set SPI transmit fifo threshold level,The maximum is 0xf.
 * - Param type : uint32_t
 * - Param usage : NULL
 * - Return value explanation :
 */
#define SPI_CMD_TXFTL             DEV_SET_SPI_SYSCMD(2)

/**
 * Set SPI receive fifo threshold level,The maximum is 0xf.
 * - Param type : uint32_t
 * - Param usage : NULL
 * - Return value explanation :
 */
#define SPI_CMD_RXFTL             DEV_SET_SPI_SYSCMD(3)

/**
 * Get SPI the number of valid data entries in the transmit fifo.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation :
 */
#define SPI_CMD_TXFL              DEV_SET_SPI_SYSCMD(4)

/**
 * Get SPI the number of valid data entries in the receive fifo.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation :
 */
#define SPI_CMD_RXFL              DEV_SET_SPI_SYSCMD(5)
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    DEVICE_IET_SPI_FUN IET spi driver functions
 * \ingroup     DEVICE_IET_SPI
 * \brief       API functions for IET spi driver
 * @{
 */

/**
  \fn           void iet_spi_senddata(uint16_t data)
  \brief        send data
  \param[in]    data
  \return       NULL
*/
extern void iet_spi_senddata(uint16_t data);


/**
  \fn           uint16_t iet_spi_receivedata(void)
  \brief        receive data
  \param[in]    NULL
  \return       data
*/
extern uint16_t iet_spi_receivedata(void);

/**
  \fn           uint16_t iet_spi_getstatus(void)
  \brief        get spi status
  \param[in]    NULL
  \return       spi status
*/
extern uint16_t iet_spi_getstatus(void);

/**
  \fn           uint32_t iet_spi_write_read(uint16_t *write_buf, uint16_t *read_buf, uint32_t size, SPI_Transfer_Type_TypeDef mode)
  \brief        send&receive data of spi
  \param[in]    send's buffer
  \param[in]    receive's buffer
  \param[in]    number of transfer
  \param[in]    transfer's type look at SPI_Transfer_Type_TypeDef
  \retval       RET_SPI_OK
*/
extern uint32_t iet_spi_write_read(uint16_t *write_buf, uint16_t *read_buf, uint32_t size, SPI_Transfer_Type_TypeDef mode);

/**
  \fn           uint32_t iet_spi_write(uint16_t *write_buf, uint32_t size, SPI_Transfer_Type_TypeDef mode)
  \brief        send data of spi
  \param[in]    send's buffer
  \param[in]    number of transfer
  \param[in]    transfer's type look at SPI_Transfer_Type_TypeDef
  \retval       RET_SPI_OK
*/
extern uint32_t iet_spi_write(uint16_t *write_buf, uint32_t size, SPI_Transfer_Type_TypeDef mode);

/**
  \fn           uint32_t iet_spi_read(uint16_t *read_buf, uint32_t size, SPI_Transfer_Type_TypeDef mode)
  \brief        receive data of spi
  \param[in]    receive's buffer
  \param[in]    number of transfer
  \param[in]    transfer's type look at SPI_Transfer_Type_TypeDef
  \retval       RET_SPI_OK
*/
extern uint32_t iet_spi_read(uint16_t *read_buf, uint32_t size, SPI_Transfer_Type_TypeDef mode);

/**
  \fn           uint32_t iet_spi_open(SPI_Init_TypeDef *SPI_InitStruct)
  \brief        open spi
  \param[in]    SPI_InitStruct      spi initstruct.
  \return       RET_SPI_OK
*/
extern uint32_t iet_spi_open(SPI_Init_TypeDef *SPI_InitStruct);

/**
  \fn           uint32_t iet_pdm_close(PDM_Num_TypeDef n_pdm)
  \brief        close spi
  \param[in]    NULL
  \return       RET_SPI_OK
*/
extern uint32_t iet_spi_close(void);

/**
  \fn           uint32_t iet_spi_control(uint32_t ctrl_cmd, void *param)
  \brief        spi control function
  \param[in]    ctrl_cmd        io-control command.
  \param[in]    param           command params.
  \return       RET_SPI_OK
*/
extern uint32_t iet_spi_control(uint32_t ctrl_cmd, void *param);

/** @} */
#ifdef __cplusplus
}
#endif
/** @} */

#endif /* __IET_SPI_H__ */

