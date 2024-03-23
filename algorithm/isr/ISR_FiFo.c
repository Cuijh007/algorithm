#include "ISR_FiFo.h"

void InitFiFoQueue(FiFoQueue *q)
{
	q->front=q->tail=0;
}

int FiFoQueueIsFull(FiFoQueue *q)
{
	if( (q->tail+1)%(q->BUFFERSIZE) == q->front)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

unsigned char FiFoQueueIsNull(FiFoQueue *q)
{
	if(q == NULL)
	{
		return -1;
	}
	if(q->front == q->tail)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int FiFoQueueSize(FiFoQueue *q)
{
	if(q == NULL)
	{
		return -1;
	}
	else
	{
		if(q->front > q->tail)
		{
			return (q->tail + (q->BUFFERSIZE) - q->front);
		}
		else
		{
			return (q->tail - q->front);
		}
	}
}

void PushFiFoQueue(FiFoQueue *q, short element)
{
	if(FiFoQueueIsFull(q))
	{
		PopFiFoQueue(q);
	}
	q->tail = (q->tail+1) % (q->BUFFERSIZE);
	q->buffer[q->tail] = element;
}

short PopFiFoQueue(FiFoQueue *q)
{
	if(FiFoQueueIsNull(q))
	{
		return -1;
	}
	q->front = (q->front+1) % (q->BUFFERSIZE);
	return q->buffer[q->front];
}

void PushFiFoQueueBatch(FiFoQueue *q, short *src, int len)
{
	if(q == NULL)
	{
		return ;
	}
	int i = 0;
	for (i=0; i<len; i++)
	{
		PushFiFoQueue(q, src[i]);
	}
}

void PopFiFoQueueBatch(FiFoQueue *q, short *dst, int len)
{
	if(q == NULL)
	{
		return ;
	}
	int i=0;
	for(i=0; i<len; i++)
	{
		dst[i] = PopFiFoQueue(q);
	}
}

void GetFiFoQueueBatch(FiFoQueue *q, short *dst, int len)
{
	if(q == NULL)
	{
		return ;
	}
	int i=0;
	int next =  q->tail + 1;
	for (i=0; i<len; i++)
	{
		next = (next+1) % (q->BUFFERSIZE);
		dst[i] = q->buffer[next];
	}
}

void SetFiFoQueueClean(FiFoQueue *q)
{
	if(q == NULL)
	{
		return ;
	}
	memset(q->buffer, 0, sizeof(short)*q->BUFFERSIZE);
}

int ReadFiFoQueueBatch(FiFoQueue *q, short **dst, int len)
{
	int top = q->tail;

    if(q == NULL)
	{
		return 0;
	}

    if (q->front !=  (q->BUFFERSIZE - 1)) {
        if (q->front > q->tail) {
            top = q->BUFFERSIZE - 1;
        }
        if ((top - q->front) < len) {
            len = top - q->front;
        }
    }

    *dst = &(q->buffer[(q->front + 1) % (q->BUFFERSIZE)]);
    q->front = (q->front + len) % (q->BUFFERSIZE);

    APP_MSG("ReadFiFoQueueBatch - read len=%d\r\n", len);
    APP_MSG("ReadFiFoQueueBatch - q->front=%d, q->tail=%d\r\n", q->front, q->tail);
    APP_MSG("ReadFiFoQueueBatch - FiFoQueue remain size=%d\r\n", FiFoQueueSize(q));
    APP_MSG("ReadFiFoQueueBatch - dst=0x%x\r\n", *dst);

    return len;
}
