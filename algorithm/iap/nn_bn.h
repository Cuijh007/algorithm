#ifndef __BN_H__
#define __BN_H__

#include "nn_file_struct.h"
#include "nn_common.h"

#ifndef ALGORITHM_RELEASE

#define BN_CCFG2_SCALE	0xA0208824
#define BN_CCFG3_SCALE	0xA0208828
#define BN_CCFG4_SCALE	0xA020882C
#define BN_LOADW_SCALE	0xA0208834

#define BN_CCFG2_VECTOR	0xA0208838
#define BN_CCFG3_VECTOR	0xA020883C
#define BN_CCFG4_VECTOR	0xA0208840
#define BN_LOADW_VECTOR	0xA0208848

#define BN_MULC_ELP_H   0xA020884C
#define BN_MULC_ELP_L   0xA0208850   
#define BN_MULC_EAI_H   0xA0208858
#define BN_MULC_EAI_L   0xA020885C
#define BN_MULC_EBI_H   0xA0208864
#define BN_MULC_EBI_L   0xA0208868
#define BN_MULC_ECI_H   0xA0208870
#define BN_MULC_ECI_L   0xA0208874
#define BN_MULC_EDI_H   0xA020887C
#define BN_MULC_EDI_L   0xA0208880

#define BN_EAA_H_MULC	0xA0208894
#define BN_EAA_L_MULC	0xA0208898
#define BN_EBA_H_MULC	0xA020889C
#define BN_EBA_L_MULC	0xA02088A0
#define BN_ECA_H_MULC	0xA02088A4
#define BN_ECA_L_MULC	0xA02088A8
#define BN_EDA_H_MULC	0xA02088AC
#define BN_EDA_L_MULC	0xA02088B0

#define BN_MULC_CCFG0   0xA0208888
#define BN_MULC_CCFG1   0xA020888C
#define BN_MULC_NUM     0xA02088C4

#endif

extern int nn_bn(const LayerParam_t *param, void *pBase);
#endif