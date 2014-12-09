/**************************************************************************************************************************//**
* @file    audiostreaming.c
* @author  Maxim Ivanchenko
* @brief   Audiostreaming for WindowsNT using waveXxx API
******************************************************************************************************************************/
#ifdef _WIN32

#include "ip_stack/udp_server.h"

// for recording
#include "ip_stack/udp_server.h"
#include "lib/rtp/rtp.h"
#include "lib/codec/g711/itu/g711itu.h" 

// common
#include "audiostreaming.h"
#include <windows.h>
#include "config.h"
#include "lib/queue/simple_queue.h"
#include "osip2/osip.h" // need to be included after sockets
#include "rtp/rtp_handling.h"

/*
MMRESULT:
MMSYSERR_NOERROR = 0,
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
extern IN_ADDR			gDestIp;
extern char				gSpdPort[6];

extern osip_t*			gpOsip;
extern char				gRtpSessionActive;
extern SOCKET			gRtpSock;

signed short			gSamplesBufEncoded16bit[SAMPLES_IN_RTP_PACKET]; // becuase of encoder operates shorts
signed char				gSamplesBufEncoded8bit[SAMPLES_IN_RTP_PACKET]; // becuase of encoder operates shorts
signed short			gSamplesBufRaw[SAMPLES_IN_RTP_PACKET];
/*****************************************************************************************************************************/
char MuLaw_Encode(signed short number);
/**************************************************************************************************************************//**
* @brief Thread for playing samples received through RTP
******************************************************************************************************************************/
#if !defined PLAY_IN_RTP_RCV_THREAD 
DWORD WINAPI PlaySamplesThread(LPVOID p)
{
	tAudioElement* pAudEl;
	int i;

	gAudioBuf[0].mutex = 0;
	gAudioBuf[0].handleNeeded = 0;
	gAudioBuf[1].mutex = 0;
	gAudioBuf[1].handleNeeded = 0;

	PlayingInit(&gAudioBuf[0]);
	PlayingInit(&gAudioBuf[1]);

	while (1)
	{

		for (i = 0; i < AUDIO_FIFO_LEN; i++)
		{
			pAudEl = &gAudioBuf[i];
			if ((pAudEl->mutex == 0) && (pAudEl->handleNeeded != 0))
			{
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
#endif
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
#if 0 // volume regulation
	LPDWORD pdwVolume = 1;
	unsigned int audioDevs = waveOutGetNumDevs();
	mmRes = waveOutGetVolume(hWaveOut, &pdwVolume);
	mmRes = waveOutSetVolume(hWaveOut, 0xfffa);
#endif

	pThis->waveXxx.whdr.lpData = (LPSTR) pThis->buffer;
	pThis->waveXxx.whdr.dwBufferLength = (DWORD) pThis->sizeInBytes;


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
	tAudioElement	audEl;
	tRtpPacket		rtp;			// unpacket rtp packet
	char*			pRtpFrame;		// packet rtp packet
	int				rtpFrameLen;
	unsigned short	cntr = 0;
	unsigned int	timestamp = 0;
	int				port = 0;
	//float			seconds		= 0;
	int				i = 0;

	// At samplerate 8000 kHz we need to accumulate SAMPLES_IN_RTP_PACKET (160) samples
	// and then send them in the single RTP packet.
	// So we got 50 RTP packets per second, i.e. send packet evety 20 ms

	RecordingInit(&audEl);

	while (1)
	{
		if (gRtpSessionActive)
		{
			// get a samples from mic
			RecordSamples(&audEl); // blocking until get 160 samples. 20 ms passed

			// encode them
			ulaw_compress(SAMPLES_IN_RTP_PACKET, gSamplesBufRaw, gSamplesBufEncoded16bit);
			for (i = 0; i < SAMPLES_IN_RTP_PACKET; i++)
			{
				gSamplesBufEncoded8bit[i] = gSamplesBufEncoded16bit[i]; // becuase of encoder operates shorts
			}

			// compose RTP packet
			RtpCompose(cntr, timestamp, gSamplesBufEncoded8bit, SAMPLES_IN_RTP_PACKET, &pRtpFrame, &rtpFrameLen);

			// send it
			port = atoi(gSpdPort);
			if ((gDestIp.S_un.S_addr != 0) && (port != 0))
			{
				//UdpSend(pRtpFrame, rtpFrameLen, gDestIp, port);
				UdpSendExistSock(gRtpSock, pRtpFrame, rtpFrameLen, gDestIp, port);
			}

			timestamp += SAMPLES_IN_RTP_PACKET;
			cntr++;
		}
	}
	// potential deinit
	RecordingDeinit(&audEl);
	return 0;
}
/**************************************************************************************************************************//**
* @brief Init of RecordSamples function
******************************************************************************************************************************/
int RecordingInit(tAudioElement* pThis)
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
	pThis->waveXxx.whdr.lpData = (LPSTR) gSamplesBufRaw;
	pThis->waveXxx.whdr.dwBufferLength = (DWORD) sizeof(gSamplesBufRaw); // 160 16-bit samples
	pThis->waveXxx.whdr.dwFlags = WHDR_BEGINLOOP;
	//pThis->waveXxx.whdr.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP | WHDR_PREPARED;
	pThis->waveXxx.whdr.dwBytesRecorded = 0;
	pThis->waveXxx.whdr.dwFlags = 0;
	pThis->waveXxx.whdr.dwLoops = 0;
	pThis->waveXxx.whdr.dwUser = 0;

	mmRes = waveInOpen(&pThis->waveXxx.hWaveIn, WAVE_MAPPER, &pThis->waveXxx.wf, 0, 0, WAVE_FORMAT_DIRECT); // WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE  CALLBACK_NULL  WAVE_FORMAT_DIRECT
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmRes = mmRes; // bp
	}

	return mmRes;
}
/**************************************************************************************************************************//**
* @brief Deinit of RecordSamples object
******************************************************************************************************************************/
int RecordingDeinit(tAudioElement* pThis)
{
	MMRESULT mmRes;
	mmRes = waveOutClose(pThis->waveXxx.hWaveIn);
	return mmRes;
}
/**************************************************************************************************************************//**
* @brief
******************************************************************************************************************************/
int RecordSamples(tAudioElement* pThis)
{
	MMRESULT mmRes;
	struct sWaveXxx* p = &pThis->waveXxx;

	mmRes = waveInPrepareHeader(p->hWaveIn, &p->whdr, sizeof(p->whdr));

	// Insert a wave input buffer
	mmRes = waveInAddBuffer(p->hWaveIn, &p->whdr, sizeof(WAVEHDR));

	// Commence sampling input
	mmRes = waveInStart(p->hWaveIn);

	// Wait until finished recording
	while (!(p->whdr.dwFlags & WHDR_DONE));
	waveInUnprepareHeader(p->hWaveIn, &p->whdr, sizeof(WAVEHDR));
}
/*****************************************************************************************************************************/
#endif //_WIN32