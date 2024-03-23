/*****************************************************************************
*  Pdm Controller Driver Head File for Chopin.
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
*  @file     iet_pdm.h
*  @brief    iet pdm Controller Driver Head File
*  @author   yang.fan
*  @email    yangfan@intenginetech.com
*  @version  1.0
*  @date     2019-12-20
*  @license  GNU General Public License (GPL)
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2019/12/20 | 1.0       | yang.fan       | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/
#ifndef __IET_PDM_H__
#define __IET_PDM_H__

/* Includes ------------------------------------------------------------------- */
#include "iet_common.h"

typedef enum
{
    COEFF_70HZ,
    COEFF_125HZ,
    COEFF_180HZ,
} PDM_Coeff_TypeDef;

/**
 * \brief   IET PDM number enum definition
 * \details define PDM_Num_TypeDef
 * @{
 */
typedef enum
{
    PDM_NUM_0,
} PDM_Num_TypeDef;

/**
 * \brief   IET PDM config structure definition
 * \details define PDM_Config_TypeDef
 * @{
 */

typedef struct
{
    uint32_t   *rx_buffer;
    uint32_t    rx_len;             
    void       *pdm_callback;
} PDM_Config_TypeDef;
/** @} */


#define    HPF_ENABLE   0x01

#ifdef __cplusplus
extern "C" {
#endif

/**
  \fn           iet_pdm_get_ring_buf_index(PDM_Num_TypeDef n_pdm)
  \brief        get ring buf index
  \param[in]    n_pdm           number of pdm.
  \return       void
*/
uint8_t iet_pdm_get_ring_buf_index(PDM_Num_TypeDef n_pdm);

/**
  \fn           void iet_pdm_set_hpf_coeff(PDM_Coeff_TypeDef freq)
  \brief        set hpf coeff
  \param[in]    PDM_Coeff_TypeDef freq  coeff freq
  \return       void
*/
extern void iet_pdm_set_hpf_coeff(PDM_Coeff_TypeDef freq);

/**
  \fn           void iet_pdm_set_hpf_stat(uint8_t hpf_sw)
  \brief        set hpf ENABLE / DISABLE
  \param[in]    hpf_sw  hpf status
  \return       NULL
*/
extern void iet_pdm_set_hpf_stat(PDM_Num_TypeDef n_pdm, uint8_t hpf_sw);

/**
  \fn           uint32_t iet_pdm_open(PDM_Num_TypeDef n_pdm, PDM_Config_TypeDef* pdm_config)
  \brief        open pdm
  \param[in]    n_pdm           number of pdm.
  \param[in]    pdm_config      pdm initstruct.
  \return       RET_PDM_OK
*/
extern uint32_t iet_pdm_open(PDM_Num_TypeDef n_pdm, PDM_Config_TypeDef* pdm_config);

/**
  \fn           uint32_t iet_pdm_close(PDM_Num_TypeDef n_pdm)
  \brief        close pdm
  \param[in]    n_pdm           number of pdm.
  \return       RET_PDM_OK
*/
extern uint32_t iet_pdm_close(PDM_Num_TypeDef n_pdm);

/**
  \fn           uint32_t iet_pdm_start(PDM_Num_TypeDef n_pdm)
  \brief        start pdm transfer
  \param[in]    n_pdm           number of pdm.
  \return       RET_PDM_OK
*/
extern uint32_t iet_pdm_start(PDM_Num_TypeDef n_pdm);

/**
  \fn           uint32_t iet_pdm_stop(PDM_Num_TypeDef n_pdm)
  \brief        stop pdm transfer
  \param[in]    n_pdm           number of pdm.
  \return       RET_PDM_OK
*/
extern uint32_t iet_pdm_stop(PDM_Num_TypeDef n_pdm);

/**
  \fn           uint32_t iet_pdm_gain(PDM_Num_TypeDef n_pdm, uint16_t gain)
  \brief        config pdm gain
  \param[in]    n_pdm           number of pdm.
  \param[in]    gain            High eight bit mean integer, low eight bit means decimal fraction
  \return       RET_PDM_OK
*/
extern uint32_t iet_pdm_gain(PDM_Num_TypeDef n_pdm, uint8_t gain);


/** @} */
#ifdef __cplusplus
}
#endif
/** @} */

#endif /* __IET_PDM_H__ */

