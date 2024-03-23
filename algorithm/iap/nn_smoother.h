#ifndef __SMOOTHER_H__
#define __SMOOTHER_H__

#include <string.h>

/********************************
*ZHANGHUI convert cpp to c
*********************************/

typedef struct CSmoother_t
{
	unsigned char m_dim;					
	unsigned char m_smoothBuffFrameNum;	
	unsigned char m_smoothWindowsSize;
	int m_frameCnt;
	int m_smooth_data[200*5];
	int m_raw_data[200*15];
	
	void (*init)(struct CSmoother_t *CSmoother, int dim, int windowsSize, int smoothBuffFrameNum);
	void (*smoothFeature)(struct CSmoother_t *CSmoother, int* result, int* maxScores);
	void (*reset)(struct CSmoother_t *CSmoother);

}CSmoother_s;

void CSmoother_reset(struct CSmoother_t *CSmoother);
void CSmoother_init(struct CSmoother_t *CSmoother, int dim, int windowsSize, int smoothBuffFrameNum);
void CSmoother_smoothFeature(struct CSmoother_t *CSmoother, int* result, int* maxScores);

#endif