/**************************************************************************************************************************//**
* @file    simple_queue.h
* @author  Maxim Ivanchenko
* @brief   Simple queue implementation
******************************************************************************************************************************/
#ifndef __SIMPLE_QUEUE_H__
#define __SIMPLE_QUEUE_H__
/*****************************************************************************************************************************/
//#include <stdio.h>
/*****************************************************************************************************************************/
typedef enum eQueueErrors
{
	QUEUE_OK,
	QUEUE_NOT_ENOUGH_MEMORY,
	QUEUE_BAD_PARAM,
	QUEUE_IS_FULL,
	QUEUE_IS_EMPTY,

}
tQueueErrors;
/*****************************************************************************************************************************/
#if 0 // 
typedef struct sQueueEntity
{
	void*			pHead;			// pointer to the head of a queue
	void*			pTail;			// pointer to the tail of a queue
	unsigned int	elSize;			// size of element in bytes
	unsigned int	maxElements;	// max quantity of elements
	unsigned int	nowElements;	// current quantity of enqueued elements

	struct _sInternal
	{
		void*			pQueueBeg;	// shows where queue begins when it is malloced
		void*			pQueueEnd;	// where queue ends
		unsigned int	overflows;	// increments when
	}
	_internal;
}
tQueueEntity;
#else
typedef struct sQueueEntity
{
	unsigned int*	pHead;			// pointer to the head of a queue
	unsigned int*	pTail;			// pointer to the tail of a queue
	unsigned int	elSize;			// size of element in bytes
	unsigned int	maxElements;	// max quantity of elements
	unsigned int	nowElements;	// current quantity of enqueued elements

	struct _sInternal
	{
		unsigned int*	pQueueBeg;	// shows where queue begins when it is malloced
		unsigned int*	pQueueEnd;	// where queue ends
		unsigned int	overflows;	// increments when
	}
	_internal;
}
tQueueEntity;
#endif // 0

/*****************************************************************************************************************************/
int QueueInit(tQueueEntity* pQueue);
int QueueDeinit(tQueueEntity* pQueue);
int QueuePut(tQueueEntity* pQueue, void* pElement);
int QueueGet(tQueueEntity* pQueue, void* pElement);
/*****************************************************************************************************************************/
#endif // __SIMPLE_QUEUE_H__
