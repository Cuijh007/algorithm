/*****************************************************************************
*
* Name: biquad.c
*
* Description: Provides a template for implementing IIR filters as a cascade
* of second-order sections, aka, "biquads".
*
* by Xinke Zhang
*
******************************************************************************/

#include "hpf_biquad.h"
#include <stdio.h>

/******************************************************************************
* function: _filter
*
* Description:
*    Filters a single sample using a single biquad
*
* Parameters:
*    x           sample input
*    coeffs      biquad coefficients
*    z           delay line input/output
*
* Return Value:
*    Filtered sample output
******************************************************************************/
int64_t w_max_fixed,w_min_fixed,y_max_fixed,y_min_fixed;
int32_t z_max_fixed,z_min_fixed;
#ifdef INT64

// void update_statistic_64_fixed(int64_t w, int64_t y, int32_t z)
// {
//  w_max_fixed = w > w_max_fixed ? w : w_max_fixed;
//  w_min_fixed = w < w_min_fixed ? w : w_min_fixed;
//  y_max_fixed = y > y_max_fixed ? y : y_max_fixed;
//  y_min_fixed = y < y_min_fixed ? y : y_min_fixed;
//  z_max_fixed = z > z_max_fixed ? z : z_max_fixed;
//  z_min_fixed = z < z_min_fixed ? z : z_min_fixed;

// }

static inline int _filter_fixed(short x,
                                       const BIQUAD_COEFFS_fixed coeffs,
                                       biquad_z_t_fixed z)
{
    int64_t w, y ;
   
    w =  ((int64_t)(-z[0]) * coeffs[BIQUAD_A2])>>Q_F_V;
    // w = SATURATE_T_32(w);
    // update_statistic_64_fixed(w,y,z[0]);

    y =  ((int64_t)z[0] * coeffs[BIQUAD_B2])>>Q_F_V;
    // y = SATURATE_T_32(y);
    // update_statistic_64_fixed(w,y,z[0]);

    w -= ((int64_t)z[1] * coeffs[BIQUAD_A1])>>Q_F_V;
    // w = SATURATE_T_32(w);
    // update_statistic_64_fixed(w,y,z[1]);

    w +=  x;
    y +=  ((int64_t)z[1] * coeffs[BIQUAD_B1])>>Q_F_V;
    // y = SATURATE_T_32(y);
    // update_statistic_64_fixed(w,y,z[1]);

    // y +=   SATURATE_T_32(((int64_t)w * coeffs[BIQUAD_B0])>>Q_F_V);
    y +=   ((int64_t)w * coeffs[BIQUAD_B0])>>Q_F_V;
    // update_statistic_64_fixed(w,y,z[1]);

    z[0] = z[1];
    
    // z[1] = (int32_t)w;
    z[1] = w;
   
    return y;
}
#else
static inline int _filter_fixed(short x,
                                       const BIQUAD_COEFFS_fixed coeffs,
                                       biquad_z_t_fixed z)
{
    int32_t w, y ;
   
    w =  ((-z[0]) * coeffs[BIQUAD_A2])>>Q_F_V;
    // w = SATURATE_T_32(w);

    y =  (z[0] * coeffs[BIQUAD_B2])>>Q_F_V;
    // y = SATURATE_T_32(y);

    w -= (z[1] * coeffs[BIQUAD_A1])>>Q_F_V;
    // w = SATURATE_T_32(w);

    w +=  x;
    y +=  (z[1] * coeffs[BIQUAD_B1])>>Q_F_V;
    // y = SATURATE_T_32(y);

    y +=   (w * coeffs[BIQUAD_B0])>>Q_F_V;
    // y +=   SATURATE_T_32((w * coeffs[BIQUAD_B0])>>Q_F_V);
    z[0] = z[1];
    
    if ( z[0] < Q20_MIN )
        z[0] = Q20_MIN;
    if ( z[0] > Q20_MAX )
        z[0] = Q20_MAX;

    if ( w < Q20_MIN )
        w = Q20_MIN;
    if ( w > Q20_MAX )
        w = Q20_MAX;

    z[1] = w;
       
    return y;
}
#endif

/******************************************************************************/
short biquad_filter_fixed(short x, const BIQUAD_COEFFS_fixed *coeffs,
                              biquad_z_t_fixed *z, int nbiquads)
{
    int i;

    for (i = 0; i < nbiquads; i++) {
        x = _filter_fixed(x, coeffs[i], z[i]);
    }
   
    return x;
}

/******************************************************************************/
void biquad_clear_fixed(biquad_z_t_fixed *z, int nbiquads)
{
    int i, j;
   
    for (i = 0; i < nbiquads; i++) {
        for (j = 0; j < BIQUAD_NZ; j++) {
            z[i][j] = 0;
        }
     }
     z_max_fixed = 0;
     z_min_fixed = 0;
}
