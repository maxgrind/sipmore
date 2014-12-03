/**************************************************************************************************************************//**
* @file    simple_queue.c
* @author  Maxim Ivanchenko
* @brief   Simple queue implementation
******************************************************************************************************************************/
#include <stdlib.h> // malloc
#include <string.h> // memcpy

#include "simple_queue.h"
/**************************************************************************************************************************//**
* @brief Init a queue
******************************************************************************************************************************/
int QueueInit(tQueueEntity* p)
{
	if (p == NULL) 
	{
		return QUEUE_BAD_PARAM;
	}

#if 0
	switch (p->elSize)
	{
		case 1:
		{
			p->_internal.pQueueBeg = (char*) malloc(p->maxElements * p->elSize); 
			break;
		}
		case 2:
		{
			p->_internal.pQueueBeg = (short*) malloc(p->maxElements * p->elSize);
			break;
		}
		case 0:
		case 3:
		{
			return QUEUE_BAD_PARAM;
		}
		default:
		{
			p->_internal.pQueueBeg = (int*) malloc(p->maxElements * p->elSize);
			break;
		}
	}
#else
	int roundedSizeInt = (p->elSize + 3) >> 2;  // rounded to int size in ints
	p->_internal.pQueueBeg = (unsigned int*) malloc(p->maxElements * roundedSizeInt * sizeof(int));
	p->_internal.pQueueEnd = p->_internal.pQueueBeg + (p->maxElements * roundedSizeInt);
#endif
	if (p->_internal.pQueueBeg == NULL)
	{
		return QUEUE_NOT_ENOUGH_MEMORY;
	}
	
	p->pHead = p->_internal.pQueueBeg;
	p->pTail = p->_internal.pQueueBeg;

	p->_internal.overflows = 0;

	return QUEUE_OK;
}
/**************************************************************************************************************************//**
* @brief Deinit a queue
******************************************************************************************************************************/
int QueueDeinit(tQueueEntity* p)
{
	if (p == NULL) 
	{
		return QUEUE_BAD_PARAM;
	}

	if (p->_internal.pQueueBeg != NULL)
	{
		free(p->_internal.pQueueBeg);
	}
	return QUEUE_OK;

}
/**************************************************************************************************************************//**
* @brief Add an element to a queue
******************************************************************************************************************************/
int QueuePut(tQueueEntity* p, void* pElement)
{
	int roundedSizeInt	= (p->elSize + 3) >> 2; // rounded to int size in ints
	int remainsInts		= 0;					// ints quantity from the head till the end

	unsigned int* pHead = p->pHead;
	unsigned int* pTail = p->pTail;
	unsigned int* pBeg = p->_internal.pQueueBeg;
	unsigned int* pEnd = p->_internal.pQueueEnd;

	if ((p == NULL) || (pElement == NULL))
	{
		return QUEUE_BAD_PARAM;
	}


	// 0. haed had overtaken the tail in previous call of the QueueAddEl()
	if ((pHead == pTail) && (p->_internal.overflows == 1))
	{
		return QUEUE_IS_FULL;
	}


	//// 1. head goes through the end, tail becomes ahead the head
	//if (((pHead + roundedSizeInt) >= pEnd) && (p->_internal.overflows == 0))
	//{	
	//	remainsInts = pEnd - pHead;

	//	if (pBeg + (roundedSizeInt - remainsInts) >= pTail) // >= ?  может просто больше?
	//	{
	//		return QUEUE_IS_FULL;
	//	}

	//	memcpy(p->pHead, pElement, p->elSize);
	//	p->pHead = pBeg + roundedSizeInt;
	//	p->_internal.overflows++;
	//}
	//// 2. this case happens after the 1st case - tail is already ahead the head
	//else if ((pHead + roundedSizeInt > pTail) && (p->_internal.overflows == 1))
	//{
	//	return QUEUE_IS_FULL;
	//}
	//// 3. head just is ahead the tail
	//else
	//{
		memcpy(p->pHead, pElement, p->elSize);
		p->pHead += roundedSizeInt;
	//}

	return QUEUE_OK;
}
/**************************************************************************************************************************//**
* @brief Remove an element from a queue
******************************************************************************************************************************/
int QueueGet(tQueueEntity* p, void* pElement)
{
	int roundedSizeInt = (p->elSize + 3) >> 2; // rounded to int size in ints
	int remainsInts = 0;					// ints quantity from the head till the beginning

	unsigned int* pHead = p->pHead;
	unsigned int* pTail = p->pTail;
	unsigned int* pBeg = p->_internal.pQueueBeg;
	unsigned int* pEnd = p->_internal.pQueueEnd;

	if ((p == NULL) || (pElement == NULL))
	{
		return QUEUE_BAD_PARAM;
	}

	// 0. haed had overtaken the tail in previous call of the QueueAddEl()
	if ((pHead == pTail) && (p->_internal.overflows == 0))
	{
		return QUEUE_IS_EMPTY;
	}


	// 1. head goes through the beginning
	if (((pHead - roundedSizeInt) <= pEnd) && (p->_internal.overflows == 1))
	{
		remainsInts = pHead - pBeg; 

		if (pEnd - (roundedSizeInt - remainsInts) < pTail)
		{
			return QUEUE_IS_EMPTY;
		}

		memcpy(pElement, p->pHead, p->elSize);
		p->pHead = pEnd - roundedSizeInt;
		p->_internal.overflows--;
	}
	// 2. this case happens after the 1st case
	else if ((pHead + roundedSizeInt > pTail) && (p->_internal.overflows == 1))
	{
		return QUEUE_IS_EMPTY;
	}
	// 3. head just is ahead the tail
	else
	{
		memcpy(pElement, p->pTail, p->elSize);
		p->pTail += roundedSizeInt;
	}


	return QUEUE_OK;
}