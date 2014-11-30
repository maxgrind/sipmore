/***************************************************************************************************************************//*
 * @file    wav.h
 * @author  Maxim Ivanchenko
 * @version 1.0
 * @date    November, 2014
 * @brief     
******************************************************************************************************************************/
#ifndef __AUDIOSTREAMING_H__
#define __AUDIOSTREAMING_H__
/*****************************************************************************************************************************/
#include <stdio.h>
/*****************************************************************************************************************************/
DWORD WINAPI PlaySamplesThread(LPVOID t);
/*****************************************************************************************************************************/
void PlaySamples(signed short* buffer, unsigned int sizeInBytes);
/*****************************************************************************************************************************/
typedef struct sAudioStreaming
{
	//void(*CbPlaySamples)(signed short* buffer, unsigned int sizeInBytes);
	//unsigned int(*CbRecordSamples)(signed short* buffer);
	struct sAudioConfig
	{
		unsigned int numOfChannels;
		unsigned int samplesPerSec;
		unsigned int bitsPerSample;
	}
	tAudioConfig;
}
tAudioStreaming;
/*****************************************************************************************************************************/
typedef struct sAudioElement
{
	char mutex;
	char handleNeeded;
	signed short buffer[1600];
	int sizeInBytes;
}
tAudioElement;

/*****************************************************************************************************************************/
#endif // __AUDIOSTREAMING_H__