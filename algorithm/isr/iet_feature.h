/******************************************************************************
*  TJKJ IET FEATURE LIBRARY
*
*  Copyright (C) 2019-2020 IET Limited. All rights reserved.
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
*  @file     iet_feature.h
*  @brief    TJKJ IET FEATURE LIBRARY
*  @author   Hui.Zhang
*  @email    huizhang@intenginetech.com
*  @version  1.0
*  @date     2020-02-29
*  @license  GNU General Public License (GPL)
*
*------------------------------------------------------------------------------
*  Remark         : Description
*------------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*------------------------------------------------------------------------------
*  2020/02/29 | 1.0       | Hui.Zhang    | Create file
*
******************************************************************************/
#ifndef __IET_FEATURE_H__
#define __IET_FEATURE_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************* Include *************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_MODEL_NUM_SUPPORTED     2


/**
  \fn           isr_feature_init
  \brief        Init the feaure process for the model
  \param[in]    model_index   model index specified
  \return       Return valude defined in mid_commom.h 
*/
extern uint32_t isr_feature_init(int32_t model_index);


/**
  \fn           isr_feature_process
  \brief        process wav data to get feature bank, Mel M=64 supported only
                all models use same output data of this function
  \param[in]    pInputData   512 left wav data for one mic or 512(left) + 512(right) wav data for two mics in this order
  \prarm[in]    pOutputdata  fbank feature, 64*5 or 64*1 as per angle and mics
  \return       Return valude defined in mid_commom.h 
*/
extern uint32_t isr_feature_process(int16_t* pInputData, int16_t* pOutputdata);
extern uint32_t isr_feature_process_virtual_2mic(int16_t* pInputData, int16_t* pOutputdata);

/**
  \fn           isr_feature_bn
  \brief        Batch normalize the feature bank data
  \param[in]    model_index   model index specified
  \param[in]    pInputData    pointer to the output data of isr_feature_process
  \prarm[in]    pOutputdata   fbank data after batch normalization
  \prarm[in]    angle         specify the angle needed, -1 for all angle. 
                              The angle here must be same as that of isr_feature_process if the later isn't -1
  \return       Return valude defined in mid_commom.h 
*/
extern uint32_t isr_feature_bn(int32_t model_index, int16_t* pInputData, int16_t* pOutputdata);

/**
  \fn           isr_get_spl
  \brief        calculate the sound pressure level(SPL) of the input frame, support 521 sample/frame only
  \param[in]    pInputData    pointer to the input wave data, first 512 sample for left channel, then right channel
                              only the first 512 sample is needed for one-microphone usage
  \return       Return the    SPL of the two channel, low 16 bits for the left channel, high 16 bits for the right one
                              int16_t for each channel, only the low 16bit is valid if one-microphone is used
*/
extern uint32_t isr_get_spl(int16_t* pInputData);


#ifdef __cplusplus
}
#endif
/** @} */

#endif /* __IET_FEATURE_H__ */
