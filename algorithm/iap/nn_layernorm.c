#include "nn_layernorm.h"


static int32_t iet_sqrt(uint32_t x)
{
	unsigned int rem = 0;
    int root = 0;
    int i;

    for (i = 0; i < 16; i++) {
        root <<= 1;
        rem <<= 2;
        rem += x >> 30;
        x <<= 2;

        if (root < rem) {
            root++;
            rem -= root;
            root++;
        }
    }

    return  (root >> 1);
}


int nn_layernorm(const Net_t *pNet, int model_index, const LayerParam_t *param, void* pBase){
    int32_t *input_sub = (int32_t*)(pBase + 0x4000*32);
	int32_t *output_addr = (int32_t*)(pBase + param->addr.outputAddr*32);
    int32_t *scale_addr = (int32_t*)(current_weight_base + param->addr.scaleAddr*64);
	int32_t *vector_addr = (int32_t*)(current_weight_base + param->addr.vectorAddr*64);
  
	type_un scale[128];
	type_un vector[128];
	iet_shell_memcpy32(&scale, scale_addr, 512);
	iet_shell_memcpy32(&vector, vector_addr, 512);
    int16_t ExtenInputC = 128;//((param->inputShape.C + 15)/16)*16;
	int truncate = param->truncate[0];	
    for(int h = 0; h < param->inputShape.H; h++){
        for(int w = 0; w < param->inputShape.W; w++){
            int64_t x_sum = 0;
			type_un x_sub[128];
            for(int c = 0; c < ExtenInputC; c++){
				x_sub[c].a = input_sub[c];         
                x_sum += (int64_t)(x_sub[c].b[0]*x_sub[c].b[0]);
				x_sum += (int64_t)(x_sub[c].b[1]*x_sub[c].b[1]);
            }
            int32_t x_var = (x_sum/256);
            int32_t sqrt_c = iet_sqrt(x_var+1);	/*1<<12*/
			//int32_t sqrt_i = (1<<24)/sqrt_c;	/*1<<12*/
			//iet_print(IET_PRINT_ALG_DBG, "w : %d, x_var :%d, sqrt_c : %d, sqrt_i : %d\r\n", w, x_var, sqrt_c, sqrt_i);
			int32_t tmp_div = sqrt_c*(1<<truncate);
            for(int c = 0; c < ExtenInputC; c++){
				type_un output;
				//scale.a = scale_addr[c];
				//vector.a = vector_addr[c];
				//output.b[0] = (int16_t)((int)((scale[c].b[0]*x_sub[c].b[0])/sqrt_c)/(1<<truncate) + vector[c].b[0]);//(int16_t)(((((int)(scale.b[0]*x_sub[c].b[0])>>12)*sqrt_i)>>14) + vector.b[0]);
				//output.b[1] = (int16_t)((int)((scale[c].b[1]*x_sub[c].b[1])/sqrt_c)/(1<<truncate) + vector[c].b[1]);;//(int16_t)(((((int)(scale.b[1]*x_sub[c].b[1])>>12)*sqrt_i)>>14) + vector.b[1]);
                output.b[0] = SAT_INT16T((int)((scale[c].b[0]*x_sub[c].b[0])/tmp_div) + vector[c].b[0]);
				output.b[1] = SAT_INT16T((int)((scale[c].b[1]*x_sub[c].b[1])/tmp_div) + vector[c].b[1]);
				*output_addr = output.a;
                output_addr++;
            }       
			input_sub += ExtenInputC;  
        }
    }
 

    return 0;
}
