/******************************************************************************
*  TJKJ IET MATH LIBRARY
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
*  @file     iet_math.h
*  @brief    TJKJ IET MATH LIBRARY
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
#ifndef __IET_MATH_H__
#define __IET_MATH_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************* Include *************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/**
  \fn           void iet_fft(int32_t *pIntSrc , int32_t size)
  \brief        fixed fft function
  \param[in]    pIntSrc   data need to stored alternately according to real and img
  \return       NULL
*/
extern void iet_fft(int32_t *pIntSrc , int32_t size);


/**
  \fn           int32_t iet_log(uint32_t x)
  \brief        fixed base log function
  \param[in]    x   
  \return       (int)(log(x)*65535)
*/
extern int32_t iet_log(uint32_t x);


/**
  \fn           int32_t iet_sqrt(uint32_t x)
  \brief        fixed sqrt function
  \param[in]    x   
  \return       (int)(sqrt(x))
*/
extern int32_t iet_sqrt(uint32_t x);

/**
  \fn           int32_t iet_abscomplex(int32_t real, int32_t img, int32_t* shift_value)
  \brief        log function
  \param[in]    real
  \param[in]	  img
  \param[in]	  shift_value  if x<=0 : absresult left shift -shift_value, else: x>=0 absresult right shift shift_value
  \return       complex modular
*/
extern int32_t iet_abscomplex(int32_t real, int32_t img, int32_t* shift_value);

/**
  \fn           int32_t iet_mult(int32_t a, int32_t b, int32_t *shift_value)
  \brief        mult function
  \param[in]    a
  \param[in]    b
  \param[in]    shift_value if x<=0 : mult result left shift -shift_value, else: x>=0 mult result right shift shift_vale
  \return       mult result
*/
extern int32_t iet_mult(int32_t a, int32_t b, int32_t Qa, int32_t Qb, int32_t *shift_value);


#ifdef __cplusplus
}
#endif
/** @} */

#endif /* __NPU_CONTROL_H__ */