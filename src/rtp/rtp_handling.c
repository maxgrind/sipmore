/**************************************************************************************************************************//**
* @file    rtp_handling.c
* @author  Maxim Ivanchenko
* @brief   rtp handling
******************************************************************************************************************************/
#include <stdio.h>
#include "ip_stack/udp_server.h"
#include "sip/sip.h"
#include "lib/rtp/rtp.h"
#include "lib/audiostreaming/audiostreaming.h"
#include "osip2/osip.h"
#include "lib/codec/g711/itu/g711itu.h"
#include "config.h"
#include "rtp/rtp_handling.h"

#include <windows.h>
/*****************************************************************************************************************************/
extern tAudioElement	gAudioBuf[];

SOCKET					gRtpSock;
extern osip_t*			gpOsip;
extern char				gRtpSessionActive;

short					samples16encoded[160];
short					samples16decoded[160];

extern char				gRtpSessionActive;

extern int				gWavIsWriting;
extern tWaveFileParams  gWavParams;

HANDLE hEventBlockPlayed;// this even happens when block has been played

/**************************************************************************************************************************//**
* @brief Filling buffers with input RTP data for PlaySamplesThread (the output data is comletely handled by RecSamplesThread thread)
******************************************************************************************************************************/
int RtpProcess(osip_t* osip, char* pRtpBuf, int udpRecvdSize, SOCKET sock)
{
	tRtpPacket		rtp;
	char*			pPrinfStart = NULL;
	static int		q = 0;
	signed short*	pUncompressed = NULL;
	int				err = 0;
	int				i = 0;
	static int		rtpsTotal = 0;
	pUncompressed = (signed short*) malloc(udpRecvdSize * 2);


	err = RtpParse(pRtpBuf, udpRecvdSize, &rtp);
	// make sure it is RTP (STUN can be also received here)
	if (err != 0)
	{
		static notRtp = 0;
		notRtp++;
		printf("notRtp %d \r\n", notRtp);
		return -1;
	}
	gRtpSessionActive = 1;

	static char buf3000[3000];
	static int cc = 0;
	static int bufsize;
	char* pPayload = rtp.pPayload;

	if ((cc & 1) != 1)
	{
		memcpy(&buf3000[bufsize], rtp.pPayload, rtp.payloadLen);
		bufsize += rtp.payloadLen;
		goto out;

	}
	else
	{
		memcpy(&buf3000[bufsize], rtp.pPayload, rtp.payloadLen);
		bufsize += rtp.payloadLen;
	
		

	}

	//cc++;


#if 0 // recording WAV
#if 1 // u-law(0) vs pcm(1)
	signed short* pInShort = (signed short*) malloc(udpRecvdSize * 2);

	if (pUncompressed == NULL)
	{
		FileWavFinish(&gWavParams);
		gWavIsWriting = 0;
		return -1;
	}
	else
	{
		//decode
#if 1 // choose g711 lib
		for (i = 0; i < rtp.payloadLen; i++)
		{
			pInShort[i] = (signed short) rtp.pPayload[i];
		}
		ulaw_expand(rtp.payloadLen, pInShort, pUncompressed);

#else
		for (i = 0; i < rtp.payloadLen; i++)
		{
			pUncompressed[i] = MuLaw_Decode(rtp.pPayload[i]);
		}
#endif
		gWavIsWriting = 1;
		FileWavAppendData(pUncompressed, rtp.payloadLen * 2, &gWavParams);
	}

#else
	gWavIsWriting = 1;
	FileWavAppendData(rtp.pPayload, rtp.payloadLen, &gWavParams);

#endif
#endif
	//}
	// stream playing
#if 1

	MMRESULT mmRes;
	static int c = 0;
	int index = c & 1;
	WAVEHDR* pCurr = &gAudioBuf[index].waveXxx.whdr;

	for (i = 0; i < rtp.payloadLen; i++)
	{
		//gAudioBuf[index].buffer[i] = MuLaw_Decode(rtp.pPayload[i]);
		gAudioBuf[index].buffer[i] = MuLaw_Decode(buf3000[i]);
	}
	//gAudioBuf[index].sizeInBytes = rtp.payloadLen;
	gAudioBuf[index].sizeInBytes = bufsize;
	gAudioBuf[index].waveXxx.whdr.lpData = gAudioBuf[index].buffer;
	gAudioBuf[index].waveXxx.whdr.dwBufferLength = gAudioBuf[index].sizeInBytes;
	//c++;

	

	//mmRes = waveOutPrepareHeader(gAudioBuf[index].waveXxx.hWaveOut, &gAudioBuf[index].waveXxx.whdr, sizeof(WAVEHDR));
	//if (mmRes != 0)
	//{
	//	mmRes = mmRes;
	//}
	mmRes = waveOutWrite(gAudioBuf[index].waveXxx.hWaveOut, &gAudioBuf[index].waveXxx.whdr, sizeof(WAVEHDR));
	if (mmRes != 0)
	{
		mmRes = mmRes;
	}

	if (WAIT_TIMEOUT == WaitForSingleObject(hEventBlockPlayed, 60))
	{
		mmRes = mmRes;
	}
	

	//mmRes = waveOutUnprepareHeader(gAudioBuf[c & 1].waveXxx.hWaveOut, &gAudioBuf[c & 1].waveXxx.whdr, sizeof(WAVEHDR));
	//if (mmRes != 0)
	//{
	//	mmRes = mmRes;
	//}
	bufsize = 0;
	c++;
#endif
out:
	cc++;
	rtpsTotal++;
	free(pUncompressed);
	rtp.pPayload = pPayload;
	RtpPacketDestroy(&rtp);
	return 0;
}
/**************************************************************************************************************************//**
* @brief Thread for playing samples received through RTP
******************************************************************************************************************************/
DWORD WINAPI RtpReceivingThread(LPVOID p)
{
	tAudioElement* pAudEl;
	int i;

	char*		pRtpBuf = (char*) malloc(3000);
	char*		pRtpBuf2 = (char*) malloc(3000);
	SOCKET		sockRtp = UdpServerCreate(&pRtpBuf, PORT_RTP);
	SOCKADDR_IN sockInRtp;
	int			udpRecvdSize = 0;
	 hEventBlockPlayed = CreateEvent(0, FALSE, FALSE, 0); // this even happens when block has been played


#if defined PLAY_IN_RTP_RCV_THREAD  // need to init if we dont use play thread and play in this thread
	gAudioBuf[0].mutex = 0;
	gAudioBuf[0].handleNeeded = 0;
	gAudioBuf[1].mutex = 0;
	gAudioBuf[1].handleNeeded = 0;

	PlayingInit(&gAudioBuf[0]);
	i = waveOutOpen(&gAudioBuf[0].waveXxx.hWaveOut, WAVE_MAPPER, &gAudioBuf[0].waveXxx.wf, hEventBlockPlayed, 0, CALLBACK_EVENT); // WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE 

	PlayingInit(&gAudioBuf[1]);
	i = waveOutOpen(&gAudioBuf[1].waveXxx.hWaveOut, WAVE_MAPPER, &gAudioBuf[1].waveXxx.wf, hEventBlockPlayed, 0, CALLBACK_EVENT); // WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE 



	i = waveOutPrepareHeader(gAudioBuf[0].waveXxx.hWaveOut, &gAudioBuf[0].waveXxx.whdr, sizeof(WAVEHDR)); // put to init
	i = waveOutPrepareHeader(gAudioBuf[1].waveXxx.hWaveOut, &gAudioBuf[1].waveXxx.whdr, sizeof(WAVEHDR)); // put to init

#endif

	gRtpSock = sockRtp;
	int c = 0;
	int bufsize = 0;
	while (1)
	{
		// RTP 
		udpRecvdSize = UdpServerProcess(sockRtp, pRtpBuf, &sockInRtp);
		if (udpRecvdSize != 0)
		{
#if 0

			// no! only payload
			if ((c&3) != 3)
			{
				memcpy(&pRtpBuf2[bufsize], pRtpBuf, udpRecvdSize);
				bufsize += udpRecvdSize;

			}
			else
			{
				memcpy(&pRtpBuf2[bufsize], pRtpBuf, udpRecvdSize);

				RtpProcess(gpOsip, pRtpBuf2, udpRecvdSize + bufsize, sockRtp);
				bufsize = 0;

			}
			
			c++;
#else
			RtpProcess(gpOsip, pRtpBuf, udpRecvdSize, sockRtp);
#endif

		}
	}
	free(pRtpBuf);
	free(pRtpBuf2);
	return 0;
}