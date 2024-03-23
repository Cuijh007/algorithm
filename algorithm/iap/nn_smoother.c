#ifndef ALGORITHM_RELEASE
#include "nn_smoother.h"

/********************************
*ZHANGHUI convert cpp to c
*********************************/

void CSmoother_init(struct CSmoother_t *CSmoother, int dim, int windowsSize, int smoothBuffFrameNum)
{
	CSmoother->init = CSmoother_init;
	CSmoother->reset = CSmoother_reset;
	CSmoother->smoothFeature = CSmoother_smoothFeature;

	CSmoother->m_dim = dim;
	CSmoother->m_smoothWindowsSize = windowsSize;
	memset(CSmoother->m_raw_data, 0, sizeof(int)*dim*windowsSize);

	CSmoother->m_smoothBuffFrameNum = smoothBuffFrameNum;
	memset(CSmoother->m_raw_data, 0, sizeof(int)*dim*smoothBuffFrameNum);

	CSmoother->reset(CSmoother);
}

void CSmoother_reset(struct CSmoother_t *CSmoother)
{
	CSmoother->m_frameCnt = 0;
}


/*
* param:
*	result : VPU output data , short 169, have left shift 15 bits
*	maxScore : have left shift 15 bits
*/
void CSmoother_smoothFeature(struct CSmoother_t *CSmoother, int* result, int* maxScores)
{
	int p_smooth;
	int h_smooth;
	int scoreId = CSmoother->m_frameCnt % CSmoother->m_smoothWindowsSize;
	int smthId = CSmoother->m_frameCnt % CSmoother->m_smoothBuffFrameNum;

	//load score
	int phone_offset = 0;
	for(int i=0; i<CSmoother->m_dim; ++i)
	{
		CSmoother->m_raw_data[scoreId * CSmoother->m_dim + i] = (int)result[i];
	}

	h_smooth = 0 > CSmoother->m_frameCnt + 1 - CSmoother->m_smoothWindowsSize ? \
							0 : CSmoother->m_frameCnt + 1 - CSmoother->m_smoothWindowsSize;
	//weight =  1.0f / (m_frameCnt - h_smooth + 1);
	for(int i=0; i<CSmoother->m_dim; i++)
	{
		p_smooth = 0;
		for(int j=0; j<CSmoother->m_frameCnt-h_smooth+1; j++)
		{
			p_smooth += CSmoother->m_raw_data[j*CSmoother->m_dim+i];
		}
		p_smooth /= (CSmoother->m_frameCnt - h_smooth + 1);
		CSmoother->m_smooth_data[smthId*CSmoother->m_dim + i] = p_smooth;
	}
	CSmoother->m_frameCnt++;
	int buffFrameCnt = (CSmoother->m_frameCnt > CSmoother->m_smoothBuffFrameNum) ? CSmoother->m_smoothBuffFrameNum : CSmoother->m_frameCnt;
	for(int j=0; j<buffFrameCnt; j++)
	{
		for(int i=0; i<CSmoother->m_dim; ++i)
		{
			maxScores[i] = (CSmoother->m_smooth_data[j*CSmoother->m_dim + i] > maxScores[i]) ? CSmoother->m_smooth_data[j*CSmoother->m_dim + i] : maxScores[i];
		}
	}

}
#endif