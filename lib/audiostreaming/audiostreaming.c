/***************************************************************************************************************************//*
* @file    audiostreaming.c
* @author  Maxim Ivanchenko
* @version 1.0
* @date    November, 2014
* @brief   
******************************************************************************************************************************/
#ifdef _WIN32
#include <windows.h> // Mmsystem.h
#include <stdio.h>
#include <string.h>
#pragma comment(lib,"winmm") 
#include "lib/wav/wav.h"
#include "audiostreaming.h"

/*  MMSYSERR_NOERROR = 0,
MMSYSERR_ERROR = 1,
MMSYSERR_BADDEVICEID = 2,
MMSYSERR_NOTENABLED = 3,
MMSYSERR_ALLOCATED = 4,
MMSYSERR_INVALHANDLE = 5,
MMSYSERR_NODRIVER = 6,
MMSYSERR_NOMEM = 7,
MMSYSERR_NOTSUPPORTED = 8,
MMSYSERR_BADERRNUM = 9,
MMSYSERR_INVALFLAG = 10,
MMSYSERR_INVALPARAM = 11,
MMSYSERR_HANDLEBUSY = 12,
MMSYSERR_INVALIDALIAS = 13,
MMSYSERR_BADDB = 14,
MMSYSERR_KEYNOTFOUND = 15,
MMSYSERR_READERROR = 16,
MMSYSERR_WRITEERROR = 17,
MMSYSERR_DELETEERROR = 18,
MMSYSERR_VALNOTFOUND = 19,
MMSYSERR_NODRIVERCB = 20,
WAVERR_BADFORMAT = 32,
WAVERR_STILLPLAYING = 33,
WAVERR_UNPREPARED = 34 */

WAVEFORMATEX	wf;
WAVEHDR			whdr;
HWAVEOUT		hWaveOut;
LPSTR			lpData;
MMRESULT		mmRes;

// double buffer
#define AUDIO_FIFO_LEN	2
tAudioElement gAudioBuf[AUDIO_FIFO_LEN];
/***************************************************************************************************************************//*
* @brief
******************************************************************************************************************************/
DWORD WINAPI PlaySamplesThread(LPVOID p)
{
	tAudioElement* pAudEl;
	int i;
	while (1)
	{
		for (i = 0; i < AUDIO_FIFO_LEN; i++)
		{
			pAudEl = &gAudioBuf[i];
			if ((pAudEl->mutex == 0) && (pAudEl->handleNeeded != 0))
			{
				pAudEl->mutex = 1;
				PlaySamples(pAudEl->buffer, pAudEl->sizeInBytes);
				pAudEl->handleNeeded = 0;
				pAudEl->mutex = 0;
			}
		}
	}
	
}
/***************************************************************************************************************************//*
* @brief 
******************************************************************************************************************************/
void PlaySamples(void* buffer, unsigned int sizeInBytes)
{
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = 1;
	wf.nSamplesPerSec = 8000;
	wf.wBitsPerSample = 16;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.wBitsPerSample/2 * wf.nChannels;
	wf.nBlockAlign = 1;
	wf.cbSize = 0;

	whdr.lpData = buffer;
	whdr.dwBufferLength = sizeInBytes;
	//whdr.dwFlags = WHDR_BEGINLOOP;
	whdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP | WHDR_PREPARED;
	whdr.dwLoops = 0;


	mmRes = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL); // WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE 

	//LPDWORD pdwVolume = 1;
	//unsigned int audioDevs = waveOutGetNumDevs();
	//mmRes = waveOutGetVolume(hWaveOut, &pdwVolume);
	//mmRes = waveOutSetVolume(hWaveOut, 0xfffa);

	mmRes = waveOutPrepareHeader(hWaveOut, &whdr, sizeof(whdr));
	mmRes = waveOutWrite(hWaveOut, &whdr, sizeof(whdr));

	while (!(whdr.dwFlags & WHDR_DONE));

	mmRes = waveOutUnprepareHeader(hWaveOut, &whdr, sizeof(whdr));
	mmRes = waveOutClose(hWaveOut);

}
/***************************************************************************************************************************//*
* @brief 
******************************************************************************************************************************/
unsigned int CbRecordSamples(void* buffer);
/*****************************************************************************************************************************/

#endif