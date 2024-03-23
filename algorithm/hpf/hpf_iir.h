#ifndef _IIR_H_
#define _IIR_H_

#include "hpf_biquad.h"

#define IIR_BIQUADS_SIZE 2

// high pass filter enum
typedef enum {
    HP_FILTER_70HZ = 0,
    HP_FILTER_125HZ,
    HP_FILTER_180HZ,
    HP_FILTER_DISABLE,
} HP_TYPE_T;

// iir_biquads
extern int16_t iir_biquads_hp_70hz_fixed[IIR_BIQUADS_SIZE][5];
extern int16_t iir_biquads_hp_125hz_fixed[IIR_BIQUADS_SIZE][5];
extern int16_t iir_biquads_hp_180hz_fixed[IIR_BIQUADS_SIZE][5];

// iir_biquads gain
extern int16_t iir_biquads_g_hp_70hz_fixed;
extern int16_t iir_biquads_g_hp_125hz_fixed;
extern int16_t iir_biquads_g_hp_180hz_fixed;

// highpass filter
//extern biquad_z_t_fixed z[IIR_BIQUADS_SIZE];
void alg_hpf_fixed_init(void);
int32_t alg_hpf_fixed(int16_t *x, int32_t n_samples, HP_TYPE_T hp_type, int16_t *out);
#endif
