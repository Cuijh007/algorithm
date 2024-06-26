#include "iet_feature.h"
#include "ISR_FiFo.h"
#include "Debug.h"
#include "feature_2_mic.h"
#include "iet_math.h"
#include "nn_file_struct.h"
#include "mid_common.h"
#include "iet_common.h"


#define DEBUG_FFT
#define DEBUG_FBANK
#define Q_VALUE 8
#define FMULI(a,b) ((a)*(b))
#define FSUBI(a,b) ((a)-((b)<<(Q_VALUE)))
#define FTOQ(x) (int32_t)((x)* (1<<Q_VALUE))
#define QUANTIZE(x) ((x) >> 18)
#define INV_QUANTIZE(x) ((x) << 7)
#define FDIVI(a,b) ((a)/(b))
#define FADD(a, b) ((a)+(b))
#define FSUB(a, b) ((a)-(b))
#define FMUL(a, b) ((int32_t)(((a)*(b))>>8))
#define FDIV(a, b) (((a)<<8)/b)
#define DIV(a) ((a) >> 10)

#define TOFIX(d) ((int32_t)( (d)*(float)(1<<(Q_VALUE)) ))  //float to fixed
#define TOFLT(a) ( (float)(a) / (float)(1<<(Q_VALUE)) )  //fixed to float


extern Net_t g_net;
extern NetInfo_t *g_p_net_info;

#define SAT_INT16T(x) ((x)>32767?32767:((x)<(-32768)?(-32768):(x)))
#define SAT_INT32T(x) ((x)>2147483647?2147483647:((x)<(-2147483648)?(-2147483648):(x)))


#define WIN_SIZE (512)

// struct 

typedef struct
{
	int16_t frameSize;      /* speech frameSize */
	int16_t numChans;       /* number of channels */
	int16_t sampPeriod;     /* sample period */
	int16_t fftN;
	int32_t x[513*2];       /* array[1..fftN] of fftchans */
}FBankInfo_FIXED;

typedef struct FeatureData {
    int16_t bn_scale[64];
    int32_t bn_vector[64];    
} FeatureData;

static int16_t micNumber = 1;
static int16_t micAngleForProcess = -1;


/*
 * weights to generate the 5 FFT vector
 * angle0_left_real0 angle0_left_image0 .... angle0_left_real256 angle0_left_image256, 
 * angle0_right_real0 angle0_right_image0 .... angle0_right_real256 angle0_right_image256
*/
int32_t *p_mic_weight_matrix;


/****************************Global Variable*********************/

FBankInfo_FIXED fb={
	.frameSize = 1024,
	.numChans = 26,
	.sampPeriod = 625,
	.fftN = 512,
};
// haming windows, multi by 1024, 1<<10
static int16_t hamWin[WIN_SIZE] = { 
    	 82,   82,   82,   82,   82,   83,   83,   84,   84,   85,   85,
         86,   87,   88,   89,   90,   91,   92,   93,   95,   96,   98,
         99,  101,  102,  104,  106,  108,  110,  112,  114,  116,  118,
        120,  122,  125,  127,  130,  132,  135,  138,  141,  143,  146,
        149,  152,  155,  158,  162,  165,  168,  172,  175,  178,  182,
        186,  189,  193,  197,  201,  204,  208,  212,  216,  220,  225,
        229,  233,  237,  242,  246,  250,  255,  259,  264,  268,  273,
        278,  282,  287,  292,  297,  302,  307,  312,  317,  322,  327,
        332,  337,  342,  347,  353,  358,  363,  368,  374,  379,  384,
        390,  395,  401,  406,  412,  417,  423,  428,  434,  440,  445,
        451,  457,  462,  468,  474,  479,  485,  491,  497,  502,  508,
        514,  520,  525,  531,  537,  543,  549,  554,  560,  566,  572,
        578,  583,  589,  595,  601,  606,  612,  618,  624,  629,  635,
        641,  646,  652,  658,  663,  669,  675,  680,  686,  691,  697,
        702,  708,  713,  719,  724,  730,  735,  740,  746,  751,  756,
        761,  766,  772,  777,  782,  787,  792,  797,  802,  807,  811,
        816,  821,  826,  831,  835,  840,  844,  849,  853,  858,  862,
        867,  871,  875,  879,  883,  888,  892,  896,  900,  903,  907,
        911,  915,  919,  922,  926,  929,  933,  936,  939,  943,  946,
        949,  952,  955,  958,  961,  964,  967,  970,  972,  975,  977,
        980,  982,  985,  987,  989,  991,  993,  995,  997,  999, 1001,
       1003, 1004, 1006, 1008, 1009, 1011, 1012, 1013, 1014, 1015, 1017,
       1018, 1018, 1019, 1020, 1021, 1021, 1022, 1022, 1023, 1023, 1024,
       1024, 1024, 1024, 1024, 1024, 1024, 1024, 1023, 1023, 1022, 1022,
       1021, 1021, 1020, 1019, 1018, 1018, 1017, 1015, 1014, 1013, 1012,
       1011, 1009, 1008, 1006, 1004, 1003, 1001,  999,  997,  995,  993,
        991,  989,  987,  985,  982,  980,  977,  975,  972,  970,  967,
        964,  961,  958,  955,  952,  949,  946,  943,  939,  936,  933,
        929,  926,  922,  919,  915,  911,  907,  903,  900,  896,  892,
        888,  883,  879,  875,  871,  867,  862,  858,  853,  849,  844,
        840,  835,  831,  826,  821,  816,  811,  807,  802,  797,  792,
        787,  782,  777,  772,  766,  761,  756,  751,  746,  740,  735,
        730,  724,  719,  713,  708,  702,  697,  691,  686,  680,  675,
        669,  663,  658,  652,  646,  641,  635,  629,  624,  618,  612,
        606,  601,  595,  589,  583,  578,  572,  566,  560,  554,  549,
        543,  537,  531,  525,  520,  514,  508,  502,  497,  491,  485,
        479,  474,  468,  462,  457,  451,  445,  440,  434,  428,  423,
        417,  412,  406,  401,  395,  390,  384,  379,  374,  368,  363,
        358,  353,  347,  342,  337,  332,  327,  322,  317,  312,  307,
        302,  297,  292,  287,  282,  278,  273,  268,  264,  259,  255,
        250,  246,  242,  237,  233,  229,  225,  220,  216,  212,  208,
        204,  201,  197,  193,  189,  186,  182,  178,  175,  172,  168,
        165,  162,  158,  155,  152,  149,  146,  143,  141,  138,  135,
        132,  130,  127,  125,  122,  120,  118,  116,  114,  112,  110,
        108,  106,  104,  102,  101,   99,   98,   96,   95,   93,   92,
         91,   90,   89,   88,   87,   86,   85,   85,   84,   84,   83,
         83,   82,   82,   82,   82,   82
};


//#if FIX_MODE  result * 4
static void PreEmphasise_Fixed(int32_t *s, int32_t k)
{
	int16_t i;
    int32_t tmp = s[WIN_SIZE-1];
    static int32_t last_value = 0;

	for (i = WIN_SIZE-1; i >= 1; i--)
	{
		s[i] -= (FMULI(k, s[i - 1]) >> 13);//FMUL(s[i - 1], preE);		
	}
    s[0] -= last_value;
    last_value = tmp;
}
//#endif

// result * 1024
static void Ham ( int32_t *s )
{
	int16_t i;
	for (i=0;i<512;i++)
	{
		s[i] = ((int32_t)hamWin[i]* s[i]);//FMUL( s[i], hamWin[i]);
	}
}


/*********************************************************************
*for 2 mics
*********************************************************************/

//for 2mics mel channel 64
static int8_t mel_channel[64] = {
     2,     2,     2,     2,     2,     2,     2,     3,     3,     2,     3,     3,     3,     4,     3,     3,
     4,     4,     3,     3,     5,     5,     4,     4,     5,     5,     5,     5,     5,     6,     6,     6,
     6,     7,     7,     7,     7,     7,     8,     8,     9,     9,     9,    10,    10,    10,    10,    11,
    12,    12,    12,    12,    13,    14,    14,    15,    15,    15,    17,    17,    17,    19,    19,    19
};

static int16_t mel_value_pos[493] = {
     4,     5,     5,     6,     6,     7,     7,     8,     8,     9,     9,    10,    10,    11,    11,    12,
    13,    12,    13,    14,    14,    15,    15,    16,    17,    16,    17,    18,    18,    19,    20,    19,
    20,    21,    22,    21,    22,    23,    23,    24,    25,    24,    25,    26,    27,    26,    27,    28,
    29,    28,    29,    30,    30,    31,    32,    31,    32,    33,    34,    35,    33,    34,    35,    36,
    37,    36,    37,    38,    39,    38,    39,    40,    41,    40,    41,    42,    43,    44,    42,    43,
    44,    45,    46,    45,    46,    47,    48,    49,    47,    48,    49,    50,    51,    50,    51,    52,
    53,    54,    52,    53,    54,    55,    56,    57,    55,    56,    57,    58,    59,    60,    58,    59,
    60,    61,    62,    63,    61,    62,    63,    64,    65,    66,    64,    65,    66,    67,    68,    69,
    70,    67,    68,    69,    70,    71,    72,    73,    71,    72,    73,    74,    75,    76,    77,    74,
    75,    76,    77,    78,    79,    80,    78,    79,    80,    81,    82,    83,    84,    81,    82,    83,
    84,    85,    86,    87,    88,    85,    86,    87,    88,    89,    90,    91,    92,    89,    90,    91,
    92,    93,    94,    95,    96,    97,    93,    94,    95,    96,    97,    98,    99,   100,   101,    98,
    99,   100,   101,   102,   103,   104,   105,   106,   102,   103,   104,   105,   106,   107,   108,   109,
   110,   111,   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,   112,   113,   114,   115,
   116,   117,   118,   119,   120,   121,   117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
   122,   123,   124,   125,   126,   127,   128,   129,   130,   131,   132,   127,   128,   129,   130,   131,
   132,   133,   134,   135,   136,   137,   138,   133,   134,   135,   136,   137,   138,   139,   140,   141,
   142,   143,   144,   139,   140,   141,   142,   143,   144,   145,   146,   147,   148,   149,   150,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,   151,   152,   153,   154,   155,
   156,   157,   158,   159,   160,   161,   162,   163,   157,   158,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
   174,   175,   176,   177,   171,   172,   173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
   183,   184,   185,   178,   179,   180,   181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
   191,   192,   186,   187,   188,   189,   190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
   200,   193,   194,   195,   196,   197,   198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
   208,   209,   201,   202,   203,   204,   205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
   215,   216,   217,   210,   211,   212,   213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
   223,   224,   225,   226,   218,   219,   220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
   230,   231,   232,   233,   234,   235,   236,   227,   228,   229,   230,   231,   232,   233,   234,   235,
   236,   237,   238,   239,   240,   241,   242,   243,   244,   245,   237,   238,   239,   240,   241,   242,
   243,   244,   245,   246,   247,   248,   249,   250,   251,   252,   253,   254,   255
};

static int16_t mel_value[493] = {
   858,   152,   871,   177,   846,   235,   788,   326,   697,   448,   575,   597,   426,   774,   249,   975,
   176,    48,   847,   423,   600,   691,   332,   979,   262,    44,   761,   587,   436,   929,   263,    94,
   760,   637,     2,   386,  1021,   404,   619,   821,   226,   202,   797,   668,    97,   355,   926,   562,
    14,   461,  1009,   501,   522,   998,   480,    25,   543,   996,   497,     7,    27,   526,  1016,   549,
    76,   474,   947,   634,   176,   389,   847,   750,   306,   273,   717,   894,   464,    41,   129,   559,
   982,   648,   237,   375,   786,   856,   457,    64,   167,   566,   959,   700,   317,   323,   706,   964,
   592,   224,    59,   431,   799,   885,   527,   174,   138,   496,   849,   849,   504,   164,   174,   519,
   859,   851,   519,   190,   172,   504,   833,   889,   569,   251,   134,   454,   772,   962,   651,   344,
    41,    61,   372,   679,   982,   765,   468,   174,   258,   555,   849,   907,   619,   334,    52,   116,
   404,   689,   971,   797,   521,   247,   226,   502,   776,  1000,   731,   466,   202,    23,   292,   557,
   821,   965,   707,   451,   197,    58,   316,   572,   826,   969,   720,   473,   228,    54,   303,   550,
   795,  1009,   769,   530,   294,    59,    14,   254,   493,   729,   964,   851,   620,   391,   164,   172,
   403,   632,   859,   963,   740,   518,   298,    80,    60,   283,   505,   725,   943,   888,   673,   460,
   249,    39,   135,   350,   563,   774,   984,   855,   648,   443,   239,    37,   168,   375,   580,   784,
   986,   860,   660,   462,   266,    71,   163,   363,   561,   757,   952,   901,   708,   517,   327,   139,
   122,   315,   506,   696,   884,   976,   789,   605,   421,   239,    57,    47,   234,   418,   602,   784,
   966,   901,   722,   545,   368,   192,    18,   122,   301,   478,   655,   831,  1005,   869,   696,   525,
   355,   186,    18,   154,   327,   498,   668,   837,  1005,   875,   709,   543,   379,   216,    54,   148,
   314,   480,   644,   807,   969,   917,   756,   597,   438,   280,   124,   106,   267,   426,   585,   743,
   899,   992,   837,   683,   529,   377,   225,    74,    31,   186,   340,   494,   646,   798,   949,   948,
   799,   651,   503,   356,   210,    65,    75,   224,   372,   520,   667,   813,   958,   944,   800,   657,
   515,   373,   232,    92,    79,   223,   366,   508,   650,   791,   931,   976,   838,   699,   562,   425,
   289,   154,    19,    47,   185,   324,   461,   598,   734,   869,  1004,   909,   775,   642,   510,   378,
   247,   117,   114,   248,   381,   513,   645,   776,   906,  1011,   882,   753,   625,   498,   371,   245,
   119,    12,   141,   270,   398,   525,   652,   778,   904,  1018,   894,   770,   646,   523,   401,   279,
   158,    37,     5,   129,   253,   377,   500,   622,   744,   865,   986,   941,   821,   702,   583,   465,
   347,   230,   113,    82,   202,   321,   440,   558,   676,   793,   910,  1021,   905,   790,   675,   560,
   446,   333,   220,   108,     2,   118,   233,   348,   463,   577,   690,   803,   915,  1019,   908,   797,
   686,   576,   466,   356,   247,   139,    31,     4,   115,   226,   337,   447,   557,   667,   776,   884,
   992,   947,   840,   733,   626,   520,   415,   309,   204,   100,    76,   183,   290,   397,   503,   608,
   714,   819,   923,  1020,   916,   813,   710,   607,   505,   403,   302,   201,   100
};


int16_t *p_bn_scale[MAX_MODEL_NUM_SUPPORTED];
int32_t *p_bn_vector[MAX_MODEL_NUM_SUPPORTED];

/*
 * pre-process input pcm data(in Q15) with emphasis, hamming window and fft
 * emphasis coef in Q13, hamming window coef in Q10
 * the out here is in Q(15+10)
 */
void ISR_Feature_Preprocess_64( const int16_t* pInput, const int32_t nLen, const int32_t micNumber, int32_t* pProcessedOut )
{
	int32_t i = 0;
	if(pInput == 0 || pProcessedOut == 0)
		return ;

	for( i=0; i<nLen; i++)
	{
		pProcessedOut[i] = (int32_t)(pInput[i]);
	}

    if(micNumber == 1)
    {
    	PreEmphasise_Fixed(pProcessedOut, 7946);
    }
	//Dual mic no PreEmphasise
	//PreEmphasise_Fixed(pProcessedOut, 7946); // 0.97*8192
	//iet_print(IET_PRINT_ALG_DBG, "Ham\r\n");
	Ham(pProcessedOut);
	//FFT 
	memset(fb.x, 0, sizeof(int32_t)*513*2);
	int32_t k = 0;

	for (; k<fb.frameSize; k++) 
	{
	    if((k%2) == 0)
	    {
    		fb.x[k] = pProcessedOut[k/2];   /* copy to workspace */
		}else{
    		fb.x[k] = 0;   /* pad with zeroes */
		}		
	}
	for (k=fb.frameSize; k<1024; k++) 
	{
		fb.x[k] = 0;   /* pad with zeroes */
	}
	//iet_print(IET_PRINT_ALG_DBG, "start fft\r\n");
	iet_fft( fb.x , 512);
	//iet_print(IET_PRINT_ALG_DBG, "fft over\r\n");
	//set output 
	for ( i = 0; i < FFT_COMPLEX_LEN*2; i++ )
	{
		pProcessedOut[i] = fb.x[i];
	}
}

/*
 * Get fbank
 * FFTout(Q25, integrate part is 0) -> power spetrum -> Mel spaced filterbank -> ln()
 * the out here is in Q10
 */
void ISR_Feature_GetFeatureBank_64( const int32_t* pFFTInput, int16_t *output)
{
	int16_t value_pos = 0;
	int16_t value_pos_tims = 0;
	for(int32_t cindex=0; cindex<64; cindex++)
	{
		int64_t fbank_feature = 0;
		for(int32_t cnum=0; cnum<mel_channel[cindex]; cnum++)
		{
			value_pos = mel_value_pos[value_pos_tims];
			int32_t real = (int32_t)pFFTInput[2*value_pos + 0];
			int32_t img  = (int32_t)pFFTInput[2*value_pos + 1];
			int32_t shift_v = 0;
			int32_t shift_r = 0;
			int32_t mag = iet_abscomplex(real, img, &shift_v);
			shift_v = 10 - shift_v;
			int64_t tmp_feature = (int64_t)iet_mult(mel_value[value_pos_tims], mag, 10, shift_v, &shift_r);			
			if(shift_r > 9)
			{
				fbank_feature += (tmp_feature>>(shift_r-9));
			}
			else
			{
				fbank_feature += (((int64_t)tmp_feature)<<(shift_r-9));
			}
			value_pos_tims++;
		}

		{
			int32_t t1 = SAT_INT32T(fbank_feature);
			if(t1 < 1)
			{
				t1 = 1;
			}
            // 90851 =  iet_log(1), 681391=iet_log(1<<15)-90851
            // '- 681391' should be used if wave pcm in space [-1, 1], the space is [-32768, 32767] for TF model now, [-1, 1] for torch
            // now it's done before feed to net if torch is used.
			output[cindex] = SAT_INT16T((iet_log(t1) - 90851/* - 681391*/)>>6);
		}
#if 0
		//output[cindex] = SAT_INT16T(((int64_t)output[cindex]*bn_scale[cindex]+bn_vector[cindex])>>10);
		int16_t tmp = output[cindex];
        		
		output[cindex] = SAT_INT16T(((int64_t)output[cindex]*p_bn_scale[0][cindex]+p_bn_vector[0][cindex])>>10);        
        iet_print(IET_PRINT_ALG_DBG, "output[%2d]: %d = (%d * %d + %d)>>10\n", cindex, output[cindex], tmp, p_bn_scale[0][cindex], p_bn_vector[0][cindex] );
#endif        
	}
}

/**
  \fn           BachFeatureProcess2Mics_64
  \brief        asr get 64*5 fbank
  \param[in]    pInputLeftData   512 left wav data
  \prarm[in]    pInputRightData  512 right wav data
  \prarm[out]   outputdata   fbank feature
  \prarm[in]    angle   angle expected
  \return       void 
*/
void BachFeatureProcess2Mics_64(const int16_t* pInputLeftData, const int16_t* pInputRightData, int16_t* pOutputdata, int32_t angle)
{
    static int32_t pProcessedLeftOut[514] = {0};
    static int32_t pProcessedRightOut[514] = {0};
    memset(pProcessedLeftOut, 0, sizeof(int32_t)*514);
    memset(pProcessedRightOut, 0, sizeof(int32_t)*514);

    static int32_t pFiveFFT[ANGLE_NUM*FFT_COMPLEX_LEN*2];

    //计算两个输入信号的FFT变换
    ISR_Feature_Preprocess_64( pInputLeftData, 512, 2, pProcessedLeftOut);
    ISR_Feature_Preprocess_64( pInputRightData, 512, 2, pProcessedRightOut);
    //转换到5个角度
    extract_2_mic_FFT_vector( pProcessedLeftOut, pProcessedRightOut, 514, 10, pFiveFFT );

    int16_t tmp_data[5*64] = {0};
    memset(tmp_data, 0, sizeof(int16_t)*5*64);
    if(angle == -1)
    {
        //循环提取FBANK
        for ( int32_t i = 0; i < ANGLE_NUM; i++ )
        {
            ISR_Feature_GetFeatureBank_64(pFiveFFT+FFT_COMPLEX_LEN*2*i, tmp_data+64*i);
        }
        //transpose
        for(int32_t i=0; i<64; i++)
        {
            for(int32_t j=0; j<5; j++)
            {
                pOutputdata[i*5+j] = tmp_data[i+j*64];
            }
        }
    }
    else
    {
        ISR_Feature_GetFeatureBank_64(pFiveFFT+FFT_COMPLEX_LEN*2*angle, pOutputdata);
    }
}


/**
  \fn           BachFeatureProcessOneMics_64
  \brief        asr get 64*1 fbank
  \param[in]    pInputLeftData   512 left wav data
  \prarm[in]    pInputRightData  512 right wav data
  \prarm[in]    outputdata   fbank feature
  \return       void 
*/
void BachFeatureProcessOneMics_64(const int16_t* pInputData, int16_t* pOutputdata)
{
    static int32_t pProcessedLeftOut[514] = {0};
    memset(pProcessedLeftOut, 0, sizeof(int32_t)*514);
    //计算两个输入信号的FFT变换
    ISR_Feature_Preprocess_64( pInputData, 512, 1, pProcessedLeftOut);
    //提取FBANK
    ISR_Feature_GetFeatureBank_64(pProcessedLeftOut, pOutputdata);
}

/*
 * Support 512 sample and melM=64 only now.
 * 
 *
 */
uint32_t isr_feature_process(int16_t* pInputData, int16_t* pOutputdata)
{    
    if(micNumber == 1)
    {
        BachFeatureProcessOneMics_64(pInputData, pOutputdata);
    }
    else
    {
        int16_t* pInputLeftData = pInputData;
        int16_t* pInputRightData = pInputData + 512;
        BachFeatureProcess2Mics_64(pInputLeftData,  pInputRightData, pOutputdata, micAngleForProcess);
    }
    return MID_FEATURE_OK;  
}


uint32_t isr_feature_process_virtual_2mic(int16_t* pInputData, int16_t* pOutputdata)
{    
    int16_t* pInputLeftData = pInputData;
    static int32_t pProcessedLeftOut[514] = {0};
    memset(pProcessedLeftOut, 0, sizeof(int32_t)*514);

    ISR_Feature_Preprocess_64(pInputLeftData, 512, 2, pProcessedLeftOut);
    ISR_Feature_GetFeatureBank_64(pProcessedLeftOut, pOutputdata);

    //transpose
    int16_t *pOut = pOutputdata + 64*5 - 1;
    for(int32_t i=63; i>=0; i--)
    {
        for(int32_t j=0; j<5; j++)
        {
            *pOut-- = pOutputdata[i];
        }
    } 

    return MID_FEATURE_OK;  
}


/*
 * Support 512 sample and melM=64 only now.
 * TODO: remove this function when all models integate bn layer
 */
uint32_t isr_feature_bn(int32_t model_index, int16_t* pInputData, int16_t* pOutputdata)
{
    int32_t cindex;
    int16_t *p_scale = p_bn_scale[model_index];
    int32_t *p_vector = p_bn_vector[model_index];

    int16_t *p_in = pInputData;
    int16_t *p_out = pOutputdata;
    int32_t angle = g_p_net_info->MicAngle[model_index];
   
    if(micNumber == 1 )
    {
        if(p_scale != NULL)
        {
            for(cindex=0; cindex<64; cindex++)
            {
                *p_out++ = SAT_INT16T(((int64_t)(*p_in) * (*p_scale) + (*p_vector))>>10);
                p_in++;
                p_scale++;
                p_vector++;
            }
        }
        else
        {
            for(cindex=0; cindex<64; cindex++)
            {
                *p_out++ = *p_in++;
            }
        }
    }
    else
    {
        if(micAngleForProcess == -1)
        {
            if(p_scale != NULL)
            {
                if(angle == -1)
                {
                    for(cindex=0; cindex<64; cindex++)
                    {
                        for(int32_t j=0; j<ANGLE_NUM; j++)
                        {
                            *p_out++ = SAT_INT16T(((int64_t)(*p_in) * (*p_scale) + (*p_vector))>>10);
                            p_in++;
                        }
                        p_scale++;
                        p_vector++;
                    }
                }
                else
                {
                    p_in += angle;
                    for(cindex=0; cindex<64; cindex++)
                    {
                        *p_out++ = SAT_INT16T(((int64_t)(*p_in) * (*p_scale) + (*p_vector))>>10);
                        p_in += ANGLE_NUM;
                        p_scale++;
                        p_vector++;
                    }
                }
            }
            else
            {
                if(angle == -1)
                {
                    for(cindex=0; cindex<64; cindex++)
                    {
                        for(int32_t j=0; j<ANGLE_NUM; j++)
                        {
                            *p_out++ = *p_in++;
                        }
                    }
                }
                else
                {
                    p_in += angle;
                    for(cindex=0; cindex<64; cindex++)
                    {
                        *p_out++ = *p_in++;
                    }
                }
            }
        }
        else
        {
            if(micAngleForProcess == angle)
            {
                if(p_scale != NULL)
                {
                    for(cindex=0; cindex<64; cindex++)
                    {
                        *p_out++ = SAT_INT16T(((int64_t)(*p_in) * (*p_scale) + (*p_vector))>>10);
                        p_in++;
                        p_scale++;
                        p_vector++;
                    }
                }
                else
                {
                    for(cindex=0; cindex<64; cindex++)
                    {
                        *p_out++ = *p_in++;
                    }
                }
            }
            else
            {
                iet_print(IET_PRINT_ERROR, "%s %d\r\n", __FUNCTION__, __LINE__);
                return MID_FEATURE_ERR;
            }
            
        }

    }
    return MID_FEATURE_OK;
  
}



uint32_t isr_feature_init(int32_t model_index)
{   
    Net_t *p_net =  &g_net;    
    NetInfo_t *p_net_info = g_p_net_info;

    if(model_index >= MAX_MODEL_NUM_SUPPORTED)
        return 1;

    for(int32_t i=0; i<p_net_info->NetNum; i++)
    {
        if(i==0)
        {
            micNumber = p_net_info->MicNumber[i];
            micAngleForProcess = p_net_info->MicAngle[i];
        }
        else
        {
            if(micNumber != p_net_info->MicNumber[i])
            {
                iet_print(IET_PRINT_ERROR, "%s %d\r\n", __FUNCTION__, __LINE__);
                return MID_FEATURE_ERR;
            }
            if(micAngleForProcess != p_net_info->MicNumber[i])
            {
                // When different angle is needed for models, process all angle set by ANGLE_NUM
                micAngleForProcess = -1;
            }
        }
    }   

    p_mic_weight_matrix = (int32_t*)p_net->pMicWeightMatrix;

    
    if(p_net_info->FeatureDataOffset[model_index] == 0)
    {
        p_bn_scale[model_index] = NULL;
        p_bn_vector[model_index] = NULL;
    }
    else
    {
        FeatureData *p_data;
        
        p_data = (FeatureData *)((uint8_t*)p_net->pPrivateData + p_net_info->FeatureDataOffset[model_index]);
        p_bn_scale[model_index] = &p_data->bn_scale[0];
        p_bn_vector[model_index] = &p_data->bn_vector[0];
    }

    return MID_FEATURE_OK;
}


/*
 * Cal the sound pressure for a channel of one frame, 512 sample only
 * spl = 20*log10(sqrt(sum(x^2)/N)) = 10*log10(sum(x^2)/N)
 *
 */
int32_t get_spl_512(int16_t* pInputData)
{
#define SPL_OUT_PRECISION   (0)
#define LOG_OUT_RECISION    (8)

    uint64_t sum  = 0;
    int16_t* p_data = pInputData;
    for(int i=0; i<512; i++)
    {
        sum += (*p_data) *(*p_data);
        p_data++;
    }
    sum >>= 9;      // sum/512, in Q30

    // ln(x) = (iet_log(y)-90581)/(2^16),  y = INT（x*(2^9))
    // 90851 = iet_log(1<<9) 
    // 953947 = iet_log(1<<(21+9)) - 90851, in Q16, that's:  ln（1<<21) = 953947(1<<16)
    // 21 + 9 is from sum in Q30

    // if w/o this special process and LOG_OUT_RECISION is over than 3, overflow will happen.
    // Use 64bit calculation will cause extral overload for cpu, no benefit for this feature
    if(sum<1)
    {
        sum = 1;
    }
    int32_t value =  (iet_log((uint32_t)sum) - 90851 - 953947)>>(16-LOG_OUT_RECISION);    // in Q8 (Q16 >> 8 -> Q8)
    value *= 445*10; // 445: 1/ln(10)*1024
    value = value + (1<<(10 + LOG_OUT_RECISION -SPL_OUT_PRECISION));
    value >>= (10 + LOG_OUT_RECISION -SPL_OUT_PRECISION);
    // printf("value >>= (10 + 8) = %d\n", value);
    return (value);
}

/*
 * Get the sound pressure of a frame.
 * Support 512 sample only now.
 *
 */
uint32_t isr_get_spl(int16_t* pInputData)
{   
    int16_t spl_l = 0;
    int16_t spl_r = 0;
    spl_l = SAT_INT16T(get_spl_512(pInputData));
    if(micNumber == 2)
    {
        spl_r = SAT_INT16T(get_spl_512( pInputData + 512));

    }

    return ((spl_r<<16)|spl_l&0xFFFF);
}
