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
/**************************************************************************************************************************//**
* @brief Filling buffers with input RTP data for PlaySamplesThread (the output data is comletely handled by RecSamplesThread thread)
******************************************************************************************************************************/
int RtpProcess(osip_t* osip, char* pRtpBuf, int udpRecvdSize, SOCKET sock)
{
	tRtpPacket		rtp;
	char*			pPrinfStart		= NULL;
	static int		q				= 0;
	signed short*	pUncompressed	= NULL;
	int				err				= 0;
	int				i				= 0;
	static int		rtpsTotal		= 0;
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


#if 1 // recording WAV
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
	tAudioElement* pAudEl1 = NULL;
	tAudioElement* pAudEl2 = NULL;

	pAudEl1 = &gAudioBuf[0];
	pAudEl2 = &gAudioBuf[1];
	if ((pAudEl1->mutex == 0) && (pAudEl1->handleNeeded != 1))
	{
		pAudEl1->mutex = 1;
		for (i = 0; i < rtp.payloadLen; i++)
		{
			pAudEl1->buffer[i] = MuLaw_Decode(rtp.pPayload[i]);
			
		}
		pAudEl1->sizeInBytes = rtp.payloadLen;
		pAudEl1->handleNeeded = 1;
#if defined PLAY_IN_RTP_RCV_THREAD
		PlaySamples(pAudEl1);
		pAudEl1->handleNeeded = 0; // for capability with PlayThread
#endif

		pAudEl1->mutex = 0;

		printf("handled1 %d \r\n", rtpsTotal);
	}
	else if ((pAudEl2->mutex == 0) && (pAudEl2->handleNeeded != 1))
	{
		pAudEl2->mutex = 1;
		for (i = 0; i < rtp.payloadLen; i++)
		{
			pAudEl2->buffer[i] = MuLaw_Decode(rtp.pPayload[i]);
		}
		pAudEl2->sizeInBytes = rtp.payloadLen;
		pAudEl2->handleNeeded = 1;
#if defined PLAY_IN_RTP_RCV_THREAD 
		PlaySamples(pAudEl2);
		pAudEl2->handleNeeded = 0; // for capability with PlayThread
#endif 

		pAudEl2->mutex = 0;

		printf("handled2 %d \r\n", rtpsTotal);

	}
	else
	{
		printf("missed %d \r\n", rtpsTotal);
	}

#endif

	rtpsTotal++;
	free(pUncompressed);
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

	char*		pRtpBuf;
	SOCKET		sockRtp = UdpServerCreate(&pRtpBuf, PORT_RTP);
	SOCKADDR_IN sockInRtp;
	int			udpRecvdSize = 0;

#if defined PLAY_IN_RTP_RCV_THREAD  // need to init if we dont use play thread and play in this thread
	gAudioBuf[0].mutex = 0;
	gAudioBuf[0].handleNeeded = 0;
	gAudioBuf[1].mutex = 0;
	gAudioBuf[1].handleNeeded = 0;

	PlayingInit(&gAudioBuf[0]);
	PlayingInit(&gAudioBuf[1]);
#endif

	gRtpSock = sockRtp;

	while (1)
	{
		// RTP 
		udpRecvdSize = UdpServerProcess(sockRtp, pRtpBuf, &sockInRtp);
		if (udpRecvdSize != 0)
		{
			RtpProcess(gpOsip, pRtpBuf, udpRecvdSize, sockRtp);
		}
	}
	return 0;
}