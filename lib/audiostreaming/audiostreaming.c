/**************************************************************************************************************************//**
* @file    audiostreaming.c
* @author  Maxim Ivanchenko
* @brief   Audiostreaming for WindowsNT using waveXxx API
******************************************************************************************************************************/
#ifdef _WIN32

#include "audiostreaming.h"
#include <windows.h>

#include "lib/rtp/rtp.h" // for recording

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
/*****************************************************************************************************************************/

// double buffering. todo: replace with a queue
#define AUDIO_FIFO_LEN	2
tAudioElement			gAudioBuf[AUDIO_FIFO_LEN];
extern HANDLE WINAPI	gPalyThreadMutex[2];
/*****************************************************************************************************************************/
// recording params. todo: make them configurable
#define SAMPLES_IN_RTP_PACKET	160
#define RTP_SAMPLE_SIZE_BYTE	8	 // PCMU encoded
#define RTP_INTERVAL_MS			20
#define SAMPLE_RATE				8000 // PCMU
extern char				gRtpSessionActive;
char					gSamplesBuf[SAMPLES_IN_RTP_PACKET];
/*****************************************************************************************************************************/
char MuLaw_Encode(signed short number);
/**************************************************************************************************************************//**
* @brief Thread for playing samples received through RTP
******************************************************************************************************************************/
DWORD WINAPI PlaySamplesThread(LPVOID p)
{

	tAudioElement* pAudEl;
	int i;

	PlayingInit(&gAudioBuf[0]);
	PlayingInit(&gAudioBuf[1]);
	while (1)
	{
		for (i = 0; i < AUDIO_FIFO_LEN; i++)
		{
			pAudEl = &gAudioBuf[i];
			//if (gPalyThreadMutex[0] )
			if ((pAudEl->mutex == 0) && (pAudEl->handleNeeded != 0))
			{
				//pAudEl->winapiMutex;
				pAudEl->mutex = 1;
				PlaySamples(pAudEl);
				pAudEl->handleNeeded = 0;
				pAudEl->mutex = 0;
			}
		}
	}
	// potential deinit
	// PlayingDeinit(&gAudioBuf[0]);
	// PlayingDeinit(&gAudioBuf[1]);
	return 0;
}
/**************************************************************************************************************************//**
* @brief Init of PlaySamples function
******************************************************************************************************************************/
int PlayingInit(tAudioElement* pThis)
{
	MMRESULT mmRes;
	// fill up the wave format
	pThis->waveXxx.wf.wFormatTag = WAVE_FORMAT_PCM;
	pThis->waveXxx.wf.nChannels = 1;
	pThis->waveXxx.wf.nSamplesPerSec = 8000;
	pThis->waveXxx.wf.wBitsPerSample = 16;
	pThis->waveXxx.wf.nBlockAlign = pThis->waveXxx.wf.nChannels * (pThis->waveXxx.wf.wBitsPerSample / 8);
	pThis->waveXxx.wf.nAvgBytesPerSec = pThis->waveXxx.wf.nSamplesPerSec * pThis->waveXxx.wf.nBlockAlign;
	//pThis->waveXxx.wf.nAvgBytesPerSec = pThis->waveXxx.wf.nSamplesPerSec * pThis->waveXxx.wf.wBitsPerSample / 8 * pThis->waveXxx.wf.nChannels; //8000 * 2;//
	pThis->waveXxx.wf.cbSize = 0;

	// fill up the wave header
	pThis->waveXxx.whdr.lpData = (LPSTR) pThis->buffer;
	pThis->waveXxx.whdr.dwBufferLength = (DWORD) pThis->sizeInBytes;
	pThis->waveXxx.whdr.dwFlags = WHDR_BEGINLOOP;
	//pThis->waveXxx.whdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP | WHDR_PREPARED;
	pThis->waveXxx.whdr.dwLoops = 0;

	mmRes = waveOutOpen(&pThis->waveXxx.hWaveOut, WAVE_MAPPER, &pThis->waveXxx.wf, 0, 0, CALLBACK_NULL); // WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE 
	return mmRes;
}
/**************************************************************************************************************************//**
 * @brief Deinit of PlaySamples object
******************************************************************************************************************************/
int PlayingDeinit(tAudioElement* pThis)
{
	MMRESULT mmRes;
	mmRes = waveOutClose(pThis->waveXxx.hWaveOut);
	return mmRes;
}
/**************************************************************************************************************************//**
* @brief Play samples received through RTP
******************************************************************************************************************************/
int PlaySamples(tAudioElement* pThis)
{
	MMRESULT mmRes;
	struct sWaveXxx* p = &pThis->waveXxx;

	//LPDWORD pdwVolume = 1;
	//unsigned int audioDevs = waveOutGetNumDevs();
	//mmRes = waveOutGetVolume(hWaveOut, &pdwVolume);
	//mmRes = waveOutSetVolume(hWaveOut, 0xfffa);

	pThis->waveXxx.whdr.lpData = (LPSTR) pThis->buffer;
	pThis->waveXxx.whdr.dwBufferLength= (DWORD) pThis->sizeInBytes;


	mmRes = waveOutPrepareHeader(p->hWaveOut, &p->whdr, sizeof(p->whdr));
	mmRes = waveOutWrite(p->hWaveOut, &p->whdr, sizeof(p->whdr));

	while (!(p->whdr.dwFlags & WHDR_DONE));

	mmRes = waveOutUnprepareHeader(p->hWaveOut, &p->whdr, sizeof(p->whdr));

	return mmRes; // todo
}
/**************************************************************************************************************************//**
* @brief Thread for recording samples and pass them through RTP
 todo: create/kill dynamically 
******************************************************************************************************************************/
DWORD WINAPI RecSamplesThread(LPVOID p)
{
	tAudioElement* pAudEl;
	int i;

	// At samplerate 8000 kHz we need to accumulate SAMPLES_IN_RTP_PACKET (160) samples
	// and then send them in the single RTP packet.
	// So we got 50 RTP packets per second, i.e. send packet evety 20 ms

	RecordingInit(&pAudEl);

	while (1)
	{
		if (gRtpSessionActive)
		{
			static unsigned char cntr = 0;
			signed char sample = 0;

			// get a sample from mic

			// encode it
			gSamplesBuf[cntr] = MuLaw_Encode(sample);

			cntr++;
			if (cntr >= SAMPLES_IN_RTP_PACKET) // 160 for now
			{
				// compose send RTP packet
				// send it

				cntr = 0;
			}
		}
	}
	// potential deinit
	RecordingDeinit(&pAudEl);
	return 0;
}
/**************************************************************************************************************************//**
* @brief Init of v object
******************************************************************************************************************************/
int RecordingInit(tAudioElement* pThis);
/**************************************************************************************************************************//**
* @brief Deinit of RecordSamples object
******************************************************************************************************************************/
int RecordingDeinit(tAudioElement* pThis);
/**************************************************************************************************************************//**
* @brief 
******************************************************************************************************************************/
int RecordSamples(tAudioElement* pThis)
{
}
/*****************************************************************************************************************************/
#endif //_WIN32