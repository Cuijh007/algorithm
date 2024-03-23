#include "hpf_iir.h"
#include <stdio.h>
/**
 *  High Pass Filter (Cutoff Frequency 70Hz)
 */   

int16_t iir_biquads_hp_70hz_fixed[IIR_BIQUADS_SIZE][5] = {
   {
      FLT2FIX(9.5047116279602051e-001),     // A2
      FLT2FIX(1.0000000000000000e+000),     // B2
      FLT2FIX(-1.9497343301773071e+000),     // A1
      FLT2FIX(-2.0000000000000000e+000),     // B1
      FLT2FIX(1.0000000000000000e+000)      // B0
  },

  {
      FLT2FIX(9.7918248176574707e-001),     // A2
      FLT2FIX(1.0000000000000000e+000),     // B2
      FLT2FIX(-1.9784348011016846e+000),     // A1
      FLT2FIX(-2.0000000000000000e+000),     // B1
      FLT2FIX(1.0000000000000000e+000)      // B0
  }
};
// iir_biquads gain
int16_t iir_biquads_g_hp_70hz_fixed  =  FLT2FIX(9.6472002401796153e-001);

/**
 *  High Pass Filter (Cutoff Frequency 125Hz) 
 */

// iir_biquads
int16_t iir_biquads_hp_125hz_fixed[IIR_BIQUADS_SIZE][5] = {
  {
     FLT2FIX(9.1326659917831421e-001),     // A2
     FLT2FIX(1.0000000000000000e+000),     // B2
     FLT2FIX(-1.9109619855880737e+000),     // A1
     FLT2FIX(-2.0000000000000000e+000),     // B1
     FLT2FIX(1.0000000000000000e+000)      // B0
  },

  {
     FLT2FIX(9.6313738822937012e-001),     // A2
     FLT2FIX(1.0000000000000000e+000),     // B2
     FLT2FIX(-1.9607727527618408e+000),     // A1
     FLT2FIX(-2.0000000000000000e+000),     // B1
     FLT2FIX(1.0000000000000000e+000)      // B0
  }
};
// iir_biquads gain
int16_t iir_biquads_g_hp_125hz_fixed  =  FLT2FIX(9.3787058805894630e-001);


/** 
 * High Pass Filter (Cutoff Frequency 180Hz) 
 */

// iir_biquads
int16_t iir_biquads_hp_180hz_fixed[IIR_BIQUADS_SIZE][5] = {
  {
     FLT2FIX(8.7749207019805908e-001),     // A2
     FLT2FIX(1.0000000000000000e+000),     // B2
     FLT2FIX(-1.8728035688400269e+000),     // A1
     FLT2FIX(-2.0000000000000000e+000),     // B1
     FLT2FIX(1.0000000000000000e+000)      // B0
  },

  {
     FLT2FIX(9.4736701250076294e-001),     // A2
     FLT2FIX(1.0000000000000000e+000),     // B2
     FLT2FIX(-1.9425040483474731e+000),     // A1
     FLT2FIX(-2.0000000000000000e+000),     // B1
     FLT2FIX(1.0000000000000000e+000)      // B0
  }
};

// iir_biquads gain
int16_t iir_biquads_g_hp_180hz_fixed  =  FLT2FIX(9.1176041060380186e-001);


/** 
 * High Pass Filter
 */
static biquad_z_t_fixed z[IIR_BIQUADS_SIZE];

void alg_hpf_fixed_init(void) {
   /* clear biquads */
   biquad_clear_fixed(z, IIR_BIQUADS_SIZE);
}

int32_t alg_hpf_fixed(int16_t *x, int32_t n_samples, HP_TYPE_T hp_type, int16_t *out) {
   int32_t i;
   int32_t output;

   /* calculate filter results */
   if(hp_type == HP_FILTER_DISABLE) {
      // Nothing to do
   }
   else if(hp_type == HP_FILTER_70HZ) {
      //  FILE *fp;
      // fp=fopen("fixed.csv","w");
      for (i = 0; i < n_samples; i++) {
         output = biquad_filter_fixed(x[i], iir_biquads_hp_70hz_fixed, z, IIR_BIQUADS_SIZE) *
                 iir_biquads_g_hp_70hz_fixed;
         
         output  = output >> Q_F_V;
         output  = SATURATE_T_16(output);
         out[i] = (int16_t)output;
         //  iet_print(IET_PRINT_ALG_DBG, "%d\n", x[i] );
         // fprintf(fp,"%d\n",output);
      }
      //  fclose(fp);
   }
   else if(hp_type == HP_FILTER_125HZ) {
      for (i = 0; i < n_samples; i++) {
         
         output = biquad_filter_fixed(x[i], iir_biquads_hp_125hz_fixed, z, IIR_BIQUADS_SIZE) *
                  iir_biquads_g_hp_125hz_fixed;
         output  = output >> Q_F_V;
         output  = SATURATE_T_16(output);
         out[i] = (int16_t)output;
         // iet_print(IET_PRINT_ALG_DBG, "%d\n", x[i] );
      }
   }
   else if(hp_type == HP_FILTER_180HZ) {
      for (i = 0; i < n_samples; i++) {
         
         output = biquad_filter_fixed(x[i], iir_biquads_hp_180hz_fixed, z, IIR_BIQUADS_SIZE) *
                  iir_biquads_g_hp_180hz_fixed;
         output  = output >> Q_F_V;
         output  = SATURATE_T_16(output);
         out[i] = (int16_t)output;
         // iet_print(IET_PRINT_ALG_DBG, "%d\n",  x[i] );
      }
   }

   return 0;
}
