#ifndef __FILE_STRUCT_H__
#define __FILE_STRUCT_H__

#define ALGORITHM_RELEASE	1
// #define DEBUG_NET_LAYER		1


#include <stdint.h>
#include <stddef.h>

#include "iet_common.h"

// at most one of the following should be valid only
// DON'T NEED enable them here, enable them when compile, such as: make LIB_TYPE=NLP_1CHP
//#define NLP_1CHIP				1
//#define NLP_2CHIP				1
//#define NON_NLP				1

#define MAX_MODEL_SUPPORTED     5               // max model number in the weight file/and netinfo structures
#define MAX_VER_INFO_SIZE       (64)            // version info szie
#define MAX_GRU_NUM_IN_MODEL    (4)             // max GRU number in a model

#define NLP_OUT_SIZE			(256)			// for NLP net(A105 + Intent), the final out is parsed from the net's last layer, not the original out

// when enable this, must make sure that app provide enough space in mid_pp_start() to hold the result
#define MAX_AUDIO_CH			(2)				// MUST change the related code if this value is changed
#define MAX_FRAME_SPL			(48)
#define SPL_DATA_SIZE			(2)				//  sound pressure data size in short
#define SPL_OUT_PRECISION   	(0)   			// decimals to keep


#ifdef NLP_1CHIP
typedef enum NetId
{
	NET_ID_WAKEUP   = 0,
	NET_ID_A105     = 1,	// first part of A105
	NET_ID_A105_2   = 2,
	NET_ID_A105_3   = 3,
	NET_ID_INTENT   = 4,
	NET_ID_NUM
}NetId;
#endif

#ifdef NLP_2CHIP
typedef enum NetId
{
	NET_ID_WAKEUP   = 1,
	NET_ID_A105     = 0,
	NET_ID_INTENT   = 1,
	NET_ID_NUM
}NetId;
#endif

#ifdef NON_NLP
// invalid one, just for compiling intent code for non nlp cases
typedef enum NetId
{
	NET_ID_WAKEUP   = 0,
	NET_ID_CMD      = 1,
	NET_ID_A105     = MAX_MODEL_SUPPORTED,
	NET_ID_INTENT   = MAX_MODEL_SUPPORTED,
	NET_ID_NUM
}NetId;
#endif


typedef enum MemoryType
{
	MEMORY_TYPE_PSRAM	= 0,
	MEMORY_TYPE_FLASH	= 1,
	MEMORY_TYPE_VMEM	= 2,
	MEMORY_TYPE_NUM
}MemoryType;


typedef enum ResultType
{
	RESULT_TYPE_VALID_CMD			= 0,    // valid cmd detected
	RESULT_TYPE_SKIP_NEXT_LAYERS    = 1,    // skip the layers after the current one
	RESULT_TYPE_SKIP_CURRENT_LAYER	= 2,	// skip the current layer for the current frame, continue to run the next layer
	RESULT_TYPE_NORMAL				= 3,    // no special process required
	RESULT_TYPE_NUM
}ResultType;

typedef enum ExraLabelType
{
	EXTRA_LABEL_TYPE_NONE = 0,
	EXTRA_LABEL_TYPE_UNK,
	EXTRA_LABEL_TYPE_EOS,
	EXTRA_LABEL_TYPE_NUM
}ExraLabelType;


enum BoardVersion{
	BACH1C = 0,
	BACH2C = 10,
};

enum LayerType
{
    Conv2D                  = 0,
    Dense                   = 1,
    NormalizeLayer          = 2,
    LSTM                    = 3,
    Activation              = 4,
    Mul                     = 5,
    RNNDenseAdd             = 6,
    RNNState                = 7,
    GRUSpecialTreatment     = 8,
    TimeDelay               = 9,
    Copy                    = 10,
    Add                     = 11,
    BN                      = 12,
    Reshape                 = 13,
    UserDefineWakeup        = 14,
    Concatenate             = 15,
    CustomFMDilation        = 16,
    Transpose               = 17,
    MatMul                  = 18,
    PositionalEncoding      = 19,
    LayerNorm               = 20,
    AddC                    = 21,
    UserDefineF2WPost       = 22,
    DecoderWordIds          = 23,
    Embedding               = 24,
    MixMask                 = 25,
    UserDefineBiDirectional = 26,
    UserDefineGather        = 27,
    UserDefineSlotPost      = 28,
    DepthwiseConv           = 29,
    ScalingMul              = 30,
    StridedSlice            = 31,
    LogSoftmax              = 32,
    UserDefineA105Post      = 33,
	UserDefineVADPost		= 34,
	UserDefineDummy			= 35,
	InitState				= 36,
    CustomEmbeddingV2       = 41,
    TopK                    = 42,

	TYPEMAX
};


enum ActType
{
	LINEAR = 0,
	SOFTMAX,
	SIGMOID,
	TANH,
	RELU,
	ACTMAX
};

enum PoolType
{
	NONE_POOL = 0,
	MAX,
	AVG
};


//TODO: update this as per real time delay working mode
enum TimeDelayType
{
	NORMAL_MODE = 0,
	INTERVAL_MODE,
	FIFO,
	EXTRACT_MODE
};


typedef struct Addr_s
{
	uint16_t inputAddr;
	uint16_t outputAddr;
	uint16_t kernelAddr;
	uint16_t biasAddr;
	uint16_t scaleAddr;
	uint16_t vectorAddr;
	uint16_t biasDestAddr;
	uint16_t saveResultAddr;
}Addr_t;

typedef struct MatrixShape_s
{
	int16_t H;
	int16_t W;
	int16_t C;
}MatrixShape_t;

typedef struct VpuPara_s
{
	int8_t 	vpu_layer;
	int8_t 	vpu_run;
	int16_t vpu_start_addr;
	int16_t vpu_end_addr;
}VpuPara_t;

typedef struct LayerParam_s
{
	uint16_t 			layerId;		    /*层索引*/
	MatrixShape_t 		inputShape;		    /*输入维度 HWC*/
	MatrixShape_t 		outputShape;	    /*网络输出维度 HWC*/
	MatrixShape_t 		kernelShape;	    /*卷积核维度 HWC*/
	//MatrixShape_t		convOutputShape;    /*卷积输出维度 HWC*/
	VpuPara_t			vpuPara;			/*vpu code 参数*/
	uint8_t  			strides[2];		    /*卷积操作步长 WH*/
	uint8_t				padding[2][2];	    /*补零方法*/
	uint8_t 			type;			    /*layer 类型*/
	uint8_t 			activation;		    /*激活函数类型*/
	uint16_t			target_rms;		    /*Normalize 参数*/
	uint8_t  			use_b;			    /*是否使用偏置*/
	uint8_t  			use_s;			    /*是否使用scale*/
	uint8_t  			use_v;			    /*是否使用vector*/
	uint8_t 			use_b8;			    /*是否使用8 bit*/
	uint8_t 			model_q;		    /*是否使用q模式*/
    int8_t              axis;               /*本层的操作维度*/
	int8_t  			truncate[6];	    /*截断方式*/
	int16_t				inputLayerId[4]; 	/*输入的层索引*/
	int16_t				outputLayerId[4];  /*以本层作为输入的层索引*/
	uint8_t 			inputLayerSize; 	/*输入的层数*/
	uint8_t 			outputLayerSize;	/*以本层作为输入的层数*/
	uint8_t				timesType;   	    /*time delay, work type */
	uint8_t 			timesize;		    /*time delay sizes*/
	Addr_t    			addr;				/*预先分配的内存使用地址信息*/
	uint8_t				saveFlag;
	/*for pooling layer*/
	uint8_t 			poolingType;
	uint8_t 			poolingSize[2];
	//uint8_t 			poolingStrides[2];
	//uint8_t 			poolingPadding[2][2];
    uint32_t            userDefine[4];      /*private static data area(aligned with 4bytes), used to store static variables, 
                                              don't use it out of space*/
    uint16_t            extraDataOffset;    /*offset of private data area to store the paramters from the float model (aligned with 4bytes), 
                                              related to the start address of NetInfo_t, must use it as per the defintion agreed on*/
}LayerParam_t;

typedef struct SpottingWord_s
{
	short index;
	short threshold_0;  // Two thresholds are needed for some net, such as K002
	short threshold_1;
	char  hanzi[120];
}SpottingWord_t;

typedef struct NetInfo_s
{
	char            Version[MAX_VER_INFO_SIZE]; 	            /*Info about weight file generation*/
	uint16_t		BoardVersion;								/*bach1c or bach2c ...*/
	uint16_t        NetNum;  						            /*Net number included in this structure*/
	uint16_t        LayersNum;			                        /*Layer number of this net*/	
	uint16_t        NetLayersStartId[MAX_MODEL_SUPPORTED];      /*Start index in Layers[] of this net*/
	uint16_t        NetLayersEndId[MAX_MODEL_SUPPORTED];        /*End index in Layers[] of this net*/
    int16_t         MicNumber[MAX_MODEL_SUPPORTED];             /*Mic number used by the model*/
    int16_t         MicAngle[MAX_MODEL_SUPPORTED];              /*Angle used by the model, -1 for all direction, 0 for the first left one*/
    int16_t         FrameLength[MAX_MODEL_SUPPORTED];           /*a frame length in ms*/
    int16_t         FrameOffset[MAX_MODEL_SUPPORTED];           /*Frame interval in ms*/
    int16_t         MinCmdLength[MAX_MODEL_SUPPORTED];          /*Minimum command length in Chinese characters*/
    uint16_t        FeatureDataOffset[MAX_MODEL_SUPPORTED];     /*Offset of the feature private data, related to the start address of this structure*/
    uint16_t        PostDataOffset[MAX_MODEL_SUPPORTED];        /*Offset of the post processing private data, 0 for no valid data*/
    uint16_t        GruStatusOffset[MAX_MODEL_SUPPORTED][MAX_GRU_NUM_IN_MODEL]; /*Offset of the GRU status data, 0 for no valid data*/
    uint16_t        TrainType[MAX_MODEL_SUPPORTED];             /*net/train type: NetType for definition*/
    uint16_t		VpuCodeSize[MAX_MODEL_SUPPORTED];			/*vpu code size of each model*/
	uint16_t        WeightMemoryType[MAX_MODEL_SUPPORTED];		/*memory type of weight data, refer MemoryType for details*/
	uint16_t		DataMemoryType[MAX_MODEL_SUPPORTED]; 		/*Memory type of buffering data, refer MemoryType for details*/
	uint16_t        CommandNumber[MAX_MODEL_SUPPORTED];         /*Command number of the model out, including 'silence'*/
    uint16_t        PostProcessMode[MAX_MODEL_SUPPORTED];       /*post processing mode, check 'MidPpMode' for details*/
	uint16_t        Reserved[24];                               /*reserved space to add more element and backward compatibility*/    
	LayerParam_t    Layers[];	                    			/*Layer's parameters, the real size depends on 'LayersNum'*/
}NetInfo_t;

struct Net_s
{
	NetInfo_t       *pNetInfo;
	void            *pNpuMemBase;
	void			*pVpuCode;
	void			*pWeight;
	void			*pMicWeightMatrix;
	void			*pPrivateData;
	int32_t         inputSize[MAX_MODEL_SUPPORTED];
	int32_t         netOutputSize[MAX_MODEL_SUPPORTED];
	int32_t         amOutputSize[MAX_MODEL_SUPPORTED];	
    void            *npu_callback;
#ifdef __x86_64
    int32_t         saveInputFlag;
    void            *npu_layer_callback;    
#endif
}__attribute__((aligned(32)));
typedef struct Net_s  Net_t;

extern void* iet_shell_memcpy32(void *des, const void *src, int32_t len);
//extern void iet_print(IET_PRINT_ALG_DBG,  const char* fmt, ... );

// common global variables
extern int frame_index_to_print;
extern uint32_t weight_base_flash;
extern uint32_t current_weight_base;

extern int16_t spl_fifo[MAX_AUDIO_CH][MAX_FRAME_SPL];
extern int32_t spl_wr;

extern Net_t g_net;
extern NetInfo_t* g_p_net_info;
#endif
