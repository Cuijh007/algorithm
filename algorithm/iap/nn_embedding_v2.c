#include "nn_embedding.h"
#include "nn_vpu_control.h"

#define LAYER_ID_EMBEDDING_GENRE    1
#define LAYER_ID_EMBEDDING_MOVIE_ID_RATE    2

#define MOVIE_GENRE_IN_DATA_LEN    32
#define MOVIE_ID_IN_DATA_LEN       10
#define MOVIE_RATE_IN_DATA_LEN     MOVIE_ID_IN_DATA_LEN
#define MOVIE_GENRE_EMBEDDING_DATA_LEN  4
#define MOVIE_ID_EMBEDDING_DATA_LEN     8

#define WEIGHT_DATA_BASE        (VPU_PSRAM_BASE)

// for movie recommendation example net
int nn_embedding_v2(const LayerParam_t *param, void* pBase, int16_t *p_in)
{
 	// int32_t *output_addr = (int16_t *)(pBase+param->addr.outputAddr*32);
    int32_t align_oc_div_2 = ALIGN_16(param->outputShape.C)>>1; // 2 sample for each
    int32_t oc_div_2 = param->outputShape.C>>1;
    int32_t *embedding_addr = (const int16_t*)(param->addr.scaleAddr*64+WEIGHT_DATA_BASE);
    int32_t *p_out = (int16_t *)(pBase+param->addr.outputAddr*32);;    
    if(param->layerId == LAYER_ID_EMBEDDING_GENRE)
    {
        for(int k = 0; k<MOVIE_GENRE_IN_DATA_LEN; k++)
        {
            int32_t *p_dst = p_out;
            int32_t embedding_index = p_in[k];
            if(embedding_index)
            {
                int32_t *p_src =  embedding_addr + embedding_index*oc_div_2;
                // read embedding part
                for(int i=0; i<MOVIE_GENRE_EMBEDDING_DATA_LEN/2; i++)
                {
                    *p_dst++ = *p_src++;
                }
                // set padding part to 0
                for(int i=MOVIE_GENRE_EMBEDDING_DATA_LEN/2; i<align_oc_div_2; i++ )
                {
                    *p_dst++ = 0;
                }
            }
            else
            {
                // set all to 0 for this input index
                for(int i=0; i<align_oc_div_2; i++)
                {
                    *p_dst++ = 0;
                }
            }
        }
        p_out += align_oc_div_2;
    }
    else
    {
        int16_t *p_id = p_in + MOVIE_GENRE_IN_DATA_LEN;
        int16_t *p_rating = p_id + MOVIE_ID_IN_DATA_LEN;
        for(int k = 0; k<MOVIE_ID_IN_DATA_LEN; k++)
        {
            int32_t *p_dst = p_out;
            int32_t embedding_index = p_id[k];
            if(embedding_index)
            {
                int32_t *p_src =  embedding_addr + embedding_index*8/2; //8: length of embedding data line, the outputShape.C is 8+1, 1 for moive rating
                // read embedding data of movie id
                for(int i=0; i<MOVIE_ID_EMBEDDING_DATA_LEN/2; i++)
                {
                    *p_dst++ = *p_src++;
                }
                // set movie rating data
                *p_dst++ = p_rating[k];
                // set 0 to the padding part
                for(int i=MOVIE_ID_EMBEDDING_DATA_LEN/2+1; i<align_oc_div_2; i++ )
                {
                    *p_dst++ = 0;
                }
            }
            else
            {
                for(int i=0; i<align_oc_div_2; i++)
                {
                    *p_dst++ = 0;
                }
            }
        }
        p_out += align_oc_div_2;      
    }

	return 0;
}