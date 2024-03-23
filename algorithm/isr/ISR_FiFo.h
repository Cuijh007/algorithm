#ifndef _ISR_FIFO_H_
#define _ISR_FIFO_H_

#include <stdio.h>

typedef struct FiFoType{
	short *buffer;
	int front;					//head Index
	int tail;					//tail Index
	short BUFFERSIZE;
}FiFoQueue;

void InitFiFoQueue(FiFoQueue *q);
int FiFoQueueIsFull(FiFoQueue *q);
unsigned char FiFoQueueIsNull(FiFoQueue *q);
int FiFoQueueSize(FiFoQueue *q);
void PushFiFoQueue(FiFoQueue *q, short element);
short PopFiFoQueue(FiFoQueue *q);
void PushFiFoQueueBatch(FiFoQueue *q, short *src, int len);
void PopFiFoQueueBatch(FiFoQueue *q, short *dst, int len);
void GetFiFoQueueBatch(FiFoQueue *q, short *dst, int len);
int  ReadFiFoQueueBatch(FiFoQueue *q, short **dst, int len);
void SetFiFoQueueClean(FiFoQueue *q);

#endif
