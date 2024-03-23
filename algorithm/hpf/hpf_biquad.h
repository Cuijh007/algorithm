/****************************************************************************
*
* Name: biquad.h
*
* Description: Provides a template for implementing IIR filters as a cascade
* of second-order sections, aka, "biquads".
*
* by Xinke Zhang
*
*****************************************************************************/

#define BIQUAD_NCOEFF 5
#define BIQUAD_NZ     2

// #define Q_F_V (9)// 10<=Q_F_V <= 14, the hpf function well,Q_F_V == 9, Q20 saturate
#define Q_F_V (10)// 10<=Q_F_V <= 14, the hpf function well
#define Q_MUL (1<<Q_F_V)
#define Q_MIN (-1*(1<<(16-1)))
#define Q_MAX ((1<<(16-1))-1)
// #define Q_MIN_32 (-1*(1<<(32-1)))
// #define Q_MAX_32 ((1<<(32-1))-1)
#define Q_MAX_32 (2147483647)
#define Q_MIN_32 (-2147483648)
#define Q_MAX_16 (32767)
#define Q_MIN_16 (-32768)

#define FLT2FIX(x) ((x*Q_MUL)> (Q_MAX)? Q_MAX: (x*Q_MUL))<(Q_MIN)?(Q_MIN):(x*(Q_MUL))
#define SATURATE_T_32(x) ((x)> (Q_MAX_32)? Q_MAX_32: (x))<(Q_MIN_32)?(Q_MIN_32):(x)
#define SATURATE_T_16(x) ((x)> (Q_MAX_16)? Q_MAX_16: (x))<(Q_MIN_16)?(Q_MIN_16):(x)
// #define FLOAT
#define INT64 
// default int32
enum BIQUAD_INDICES {
    /* coefficient indices (change to suit) */
    BIQUAD_A2,
    BIQUAD_B2,     
    BIQUAD_A1,   
    BIQUAD_B1,
    BIQUAD_B0
};


#include <stdint.h>

#define Q20_MIN (-524288)
#define Q20_MAX (524287)

// #define Q20_MIN (-262144) //infact Q19
// #define Q20_MAX (262143)
//#define Q20_Saturation(x) ((x) > (Q20_MAX) ? (Q20_MAX) : ( (x) < (Q20_MIN) ? (Q20_MIN) : (x) ) )
void update_statistic_64_fixed(int64_t w, int64_t y, int32_t z);
typedef short BIQUAD_COEFFS_fixed[BIQUAD_NCOEFF];

typedef int32_t biquad_z_t_fixed[BIQUAD_NZ];

short biquad_filter_fixed(short x, const BIQUAD_COEFFS_fixed *p_coeffs,
                              biquad_z_t_fixed *p_z, int nbiquads);
void biquad_clear_fixed(biquad_z_t_fixed *p_z, int nbiquads);
