/*****************************************************************************
*  Uart Controller Driver Head File for CHOPIN.
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
*  @file     iet_uart.h
*  @brief    iet uart Controller Driver Head File
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
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  
*  
*----------------------------------------------------------------------------
*
*****************************************************************************/

#ifndef __IET_UART_H__
#define __IET_UART_H__

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"


/**
 * \defgroup	DEVICE_IET_UART	IET UART Driver Interface
 * \ingroup	DEVICE_IET_DEF
 * \brief	definitions for uart controller (\ref iet_uart.h)
 * \details	provide interfaces for uart driver to implement
 * @{
 *
 * \file
 * \brief	uart device definitions
 * \details	provide common definitions for uart device,
 * 	then software developer can develop uart driver
 * 	following this definitions, and the applications
 * 	can directly call this definition to realize functions
 */

/**
 * \defgroup	DEVICE_IET_UART_DEVCONFIG	IET UART Driver Config
 * \ingroup	DEVICE_IET_UART
 * \brief	contains definitions of uart driver config.
 * \details	this config will be used in user implemented code, which was called
 *     Device Driver Implement Layer for uart to realize in user code.
 * @{
 */

/**
 * \brief	IET UART driver config definition
 * @{
 */
#define UART_NUM   1

/* Possible interrupt IIR_MASK values */
#define IET_UART_IIR_MDM_STATUS			(0x00)
#define IET_UART_IIR_XMIT_EMPTY			(0x02)
#define IET_UART_IIR_DATA_AVAIL			(0x04)
#define IET_UART_IIR_LINE_STATUS		(0x06)
#define IET_UART_IIR_RX_TIMEOUT			(0x0C)
/** @} */

/**
 * \brief	IET UART number enum definition
 * \details	define UART_Num_TypeDef
 * @{
 */
typedef enum
{
	UART_NUM_0 = 0x00,
}UART_Num_TypeDef;
/** @} */
#define IS_UART_NUM(NUM) (((NUM) == UART_NUM_0))

/**
 * \brief	IET UART data len enum definition
 * \details	define UART_DataLen_TypeDef
 * @{
 */
typedef enum
{
    UART_DATA_LEN_5,
    UART_DATA_LEN_6,
    UART_DATA_LEN_7,
    UART_DATA_LEN_8,
}UART_DataLen_TypeDef;
/** @} */
#define IS_UART_DATALEN(DATALEN) (((DATALEN) == UART_DATA_LEN_5) || ((DATALEN) == UART_DATA_LEN_6) || ((DATALEN) == UART_DATA_LEN_7)|| ((DATALEN) == UART_DATA_LEN_8))

/**
 * \brief	IET UART stop bit enum definition
 * \details	define UART_StopBit_TypeDef
 * @{
 */
typedef enum
{
    UART_STOP_BIT_1,
    UART_STOP_BIT_2   = 0x4,
    UART_STOP_BIT_1D5 = 0x4,/*!< only used of UART_DATA_LEN_5*/
}UART_StopBit_TypeDef;
/** @} */
#define IS_UART_STOPBIT(STOPBIT) (((STOPBIT) == UART_STOP_BIT_1) || ((STOPBIT) == UART_STOP_BIT_2) || ((STOPBIT) == UART_STOP_BIT_1D5))

/**
 * \brief	IET UART parity enum definition
 * \details	define UART_Parity_TypeDef
 * @{
 */
typedef enum
{
    UART_PARITY_NONE  = (0x00),
    UART_PARITY_ODD   = (0x08), 
    UART_PARITY_EVEN  = (0x18),
    UART_PARITY_MARK  = (0x28),
    UART_PARITY_SPACE = (0x38),
}UART_Parity_TypeDef;
/** @} */
#define IS_UART_PARITY(PARITY) (((PARITY) == UART_PARITY_NONE) || ((PARITY) == UART_PARITY_ODD) || ((PARITY) == UART_PARITY_EVEN) || ((PARITY) == UART_PARITY_MARK) || ((PARITY) == UART_PARITY_SPACE))

/**
 * \brief	IET UART direction select enum definition
 * \details	define UART_Direction_TypeDef
 * @{
 */
typedef enum
{
    UART_TRANSMIT = 0x1,
    UART_RECEIVE  = 0x2,
}UART_Direction_TypeDef;
/** @} */
#define IS_UART_DIRECTION(DIR) (((DIR) == UART_TRANSMIT) || ((DIR) == UART_RECEIVE))

/**
 * \brief	IET UART break config enum definition
 * \details	define UART_Break_TypeDef
 * @{
 */
typedef enum
{
    UART_BREAK_SET,
    UART_BREAK_CLR, 
}UART_Break_TypeDef;
/** @} */
#define IS_BREAK_STAT(STAT) (((STAT) == UART_BREAK_SET) || ((STAT) == UART_BREAK_CLR))

/**
 * \brief	IET UART hardware flow control enum definition
 * \details	define UART_HWFC_TypeDef
 * @{
 */
typedef enum {
	UART_FC_NONE,	/*!< Non hardware flow control */
	UART_FC_RTS,	/*!< Request To Send */
	UART_FC_CTS,	/*!< Clear To Send */
	UART_FC_BOTH,	/*!< Both hardware flow control methods */
} UART_HWFC_TypeDef;
/** @} */
#define IS_HWFC_SET(HWFC) (((HWFC) == UART_FC_NONE) || ((HWFC) == UART_FC_RTS) || ((HWFC) == UART_FC_CTS) || ((HWFC) == UART_FC_BOTH))

/**
 * \brief	IET UART interrupt config structure definition
 * \details	define UART_Int_TypeDef
 * @{
 */
typedef struct {
    DEV_BUFFER *tx_buf;
    DEV_BUFFER *rx_buf;
    UART_Parity_TypeDef parity;
    uint32_t baudrate;
} UART_Int_TypeDef;
/** @} */

/**
 * \brief	IET UART transfer format config structure definition
 * \details	define UART_DPS_Format_TypeDef
 * @{
 */
typedef struct {
    uint32_t datalen;
    uint32_t stopbit;
    uint32_t parity;
} UART_DPS_Format_TypeDef;
/** @} */

/**
 * \brief	IET UART initialization structure definition
 * \details	define UART_Init_TypeDef
 * @{
 */
typedef struct {
    UART_DPS_Format_TypeDef *dps_format;
    uint32_t baudrate;
} UART_Init_TypeDef;
/** @} */
/** @} */

/**
 * \defgroup	DEVICE_IET_UART_CTRLCMD		IET UART Driver Io-Control Commands
 * \ingroup	DEVICE_IET_UART
 * \brief	Definitions for uart driver io-control commands
 * \details	These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure or value, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref RET_UART_OK,		Control device successfully
 * @{
 */
/** Define UART control commands for common usage */
#define DEV_SET_UART_SYSCMD(cmd)    DEV_SET_SYSCMD(DEV_SYS_CMDBSE|(cmd))

/* ++++ Common commands for UART Device ++++ */
/**
 * Set UART baudrate.
 * - Param type : uint32_t
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_SET_BAUD			DEV_SET_UART_SYSCMD(0)

/**
 * UART FLUSH_OUTPUT.
 * - Param type : NULL
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_FLUSH_OUTPUT		DEV_SET_UART_SYSCMD(1)

/**
 * Get UART how many bytes are available to receive.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_GET_RXAVAIL		DEV_SET_UART_SYSCMD(2)

/**
 * Set UART how many bytes space are available to transmit.
 * - Param type : uint32_t *
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_GET_TXAVAIL		DEV_SET_UART_SYSCMD(3)

/**
 * Set/Clear UART break condition.
 * - Param type : UART_Break_TypeDef
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_BREAK		        DEV_SET_UART_SYSCMD(4)

/**
 * Set UART dps_format "D/P/S(Data/Parity/Stop) format".
 * - Param type : UART_DPS_Format_TypeDef *
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_SET_DPS_FORMAT		DEV_SET_UART_SYSCMD(5)

/**
 * Set UART hardware flow control.
 * - Param type : UART_HWFC_TypeDef
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_SET_HWFC   		DEV_SET_UART_SYSCMD(6)

/**
 * Abort current interrupt receive/transmit operation if rx/rx interrupt enabled.
 * - Param type : UART_Direction_TypeDef
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_ABORT		        DEV_SET_UART_SYSCMD(7)

/**
 * Enable transmit or receive interrupt.
 * - Param type : UART_Direction_TypeDef
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_SET_INT		    DEV_SET_UART_SYSCMD(8)

/**
 * Disable transmit or receive interrupt.
 * - Param type : UART_Direction_TypeDef
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_CLR_INT		    DEV_SET_UART_SYSCMD(9)

/**
 * Set UART transmit success callback function.
 * - Param type : void *
 * - Param usage : void (*function_name)(void)
 * - Return value explanation : 
 */
#define UART_CMD_SET_TXCB		    DEV_SET_UART_SYSCMD(10)

/**
 * Set UART success success callback function.
 * - Param type : void *
 * - Param usage : void (*function_name)(void)
 * - Return value explanation : 
 */
#define UART_CMD_SET_RXCB		    DEV_SET_UART_SYSCMD(11)

/**
 * Set UART transfer error callback function.
 * - Param type : void *
 * - Param usage : void (*function_name)(uint32_t type, uint32_t code)
 * - Return value explanation : 
 */
#define UART_CMD_SET_ERRCB		    DEV_SET_UART_SYSCMD(12)

/**
 * Set UART transmit buffer via interrupt.
 * - Param type : DEV_BUFFER *
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_SET_TXINT_BUF		DEV_SET_UART_SYSCMD(13)

/**
 * Set UART receive buffer via interrupt.
 * - Param type : DEV_BUFFER *
 * - Param usage : NULL
 * - Return value explanation : 
 */
#define UART_CMD_SET_RXINT_BUF		DEV_SET_UART_SYSCMD(14)
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup	DEVICE_IET_UART_FUN IET uart driver functions
 * \ingroup	    DEVICE_IET_UART
 * \brief		API functions for IET uart driver
 * @{
 */

/**
  \fn          	uint32_t iet_uart_getready(UART_Num_TypeDef n_uart)
  \brief		whether get ready
  \param[in]	n_uart          number of uart.
  \return		1               get ready
  \return		0               not ready
*/
extern uint32_t iet_uart_getready(UART_Num_TypeDef n_uart);

/**
  \fn          	uint32_t iet_uart_putready(UART_Num_TypeDef n_uart)
  \brief		whether put ready
  \param[in]	n_uart          number of uart.
  \return		1               put ready
  \return		0               not ready
*/
extern uint32_t iet_uart_putready(UART_Num_TypeDef n_uart);

/**
  \fn          	uint8_t iet_serial_getc(UART_Num_TypeDef n_uart)
  \brief		get char val from uart
  \param[in]	n_uart          number of uart.
  \return		val             receive from uart
*/
extern uint8_t iet_serial_getc(UART_Num_TypeDef n_uart);

/**
  \fn          	uint32_t iet_serial_putc(UART_Num_TypeDef n_uart, uint8_t c)
  \brief		put char val to uart
  \param[in]	n_uart          number of uart.
  \param[in]	val             transmit to uart.
  \return		RET_UART_OK
*/
extern uint32_t iet_serial_putc(UART_Num_TypeDef n_uart, uint8_t c);

/**
  \fn          	uint32_t iet_serial_puts(UART_Num_TypeDef n_uart, char *s)
  \brief		put char vals to uart
  \param[in]	n_uart          number of uart.
  \param[in]	vals' pointer   transmit to uart.
  \return		RET_UART_OK
*/
extern uint32_t iet_serial_puts(UART_Num_TypeDef n_uart, char *s);

/**
  \fn          	uint32_t iet_uart_open(UART_Num_TypeDef n_uart, UART_Init_TypeDef *uart_initstruct)
           also uint32_t iet_uart_open(UART_Num_TypeDef n_uart) to use default config
  \brief		open uart
  \param[in]	n_uart          number of uart.
  \param[in]   	uart_initstruct uart initstruct.
  \return		RET_UART_OK
*/
extern uint32_t iet_uart_open(UART_Num_TypeDef n_uart, UART_Init_TypeDef *uart_initstruct);

/**
  \fn          	uint32_t iet_uart_close(UART_Num_TypeDef n_uart)
  \brief		close uart
  \param[in]	n_uart          number of uart.
  \return		RET_UART_OK
*/
extern uint32_t iet_uart_close(UART_Num_TypeDef n_uart);

/**
  \fn          	uint32_t iet_uart_control(UART_Num_TypeDef n_uart, uint32_t ctrl_cmd, void *param)
  \brief		uart control function
  \param[in]	n_uart          number of uart.
  \param[in]    ctrl_cmd		io-control command.
  \param[in]   	param			command params.
  \return		RET_UART_OK
*/
extern uint32_t iet_uart_control(UART_Num_TypeDef n_uart, uint32_t ctrl_cmd, void *param);

/**
  \fn           void iet_uart_data_send_isr(UART_Num_TypeDef n_uart,void *data_buf,uint32_t size);
  \brief        send data by dma
  \param[in]    n_uart          number of uart.
  \param[in]    void *data_buf  source data buf
  \param[in]    uint32_t size   data size
*/
extern void     iet_uart_data_send_isr(UART_Num_TypeDef n_uart,void *data_buf,uint32_t size);

/**
  \fn           void iet_uart_data_rec_isr (UART_Num_TypeDef n_uart,void *data_buf,uint32_t size);
  \brief        rec data by dma
  \param[in]    n_uart          number of uart.
  \param[in]    void *data_buf  dst data buf
  \param[in]    uint32_t size   data size
*/
extern void     iet_uart_data_rec_isr (UART_Num_TypeDef n_uart,void *data_buf,uint32_t size);

/** @} */
#ifdef __cplusplus
}
#endif
/** @} */

#endif /* __IET_UART_H__ */

