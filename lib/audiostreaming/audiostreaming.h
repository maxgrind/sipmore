/***************************************************************************************************************************//*
 * @file    audiostreaming.h
 * @author  Maxim Ivanchenko
 * @brief     
******************************************************************************************************************************/
#ifndef __AUDIOSTREAMING_H__
#define __AUDIOSTREAMING_H__
/*****************************************************************************************************************************/
#define AUDIO_ELEMENT_BUFFER_LEN  1600 // elements of short
/*****************************************************************************************************************************/
#include <windows.h> // Mmsystem.h
#include <stdio.h>
#include <string.h>
#pragma comment(lib,"winmm") 
#include "lib/wav/wav.h"
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
	//HANDLE WINAPI winapiMutex;
	char mutex;
	char handleNeeded;
	signed short buffer[AUDIO_ELEMENT_BUFFER_LEN];
	int sizeInBytes;

	//waveXxx types
	struct sWaveXxx
	{
		WAVEFORMATEX	wf;
		WAVEHDR			whdr;
		HWAVEOUT		hWaveOut;
	//	LPSTR			lpData;
	}
	waveXxx;
}
tAudioElement;
/*****************************************************************************************************************************/
DWORD WINAPI PlaySamplesThread(LPVOID t);
/*****************************************************************************************************************************/
int PlayingInit(tAudioElement* pThis);
int PlayingDeinit(tAudioElement* pThis);
int PlaySamples(tAudioElement* pThis);
/*****************************************************************************************************************************/
#endif // __AUDIOSTREAMING_H__