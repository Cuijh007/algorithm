/*****************************************************************************
*  QSPI Controller Driver for CHOPIN.
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
*  @file     iet_qspi.h
*  @brief    QSPI Controller Driver
*  @author   yang.fan
*  @email    yangfan@intenginetech.com
*  @version  1.0
*  @date     2020-01-09
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/01/09 | 1.0       | yang.fan       | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/
#ifndef __IET_QSPI_H_
#define __IET_QSPI_H_
#include "iet_common.h"

//DEBUG
//#define QSPI_DEBUG(format,...) iet_print(IET_PRINT_ALG_DBG, "%s %d: "format"", __func__,__LINE__, ##__VA_ARGS__)


typedef unsigned int  u32;
typedef int s32;
typedef unsigned char u8;
//typedef char s8;

//QSPI parameter
#define QSPI_FLASH_XFER_SIZE        256
#define QSPI_MAX_CMD_LEN            32
#define FLASH_SIZE                  0x800000
#define FLASH_SECTOR_SIZE           0x1000

#define QSPI_BASE_ADDR              0x51B00190

#define QSPI_NORFLASH_REMAP         0x10000000
#define QSPI_SPI_WR_FIFO_SIZE       256
#define QSPI_SPI_RD_FIFO_SIZE       64
//QSPI REG FLAG
#define QSPI_NOT_CMD                0
#define SET_SOFT_CTRL               0x01
#define CANCEL_SOFT_CTRL            0x02
//QSPI REG FUNC
#define SOFT_CTRL                   (1 << 31)
#define REG_SPI_WDATA_VLD           0x08
#define REG_SPI_RDATA_VLD           0x01
#define QSPI_AHB_MODE               0x00
#define QSPI_SPI_MODE               0x01

/*----------------------------------Nor Flash--------------------------------------*/
//QSPI NOR FLASH CMD
#define QSPI_NOR_FLASH_RDID         0x9030109F
#define QSPI_NOR_FLASH_RDSR1        0x90301005
#define QSPI_NOR_FLASH_RDSR2        0x90301035
#define QSPI_NOR_FLASH_RDCR1        0x90321035
#define QSPI_NOR_FLASH_RDCR2        0x90321015
#define QSPI_NOR_FLASH_RDCR3        0x90341033
#define QSPI_NOR_FLASH_RDAR         0x90005765
#define QSPI_NOR_FLASH_WRR          0x90001001
#define QSPI_NOR_FLASH_WRDI         0x90000004
#define QSPI_NOR_FLASH_WREN         0x90000006
#define QSPI_NOR_FLASH_WRENV        0x90000050
#define QSPI_NOR_FLASH_CLSR         0x90000030
#define QSPI_NOR_FLASH_READ         0x9FF45003
#define QSPI_NOR_FLASH_AHB_RD_S     0x0004570B    //?
#define QSPI_NOR_FLASH_AHB_RD_D     0x0004673B    //?
#define QSPI_NOR_FLASH_AHB_RD_Q     0x0004776B    //?
#define QSPI_NOR_FLASH_FSRD         0x9FF4570B
#define QSPI_NOR_FLASH_DOR          0x9FF4673B
#define QSPI_NOR_FLASH_QOR          0x9FF4776B

#define QSPI_NOR_FLASH_PP           0x9FFC5002
#define QSPI_NOR_FLASH_PP_AHB       0x000C5002    //?
#define QSPI_NOR_FLASH_QPP          0x9FFC7032
#define QSPI_NOR_FLASH_QPP_AHB      0x000C7032    //?
#define QSPI_NOR_FLASH_SE           0x900C4020
#define QSPI_NOR_FLASH_BE           0x900C40D8
#define QSPI_NOR_FLASH_CE           0x900000C7
#define QSPI_NOR_FLASH_WRSR1        0x90181001
#define QSPI_NOR_FLASH_WRSR2        0x90181031

//NOR FALSH CMD
#define NOR_FLASH_RDID              0x9F
#define NOR_FLASH_RDSR1             0x05
#define NOR_FLASH_RDSR2             0x07
#define NOR_FLASH_RDCR1             0x35
#define NOR_FLASH_RDCR2             0x15
#define NOR_FLASH_RDCR3             0x33
#define NOR_FLASH_RDAR              0x65
#define NOR_FLASH_WRR               0x01
#define NOR_FLASH_WRDI              0x04
#define NOR_FLASH_WREN              0x06
#define NOR_FLASH_WRENV             0x50
#define NOR_FLASH_CLSR              0x30
#define NOR_FLASH_READ              0x03
#define NOR_FLASH_FSRD              0x0B
#define NOR_FLASH_DOR               0x3B
#define NOR_FLASH_QOR               0x6B
#define NOR_FLASH_PP                0x02
#define NOR_FLASH_QPP               0x32
#define NOR_FLASH_SE                0x20
#define NOR_FLASH_BE                0xD8
#define NOR_FLASH_CE                0xC7
//NOR FLASH REG TYPE
#define CONFIG_REG                  0x01
#define CONFIG_STAT                 0x02

//qspi �Ĵ���
struct iet_qspi_reg
{
    u32 reg_base;
    u32 cmd;
    u32 soft_addr;
    u32 wdata;
    u32 wdata_vld;
    u32 rdata;
    u32 spi_done;
};

struct iet_qspi
{
    struct iet_qspi_reg *reg;
    const void *tx_buf;
    void *rx_buf;
    u32 len;
    u32 opt_flag;
    u8  exec_cmd;
};


extern int iet_qspi_flash_test(void);

extern void iet_qspi_wdata_push(void *data, u32 len);
extern void iet_qspi_set_rdvld(void);
extern void iet_wait_qspi_idle(void);
extern void iet_clr_qspi_rfifo(void);
extern void iet_qspi_send_cmd(u32 cmd);
extern void iet_qspi_set_addr(u32 addr);
extern void iet_qspi_soft_ctrl(u8 on);
extern void iet_nandflash_set_qe(void);
extern void iet_nandflash_rm_wp(void);
extern uint32_t  iet_flash_init(void);



#endif //__IET_QSPI_H_
