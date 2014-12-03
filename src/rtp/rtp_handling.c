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
#include "config.h"

#include <windows.h>
/*****************************************************************************************************************************/
extern tAudioElement	gAudioBuf[];
extern osip_t*			gpOsip;
extern HANDLE WINAPI	gPalyThreadMutex[2];
extern char				gRtpSessionActive;
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

	//pPrinfStart = rtp.pPayload;
	//printf("%d: ", rtp.header.ts);
	//for (i = 0; i < 10; i++)
	//{
	//	//printf("%02X ", rtp.pPayload[i]);
	//}
	//printf("\r\n");
	//q++;

	//if (q>300)
	//{
	//	// stop writing WAV at 300 rtp packets
	//	FileWavFinish(&gWavParams);
	//	gWavIsWriting = 0;
	//	return;
	//}
	//else
	//{

#if 0 // recording WAV
#if 1 // u-law vs pcm


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

#if 1  // stream playing
	static int rtpsTotal = 0;

	if (((&gAudioBuf[0])->mutex == 0) && ((&gAudioBuf[0])->handleNeeded != 1))
	{
		(&gAudioBuf[0])->mutex = 1;
		for (i = 0; i < rtp.payloadLen; i++)
		{
			(&gAudioBuf[0])->buffer[i] = MuLaw_Decode(rtp.pPayload[i]);

		}
		(&gAudioBuf[0])->sizeInBytes = rtp.payloadLen;
		(&gAudioBuf[0])->handleNeeded = 1;
		(&gAudioBuf[0])->mutex = 0;
		static h1 = 0;
		h1++;
		//if (h1%100==0)
		printf("handled1 %d \r\n", rtpsTotal);
	}
	else if (((&gAudioBuf[1])->mutex == 0) && ((&gAudioBuf[1])->handleNeeded != 1))
	{
		(&gAudioBuf[1])->mutex = 1;
		for (i = 0; i < rtp.payloadLen; i++)
		{
			(&gAudioBuf[1])->buffer[i] = MuLaw_Decode(rtp.pPayload[i]);
		}
		(&gAudioBuf[1])->sizeInBytes = rtp.payloadLen;
		(&gAudioBuf[1])->handleNeeded = 1;
		(&gAudioBuf[1])->mutex = 0;
		static h2 = 0;
		h2++;
		//if (h2 % 100 == 0)
		printf("handled2 %d \r\n", rtpsTotal);

	}
	else
	{
		static int miss = 0;
		miss++;
		printf("missed %d \r\n", rtpsTotal);
	}



#endif


	rtpsTotal++;
	//printf("rtps %d \r\n", rtpsTotal);

	free(pUncompressed);
	RtpPacketDestroy(&rtp);
	return 0;
}