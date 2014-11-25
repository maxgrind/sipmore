/***************************************************************************************************************************//*
* @file    sip.c
* @author  Maxim Ivanchenko
* @version 1.0
* @date    September, 2014
* @brief   Libosib2 usage
******************************************************************************************************************************/
#include <assert.h>

#include "ip_stack/udp_server.h"
#include "sip/sip.h"
#include "sip/callbacks.h"
#include "lib/rtp/rtp.h"
#include "lib/wav/wav.h"
#include "osip2/osip.h"
#include "lib/audiostreaming/audiostreaming.h"
#include "config.h"
/*****************************************************************************************************************************/
int gWavIsWriting = 0;
tWaveFileParams gWavParams;
tAudioElement gAudioBuf[];
char gSpdPort[6];
char gClientIp[13];
signed short MuLaw_Decode(char number);
/*****************************************************************************************************************************/
void MgsParseTest(pMsg)
{
#define PARSE_TEST_1
	osip_message_t sip;
	osip_message_t* pSip;
	const char* fileName = "C:\\m\\Dropbox\\_sip_proxy\\input\\sip01m";
	FILE* handle; 
#if defined PARSE_TEST_1

	DWORD fileSizeInBytes;
	DWORD bytesRead = 0;
	errno_t err;
	char* pData = (char*) osip_malloc(200000);


	osip_message_init(&pSip);

	err = fopen_s(&handle, fileName, "rb");
	fseek(handle, 0, SEEK_END); // set internal pointer to the end of file 
	fileSizeInBytes = ftell(handle); // size in bytes
	fseek(handle, 0, SEEK_SET); // put the pointer back to the beginnig
	bytesRead = fread(pData, 1, 200000, handle);
	//osip_message_parse(&sip, pData, bytesRead);
	osip_message_parse(pSip, pData, bytesRead);
	pSip = pSip;
#elif defined PARSE_TEST_2

	DWORD fileSizeInBytes;
	DWORD bytesRead = 0;
	errno_t err;
	char* pData = (char*) osip_malloc(200000);

	err = fopen_s(&handle, fileName, "rb");
#endif
}
SOCKET socket2;
/*****************************************************************************************************************************/
extern IN_ADDR gDestIp;
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
	// UDP socket for RTP
	char*		pRtpBuf;
	SOCKET		sockRtp			= UdpServerCreate(&pRtpBuf, PORT_RTP);
	SOCKADDR_IN sockInRtp;
	int			udpRecvdSize	= 0;

	HANDLE WINAPI playThreadHandle;
	HANDLE WINAPI palyThreadMutex;
	int			i;

	_itoa_s(PORT_RTP, gSpdPort, 6, 10); // 6 - max quantity of digits in port value; 10 - radix

	i = osip_init(&pOsip);
	if (i != 0)
	{
		return -1;
	}
	SetCallbacks(pOsip);
	gWavParams.pFileName = "d:\\sipmore.wav";

	gAudioBuf[0].mutex = 0;
	gAudioBuf[0].handleNeeded = 0;
	gAudioBuf[1].mutex = 0;
	gAudioBuf[1].handleNeeded = 0;

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
	gWavParams.sampleFormat.averageBytesPerSecond = 128000;
	gWavParams.sampleFormat.blockAlign = 2;// SignificantBitsPerSample / 8 * NumChannels 
#endif // 0

	unsigned int playThreadPArams[2];
	FileWavCreate(&gWavParams);

	//palyThreadMutex = CreateMutex(NULL, 0, NULL);
	playThreadHandle =  CreateThread(
		NULL, // this thread wouldn't be inherited
		0, // stack size in bytes (0 - defualts stack size of 1 Mb)
		PlaySamplesThread,
		NULL,
		0,
		0
		);
	
	//MgsParseTest();
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

#if 0
			unsigned char* pBuf = (unsigned char*)malloc(udpRecvdSize + 3);
			char* pPrinfStart = pBuf;
			memcpy(pBuf, pRtpBuf, udpRecvdSize);

#if 1 
			for (i = 0; i < udpRecvdSize; i++)
			{
				//printf("0x%02x", pBuf[i]);
				//printf("%#2x ", pBuf[i]);
				printf("%02X ", pBuf[i]);
				//printf("%d ", pBuf[i]);
			}
#else
			for (i = 0; i < udpRecvdSize; i++)
			{
				pPrinfStart += sprintf(pPrinfStart, "%02X", pBuf[i]);
			}
#endif
			//printf("%d\r\n", udpRecvdSize);
			printf("\r\n");
#endif // 0
			tRtpPacket rtp;
			char* pPrinfStart;
			static int q = 0;
			unsigned short* pUncompressed = NULL;
			//pUncompressed = (unsigned short*) malloc(udpRecvdSize * 2);

			RtpParse(pRtpBuf, udpRecvdSize, &rtp);
			pPrinfStart = rtp.pPayload;
			printf("%d: ", rtp.header.ts);
			for (i = 0; i < 10; i++)
			{
				printf("%02X ", rtp.pPayload[i]);
			}
			printf("\r\n");
			q++;

			//if (q>300)
			//{
			//	// stop writing WAV at 300 rtp packets
			//	FileWavFinish(&gWavParams);
			//	gWavIsWriting = 0;
			//	return;
			//}
			//else
			//{
//#if 0 // u-law vs pcm
//
//				
//				pUncompressed = (unsigned short*) malloc(udpRecvdSize * 2);
//
//				if (pUncompressed == NULL)
//				{
//					FileWavFinish(&gWavParams);
//					gWavIsWriting = 0;
//					return;
//				}
//				else
//				{
//					
//					//decode 
//					for (i = 0; i < udpRecvdSize; i++)
//					{
//						pUncompressed[i] = MuLaw_Decode(rtp.pPayload[i]);
//					}
//
//					gWavIsWriting = 1;
//					FileWavAppendData(pUncompressed, udpRecvdSize*2, &gWavParams);
//					free(pUncompressed);
//				}
//
//#else
//				gWavIsWriting = 1;
//				FileWavAppendData(rtp.pPayload, udpRecvdSize, &gWavParams);
//				free(pUncompressed);
//#endif

			//}
#if 0 // writing wav file

			//decode 
			for (i = 0; i < udpRecvdSize; i++)
			{
				pUncompressed[i] = MuLaw_Decode(rtp.pPayload[i]);
			}
			FileWavAppendData(pUncompressed, udpRecvdSize * 2, &gWavParams);
			
#else // stream playing
			if (((&gAudioBuf[0])->mutex == 0) && ((&gAudioBuf[0])->handleNeeded != 1))
			{
				(&gAudioBuf[0])->mutex = 1;
				for (i = 0; i < udpRecvdSize; i++)
				{
					(&gAudioBuf[0])->buffer[i] = MuLaw_Decode(rtp.pPayload[i]);
				}
				(&gAudioBuf[0])->sizeInBytes = udpRecvdSize;
				(&gAudioBuf[0])->handleNeeded = 1;
				(&gAudioBuf[0])->mutex = 0;
			}
			else if (((&gAudioBuf[1])->mutex == 0) && ((&gAudioBuf[1])->handleNeeded != 1))
			{
				(&gAudioBuf[1])->mutex = 1;
				for (i = 0; i < udpRecvdSize; i++)
				{
					(&gAudioBuf[1])->buffer[i] = MuLaw_Decode(rtp.pPayload[i]);
				}
				(&gAudioBuf[1])->sizeInBytes = udpRecvdSize;
				(&gAudioBuf[1])->handleNeeded = 1;
				(&gAudioBuf[1])->mutex = 0;
			}
			else
			{
			//	assert(0);
			}


#endif
			free(pUncompressed);
			RtpPacketDestroy(&rtp);
		}
		
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
