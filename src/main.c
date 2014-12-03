/***************************************************************************************************************************//*
* @file    main.c
* @author  Maxim Ivanchenko
* @brief   main()
******************************************************************************************************************************/
#include <assert.h>

#include "ip_stack/udp_server.h"
#include "sip/sip.h"
#include "rtp/rtp_handling.h"
#include "sip/callbacks.h"
#include "config.h"

#include "osip2/osip.h" // need to be included after sockets

#include "lib/rtp/rtp.h"
#include "lib/wav/wav.h"
#include "lib/audiostreaming/audiostreaming.h"
#include "lib/codec/g711/itu/g711itu.h"
/*****************************************************************************************************************************/
int				gWavIsWriting = 0;
tWaveFileParams gWavParams;
tAudioElement	gAudioBuf[];
osip_t*			gpOsip;
SOCKET			gRtpSock;
char			gSpdPort[6];
char			gClientIp[13];
char			gRtpSessionActive;
HANDLE WINAPI	gPlayThreadHandle;
HANDLE WINAPI	gRecThreadHandle;
HANDLE WINAPI	gPalyThreadMutex[2];
/*****************************************************************************************************************************/
extern IN_ADDR gDestIp;
/*****************************************************************************************************************************/
signed short	MuLaw_Decode(char number);
/*****************************************************************************************************************************/
/***************************************************	M	A	I	N	******************************************************/
/*****************************************************************************************************************************/
int main(int argc, char ** argv, char ** env)
{
	osip_t *	pOsip			= NULL;

	// UDP socket for SIP
	char*		pUdpBuf; 
	SOCKET		sock			= UdpServerCreate(&pUdpBuf, PORT_SIP);
	SOCKADDR_IN sockIn;

	// UDP socket for RTP. todo: create dynamically on server invite receive
	char*		pRtpBuf;
	SOCKET		sockRtp			= UdpServerCreate(&pRtpBuf, PORT_RTP);
	SOCKADDR_IN sockInRtp;

	int			udpRecvdSize	= 0;
	int			i;

	_itoa_s(PORT_RTP, gSpdPort, 6, 10); // 6 - max quantity of digits in port value; 10 - radix
	gRtpSock = sockRtp;
	i = osip_init(&pOsip);
	gpOsip = pOsip;
	if (i != 0)
	{
		return -1;
	}

	SetCallbacks(pOsip);
	gWavParams.pFileName = "d:\\sipmore.wav";



#if 0
	gWavParams.sampleFormat.compressionCode = WAV_FMT_COMP_CODE_G711_ULAW;
	gWavParams.sampleFormat.numberOfChannels = 1;
	gWavParams.sampleFormat.significantBitsPerSample = 8;
	gWavParams.sampleFormat.sampleRate = 8000;
	gWavParams.sampleFormat.averageBytesPerSecond = 64000;
	gWavParams.sampleFormat.blockAlign = 1;// SignificantBitsPerSample / 8 * NumChannels 
#else
	gWavParams.sampleFormat.compressionCode = WAV_FMT_COMP_CODE_PCM;
	gWavParams.sampleFormat.numberOfChannels = 1;
	gWavParams.sampleFormat.significantBitsPerSample = 16;
	gWavParams.sampleFormat.sampleRate = 8000;
	gWavParams.sampleFormat.averageBytesPerSecond = 8000 * 2;
	gWavParams.sampleFormat.blockAlign = 2;// SignificantBitsPerSample / 8 * NumChannels 
#endif // 0

	FileWavCreate(&gWavParams);

	gAudioBuf[0].mutex			= 0;
	gAudioBuf[0].handleNeeded	= 0;
	gAudioBuf[1].mutex			= 0;
	gAudioBuf[1].handleNeeded	= 0;
	gRtpSessionActive			= 0;
	//gPalyThreadMutex[0] = CreateMutex(NULL, 0, NULL);
	//gPalyThreadMutex[1] = CreateMutex(NULL, 0, NULL);

	// playback thread
	gPlayThreadHandle = CreateThread(
		NULL, // this thread wouldn't be inherited
		0, // stack size in bytes (0 - defualts stack size of 1 Mb)
		PlaySamplesThread,
		NULL,
		0,
		0
		);

	// record thread
	gRecThreadHandle = CreateThread(
		NULL, // this thread wouldn't be inherited
		0, // stack size in bytes (0 - defualts stack size of 1 Mb)
		RecSamplesThread,
		NULL,
		0,
		0
		);
//***************************** MAIN CYCLE ************************************//
	while (1)
	{
		// SIP
		udpRecvdSize = UdpServerProcess(sock, pUdpBuf, &sockIn);
		if (udpRecvdSize != 0)
		{
			SipProcess(pOsip, pUdpBuf, udpRecvdSize, sock);
		}

		// RTP 
		udpRecvdSize = UdpServerProcess(sockRtp, pRtpBuf, &sockInRtp);
		if (udpRecvdSize != 0)
		{
			RtpProcess(pOsip, pRtpBuf, udpRecvdSize, sockRtp);
		}
		
		// osip core
		osip_ict_execute(pOsip);
		osip_ist_execute(pOsip);
		osip_nict_execute(pOsip);
		osip_nist_execute(pOsip);
		osip_timers_ict_execute(pOsip);
		osip_timers_ist_execute(pOsip);
		osip_timers_nict_execute(pOsip);
		osip_timers_nist_execute(pOsip);
	}
	return 0;
}
/*****************************************************************************************************************************/
