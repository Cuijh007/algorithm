#include "nn_activation.h"
#include "nn_common.h"
#include "iet_math.h"
#include <stdint.h>
#include "iet_common.h"


int nn_softmax(const LayerParam_t *param, void *pBase)
{
    const int32_t* input_addr = (const int32_t*)(pBase+param->addr.inputAddr*32);
    int32_t* output_addr = (int32_t*)(pBase+param->addr.outputAddr*32);
    int16_t outputsize = param->outputShape.C;
    int32_t result_addr[512] = {0};
    int16_t times = (outputsize + 1)/2;
    int32_t ExtendOutputC = (param->outputShape.C + 15)/16 * 16/2;

    for(int h = 0; h < param->outputShape.H; h++)
    {
        for(int w = 0; w < param->outputShape.W; w++)
        {
            int16_t max_data = -32768;
            for(int index=0; index<times; index++)
            {
                int32_t tmp_data = input_addr[index];
                result_addr[index*2] = (int16_t)(tmp_data&0xFFFF);
                if(max_data < result_addr[index*2])
                {
                    max_data = result_addr[index*2];
                }
                result_addr[index*2+1] = 0;
                if(index*2+1 < outputsize){
                    result_addr[index*2+1] = (int16_t)((tmp_data>>16)&0xFFFF);
                    if(max_data < result_addr[index*2+1])
                    {
                        max_data = result_addr[index*2+1];
                    }
                }
            }

            int16_t inputQ = param->truncate[2];
            int16_t outputQ = param->truncate[3];
            int16_t shift_Q = 0;
            int32_t nSum = 0;

            if(inputQ >= 9)
            {
                shift_Q = inputQ - 9;
                int32_t add_num = - (max_data>>shift_Q);
                for(int index=0; index<outputsize; index++)
                {
                    result_addr[index] = (result_addr[index]>>shift_Q) + add_num;
                    if(result_addr[index] < -4095){
                        result_addr[index] = 0;
                    }else{
                        result_addr[index] = exp_table[-result_addr[index]];
                    }
                    nSum = SAT_INT32T(nSum + result_addr[index]);
                }
            }
            else
            {
                shift_Q = 9 - inputQ;
                int32_t add_num = - (max_data<<shift_Q);
                for(int index=0; index<outputsize; index++)
                {
                    result_addr[index] = (result_addr[index]<<shift_Q) + add_num;
                    if(result_addr[index] < -4096){
                        result_addr[index] = 1;
                    }else{
                        result_addr[index] = exp_table[-result_addr[index]];
                    }
                    nSum = SAT_INT32T(nSum + result_addr[index]);
                }
            }

            for(int index=0; index<ExtendOutputC; index++)
            {
                int16_t l16bits = 0;
                int16_t h16bits = 0;
                if(index*2 <  outputsize)
                {
                    l16bits = SAT_INT16T((result_addr[index*2]*(1<<outputQ))/nSum);
                }
                if (index*2+1 < outputsize)
                {
                    h16bits = SAT_INT16T((result_addr[index*2+1]*(1<<outputQ))/nSum);
                }
                output_addr[index] = (int32_t)l16bits&0x0000FFFF | ((int32_t)h16bits<<16)&0xFFFF0000 ;
            }

            input_addr += ExtendOutputC;
            output_addr += ExtendOutputC;
        }
    }
    //printf("bach1c_softmax\n");
    return 0;
}

/************************************************************
*Func: nn_exp
*input:Q9
*output:Q3
*
************************************************************/
int16_t left_16bit(int16_t data, int16_t quant)
{
    int16_t tmp_data = data >> (15-quant);
    if(tmp_data==(0) || tmp_data==(-1))
    {
        data <<= quant;
    }
    else
    {
        if(data>0)  data = 32767;
        if(data<0)  data = (-32768);
    }

    return data;
}


static int nn_softmax_mixmask(const LayerParam_t *param, void *pBase)
{
    const int32_t* input_addr = (const int32_t*)(pBase+param->addr.inputAddr*32);
    int32_t* output_addr = (int32_t*)(pBase+param->addr.outputAddr*32);
    int16_t outputsize = param->outputShape.C;
    int32_t result_addr[300] = {0};
    int32_t word_num = eos_offset_index + 1;
    int32_t ExtendOutputC = (param->outputShape.C + 15)/16 * 16/2;
    
    for(int h = 0; h < param->outputShape.H; h++){
        for(int w = 0; w < param->outputShape.W; w++){
            int16_t mix_mask = w+1;
            if(param->inputShape.W == 1){
                mix_mask = word_num;
            }
            if(mix_mask > word_num){
                mix_mask = word_num;
                input_addr += outputsize/2;
                output_addr += outputsize/2;
                continue;
            }
            int16_t times = (mix_mask + 1)/2;
            int16_t max_data = -32768;
            for(int index=0; index<times; index++)
            {
                int32_t tmp_data = input_addr[index];
                result_addr[index*2] = (int16_t)(tmp_data&0xFFFF);
                if(max_data < result_addr[index*2])
                {
                    max_data = result_addr[index*2];
                }
                result_addr[index*2+1] = 0;
                if(index*2+1 < mix_mask){
                    result_addr[index*2+1] = (int16_t)((tmp_data>>16)&0xFFFF);
                    if(max_data < result_addr[index*2+1])
                    {
                        max_data = result_addr[index*2+1];
                    }
                }
            }
            int16_t inputQ = param->truncate[2];
            int16_t outputQ = param->truncate[3];
            int16_t shift_Q = 0;
            int32_t nSum = 0;
            int32_t add_num=0;
            if(inputQ >= 9)
            {
                shift_Q = inputQ - 9;
                add_num = - (max_data>>shift_Q);
                for(int index=0; index<mix_mask; index++)
                {
                    result_addr[index] = (result_addr[index]>>shift_Q) + add_num;
                    if(result_addr[index] < -4095){
                        result_addr[index] = 0;
                    }else{
                        result_addr[index] = exp_table[-result_addr[index]];
                    }
                    nSum = SAT_INT32T(nSum + result_addr[index]);
                }
            }
            else
            {
                shift_Q = 9 - inputQ;
                add_num = - (max_data<<shift_Q);
                for(int index=0; index<mix_mask; index++)
                {
                    result_addr[index] = (result_addr[index]<<shift_Q) + add_num;
                    if(result_addr[index] < -4096){
                        result_addr[index] = 1;
                    }else{
                        result_addr[index] = exp_table[-result_addr[index]];
                    }
                    nSum = SAT_INT32T(nSum + result_addr[index]);
                }
            }
                        
            int16_t SingleOutputC = (mix_mask + 15)/16 * 16;
            for(int index=0; index<SingleOutputC/2; index++)
            {
                int16_t l16bits = 0;
                int16_t h16bits = 0;
                if(index*2 <  mix_mask)
                {
                    l16bits = SAT_INT16T((result_addr[index*2]*(1<<outputQ))/nSum);
                }
                if (index*2+1 < mix_mask)
                {
                    h16bits = SAT_INT16T((result_addr[index*2+1]*(1<<outputQ))/nSum);
                }
                output_addr[index] = (int32_t)l16bits&0x0000FFFF | ((int32_t)h16bits<<16)&0xFFFF0000 ;
            }
            input_addr += ExtendOutputC;
            output_addr += ExtendOutputC; 
        }
    }

    return 0;
}


#ifdef __x86_64
static int16_t SigmoidTable[2048] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 36, 36, 37, 37, 38, 38, 39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 76, 77, 78, 79, 81, 82, 83, 84, 86, 87, 88, 90, 91, 93, 94, 96, 97, 99, 100, 102, 103, 105, 107, 108, 110, 112, 114, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137, 139, 141, 144, 146, 148, 151, 153, 155, 158, 160, 163, 165, 168, 171, 173, 176, 179, 182, 184, 187, 190, 193, 196, 199, 202, 206, 209, 212, 215, 219, 222, 226, 229, 233, 237, 240, 244, 248, 252, 256, 260, 264, 268, 272, 276, 281, 285, 289, 294, 299, 303, 308, 313, 318, 323, 328, 333, 338, 343, 349, 354, 360, 365, 371, 377, 382, 388, 394, 401, 407, 413, 420, 426, 433, 440, 446, 453, 460, 467, 475, 482, 490, 497, 505, 513, 521, 529, 537, 545, 554, 562, 571, 580, 589, 598, 607, 617, 626, 636, 646, 656, 666, 676, 686, 697, 708, 719, 730, 741, 752, 764, 776, 788, 800, 812, 825, 837, 850, 863, 876, 890, 903, 917, 931, 946, 960, 975, 990, 1005, 1020, 1036, 1051, 1067, 1084, 1100, 1117, 1134, 1151, 1169, 1186, 1204, 1223, 1241, 1260, 1279, 1298, 1318, 1338, 1358, 1379, 1399, 1421, 1442, 1464, 1486, 1508, 1531, 1554, 1577, 1600, 1624, 1649, 1673, 1698, 1724, 1749, 1775, 1802, 1829, 1856, 1883, 1911, 1940, 1968, 1998, 2027, 2057, 2087, 2118, 2149, 2181, 2213, 2245, 2278, 2312, 2345, 2380, 2414, 2450, 2485, 2521, 2558, 2595, 2633, 2671, 2709, 2748, 2788, 2828, 2869, 2910, 2952, 2994, 3037, 3080, 3124, 3168, 3213, 3259, 3305, 3352, 3399, 3447, 3496, 3545, 3594, 3645, 3696, 3747, 3799, 3852, 3906, 3960, 4014, 4070, 4126, 4182, 4240, 4298, 4356, 4416, 4476, 4537, 4598, 4660, 4723, 4786, 4851, 4916, 4981, 5048, 5115, 5183, 5251, 5320, 5390, 5461, 5533, 5605, 5678, 5752, 5826, 5901, 5977, 6054, 6131, 6210, 6289, 6369, 6449, 6530, 6613, 6695, 6779, 6863, 6949, 7035, 7121, 7209, 7297, 7386, 7476, 7566, 7658, 7750, 7842, 7936, 8030, 8126, 8221, 8318, 8415, 8513, 8612, 8712, 8812, 8913, 9015, 9117, 9221, 9324, 9429, 9534, 9640, 9747, 9854, 9962, 10071, 10180, 10290, 10401, 10512, 10624, 10737, 10850, 10963, 11078, 11192, 11308, 11424, 11540, 11658, 11775, 11893, 12012, 12131, 12251, 12371, 12491, 12612, 12734, 12856, 12978, 13101, 13224, 13347, 13471, 13595, 13719, 13844, 13969, 14095, 14220, 14346, 14472, 14599, 14725, 14852, 14979, 15106, 15233, 15361, 15488, 15616, 15744, 15872, 16000, 16128, 16256, 16384, 16511, 16639, 16767, 16895, 17023, 17151, 17279, 17406, 17534, 17661, 17788, 17915, 18042, 18168, 18295, 18421, 18547, 18672, 18798, 18923, 19048, 19172, 19296, 19420, 19543, 19666, 19789, 19911, 20033, 20155, 20276, 20396, 20516, 20636, 20755, 20874, 20992, 21109, 21227, 21343, 21459, 21575, 21689, 21804, 21917, 22030, 22143, 22255, 22366, 22477, 22587, 22696, 22805, 22913, 23020, 23127, 23233, 23338, 23443, 23546, 23650, 23752, 23854, 23955, 24055, 24155, 24254, 24352, 24449, 24546, 24641, 24737, 24831, 24925, 25017, 25109, 25201, 25291, 25381, 25470, 25558, 25646, 25732, 25818, 25904, 25988, 26072, 26154, 26237, 26318, 26398, 26478, 26557, 26636, 26713, 26790, 26866, 26941, 27015, 27089, 27162, 27234, 27306, 27377, 27447, 27516, 27584, 27652, 27719, 27786, 27851, 27916, 27981, 28044, 28107, 28169, 28230, 28291, 28351, 28411, 28469, 28527, 28585, 28641, 28697, 28753, 28807, 28861, 28915, 28968, 29020, 29071, 29122, 29173, 29222, 29271, 29320, 29368, 29415, 29462, 29508, 29554, 29599, 29643, 29687, 29730, 29773, 29815, 29857, 29898, 29939, 29979, 30019, 30058, 30096, 30134, 30172, 30209, 30246, 30282, 30317, 30353, 30387, 30422, 30455, 30489, 30522, 30554, 30586, 30618, 30649, 30680, 30710, 30740, 30769, 30799, 30827, 30856, 30884, 30911, 30938, 30965, 30992, 31018, 31043, 31069, 31094, 31118, 31143, 31167, 31190, 31213, 31236, 31259, 31281, 31303, 31325, 31346, 31368, 31388, 31409, 31429, 31449, 31469, 31488, 31507, 31526, 31544, 31563, 31581, 31598, 31616, 31633, 31650, 31667, 31683, 31700, 31716, 31731, 31747, 31762, 31777, 31792, 31807, 31821, 31836, 31850, 31864, 31877, 31891, 31904, 31917, 31930, 31942, 31955, 31967, 31979, 31991, 32003, 32015, 32026, 32037, 32048, 32059, 32070, 32081, 32091, 32101, 32111, 32121, 32131, 32141, 32150, 32160, 32169, 32178, 32187, 32196, 32205, 32213, 32222, 32230, 32238, 32246, 32254, 32262, 32270, 32277, 32285, 32292, 32300, 32307, 32314, 32321, 32327, 32334, 32341, 32347, 32354, 32360, 32366, 32373, 32379, 32385, 32390, 32396, 32402, 32407, 32413, 32418, 32424, 32429, 32434, 32439, 32444, 32449, 32454, 32459, 32464, 32468, 32473, 32478, 32482, 32486, 32491, 32495, 32499, 32503, 32507, 32511, 32515, 32519, 32523, 32527, 32530, 32534, 32538, 32541, 32545, 32548, 32552, 32555, 32558, 32561, 32565, 32568, 32571, 32574, 32577, 32580, 32583, 32585, 32588, 32591, 32594, 32596, 32599, 32602, 32604, 32607, 32609, 32612, 32614, 32616, 32619, 32621, 32623, 32626, 32628, 32630, 32632, 32634, 32636, 32638, 32640, 32642, 32644, 32646, 32648, 32650, 32652, 32653, 32655, 32657, 32659, 32660, 32662, 32664, 32665, 32667, 32668, 32670, 32671, 32673, 32674, 32676, 32677, 32679, 32680, 32681, 32683, 32684, 32685, 32686, 32688, 32689, 32690, 32691, 32693, 32694, 32695, 32696, 32697, 32698, 32699, 32700, 32701, 32702, 32703, 32704, 32705, 32706, 32707, 32708, 32709, 32710, 32711, 32712, 32713, 32713, 32714, 32715, 32716, 32717, 32718, 32718, 32719, 32720, 32721, 32721, 32722, 32723, 32723, 32724, 32725, 32725, 32726, 32727, 32727, 32728, 32729, 32729, 32730, 32730, 32731, 32731, 32732, 32733, 32733, 32734, 32734, 32735, 32735, 32736, 32736, 32737, 32737, 32738, 32738, 32739, 32739, 32739, 32740, 32740, 32741, 32741, 32742, 32742, 32742, 32743, 32743, 32744, 32744, 32744, 32745, 32745, 32745, 32746, 32746, 32746, 32747, 32747, 32747, 32748, 32748, 32748, 32749, 32749, 32749, 32749, 32750, 32750, 32750, 32750, 32751, 32751, 32751, 32752, 32752, 32752, 32752, 32752, 32753, 32753, 32753, 32753, 32754, 32754, 32754, 32754, 32754, 32755, 32755, 32755, 32755, 32755, 32756, 32756, 32756, 32756, 32756, 32757, 32757, 32757, 32757, 32757, 32757, 32757, 32758, 32758, 32758, 32758, 32758, 32758, 32759, 32759, 32759, 32759, 32759, 32759, 32759, 32759, 32760, 32760, 32760, 32760, 32760, 32760, 32760, 32760, 32761, 32761, 32761, 32761, 32761, 32761, 32761, 32761, 32761, 32761, 32762, 32762, 32762, 32762, 32762, 32762, 32762, 32762, 32762, 32762, 32762, 32762, 32763, 32763, 32763, 32763, 32763, 32763, 32763, 32763, 32763, 32763, 32763, 32763, 32763, 32763, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32764, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32765, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, };
#else
int16_t SigmoidTable[20] = {0};
#endif
/*
 * the table is symmetrical by 1024(corresponded to 0 in X-axis), total 2048 sample, one half for the positive part, the other part for the negative part
 * formatting the input to Q1_9_6 firstly, found the value from the table if the value in +/-Q4_6, set to the max/min if out of this space
 * that's set the value to +/-1 if the value after fortting is out of [-15, 14.999]
 */
// sigmoid and tanh is done by NPU for bach2c,  the code here is unnecessary.
static int nn_sigmoid(const LayerParam_t *param, void *pBase)
{

    const int32_t* input_addr = (const int32_t*)(pBase+param->addr.inputAddr*32);
    int32_t* output_addr = (int32_t*)(pBase+param->addr.outputAddr*32);
    int32_t outputsize = param->inputShape.H * param->inputShape.W * param->outputShape.C;
    int16_t times = (outputsize+1)/2;
    int16_t InputQ = param->truncate[2];
    for(int index=0; index<times; index++)
    {
        int tmp = input_addr[index];
        int32_t lowdata = tmp&0xFFFF; // prevent overflow
        int32_t highdata = (tmp>>16)&0xFFFF;
        lowdata = (lowdata<<16)>>16;
        highdata = (highdata<<16)>>16;
        if(InputQ > 6)
        {
            lowdata = lowdata>>(InputQ - 6);
            highdata = highdata>>(InputQ - 6);
        }
        else
        {
            // lowdata = lowdata<<(6-InputQ);
            lowdata = left_16bit(lowdata, (8 - InputQ));
            // highdata = highdata<<(6-InputQ);
            highdata = left_16bit(highdata, (8 - InputQ));

        }
        if(lowdata + 1024 > 2047)
        {
            lowdata = 32767;
        }
        else if (lowdata + 1024 < 0)
        {
            lowdata = 0;
        }
        else
        {
            lowdata = SigmoidTable[lowdata+1024];
        }

        if(highdata + 1024 > 2047)
        {
            highdata = 32767;
        }
        else if (highdata + 1024 < 0)
        {
            highdata = 0;
        }
        else
        {
            highdata = SigmoidTable[highdata+1024];
        }
        if((index+1)*2 > outputsize)
        {
            highdata = 0;
        }
        output_addr[index] = (((int)lowdata & 0x0000FFFF) + (int)((((int)highdata) << 16 ) & 0xFFFF0000));
    }
    for(int index=times; index<ALIGN_16(outputsize)/2; index++)
    {
        output_addr[index] = 0;
    }

    return 0;
}


/*
 * the table is symmetrical by 1024(corresponded to 0 in X-axis), total 2048 sample, one half for the positive part, the other part for the negative part
 * formatting the input to Q1_7_8 firstly, found the value from the table if the value in +/-Q2_8, set to the max/min if out of this space
 * that's set the value to +/-1 if the value after fortting is out of [-4, 3.99609375],
 *  actual Q1_2_8 gen table by python
 */
static int nn_tanh(const LayerParam_t *param, void *pBase)
{

    const int32_t* input_addr = (const int32_t*)(pBase+param->addr.inputAddr*32);
    int32_t* output_addr = (int32_t*)(pBase+param->addr.outputAddr*32);
    int16_t outputsize = param->inputShape.H * param->inputShape.W * param->outputShape.C;
    int16_t times = (outputsize+1)/2;
    int16_t InputQ = param->truncate[2];
    for(int index=0; index<times; index++)
    {
        int tmp = input_addr[index];
        int16_t lowdata = tmp&0xFFFF;
        int16_t highdata = (tmp>>16)&0xFFFF;
        //iet_print(IET_PRINT_ALG_DBG, "l:%d, h:%d,  ", lowdata, highdata);
        if(InputQ > 8)
        {
            lowdata = lowdata>>(InputQ - 8);
            highdata = highdata>>(InputQ - 8);
        }
        else
        {
            // lowdata = lowdata<<(8 - InputQ);
            lowdata = left_16bit(lowdata, (8 - InputQ));
            // highdata = highdata<<(8 - InputQ);
            highdata = left_16bit(highdata, (8 - InputQ));
        }
        if(lowdata + 1024 > 2047)
        {
            // lowdata = 32767;
            lowdata = 32745;
        }
        else if (lowdata + 1024 < 0)
        {
            // lowdata = -32768;
            lowdata = -32767;
        }
        else
        {
            lowdata = TanhTable[lowdata+1024];
        }

        if(highdata + 1024 > 2047)
        {
            // highdata = 32767;
            highdata = 32745;
        }
        else if (highdata + 1024 < 0)
        {
            // highdata = -32768;
            highdata = -32767;
        }
        else
        {
            highdata = TanhTable[highdata+1024];
        }
        if(times*2 > outputsize)
        {
            highdata = 0;
        }
        //iet_print(IET_PRINT_ALG_DBG, "%d,%d\r\n", lowdata, highdata);
        output_addr[index] = (((int)lowdata & 0x0000FFFF) + (int)((((int)highdata) << 16 ) & 0xFFFF0000));
    }
    for(int index=times; index<ALIGN_16(outputsize)/2; index++)
    {
        output_addr[index] = 0;
    }
    //iet_print(IET_PRINT_ALG_DBG, "\r\n");
    return 0;
}


int nn_activation(const LayerParam_t *param, void* pBase)
{
    switch(param->activation)
    {
        case SOFTMAX:
            //iet_print(IET_PRINT_ALG_DBG, "activation: SoftMax\r\n");
            if(param->target_rms)
            {
                nn_softmax_mixmask(param, pBase);
            }
            else
            {
                nn_softmax(param, pBase);
            }
            break;            
        case SIGMOID:
            //iet_print(IET_PRINT_ALG_DBG, "activation: SigMoid\r\n");
            nn_sigmoid(param, pBase);
            break;
        case TANH:
            //iet_print(IET_PRINT_ALG_DBG, "activation: Tanh\r\n");
            nn_tanh(param, pBase);
            break;
        default:
            break;
    }

    return 0;
}
