#ifndef FEATURE_2_MIC_H
#define FEATURE_2_MIC_H

#include <stdint.h>

#ifdef __cplusplus 
extern "C"
{ 
#endif  

#define silk_int16_MAX   0x7FFF                               /*  2^15 - 1 =  32767 */
#define silk_int16_MIN   ((short)0x8000)                 /* -2^15     = -32768 */
#define silk_int32_MAX   0x7FFFFFFF                           /*  2^31 - 1 =  2147483647 */
#define silk_int32_MIN   ((int)0x80000000)             /* -2^31     = -2147483648 */

#define silk_SAT16(a)                       ((a) > silk_int16_MAX ? silk_int16_MAX :      \
                                            ((a) < silk_int16_MIN ? silk_int16_MIN : (a)))

#define FFT_COMPLEX_LEN  (257u)
#define WEIGHT_MATRIX_Q  (10u)
#define ANGLE_NUM        (5u) 
#define MIC_WEIGHT_SIZE  (ANGLE_NUM*FFT_COMPLEX_LEN*4)

typedef struct _stComplex_int16_t
{
    int16_t real;
    int16_t image;
} stComplex_int16_t;

typedef struct _stComplex_int32_t
{
    int32_t real;
    int32_t image;
} stComplex_int32_t;

/*
 * 将双麦频域数据转换为5个角度的频域输出
 * @param, pLeftFFTVector: 输入， 左边mic的FFT变换，存储形式为 real0 image0 real1 image1 ... real_nFFTLen image_nFFTLen
 * @param, pRightFFTVector: 输入，右边mic的FFT变换，存储形式同上
 * @param, nFFTLen: 输入，FFT变换后的长度，默认为FFT_COMPLEX_LEN
 * @param, nQValue: 输入，FFT变换的Q值，输出数据与这个Q值相同
 * @param, pDstVector: 输出，5个角度的FFT顺序存储，angle0_real0 angle0_image0 angle0_real1 angle0_image1 .... angle4_real0 angle4_image0 .... 
*/
void extract_2_mic_FFT_vector( 
                                const int32_t* pLeftFFTVector, 
                                const int32_t* pRightFFTVector, 
                                const int32_t nFFTLen, 
                                const int32_t nQValue,
                                int32_t* pDstVector );

void complex_element_multiply_int16t_2_int16_t(
                                                const stComplex_int16_t* pSrc1,
                                                const stComplex_int16_t* pSrc2, 
                                                const int32_t nLen, 
                                                const int32_t nQValue,
                                                stComplex_int16_t* pDst );

void complex_element_multiply_int16t_2_int32_t(
                                                const stComplex_int16_t* pSrc1,
                                                const stComplex_int32_t* pSrc2, 
                                                const int32_t nLen, 
                                                const int32_t nQValue,
                                                stComplex_int32_t* pDst );

void complex_element_multiply_int32t_2_int32_t(
                                                const stComplex_int32_t* pSrc1,
                                                const stComplex_int32_t* pSrc2, 
                                                const int32_t nLen, 
                                                const int32_t nQValue,
                                                stComplex_int32_t* pDst );

#ifdef __cplusplus 
} 
#endif  

#endif