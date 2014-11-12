#include "ip_stack/udp_server.h"
#include "sip/sip.h"
#include "sip/callbacks.h"
#include "lib/rtp/rtp.h"
#include "lib/wav/wav.h"
#include "osip2/osip.h"
#include "config.h"

//#if defined _WIN32_WINNT_WIN7 
//#define M_HOME_PC
//#elif defined(_WIN32_WINNT_WIN8) || defined(_WIN32_WINNT_WINBLUE)
//#dfine M_WORK_LAPTOP
//#endif


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

extern IN_ADDR gDestIp;
/***********************************************************************************************************************/
/***********************************************	M	A	I	N	****************************************************/
/***********************************************************************************************************************/
int main(int argc, char ** argv, char ** env)
{
	osip_t * pOsip = NULL;
	int i;
	char* pUdpBuf; 
	SOCKET sock = UdpServerCreate(&pUdpBuf, PORT_SIP);
	SOCKADDR_IN sockIn;

	char* pRtpBuf;
	SOCKET sockRtp = UdpServerCreate(&pRtpBuf, PORT_RTP);
	SOCKADDR_IN sockInRtp;

	//SOCKET socketTx = UdpServerCreate(&pUdpBuf);
	int updRecvdSize = 0;
	char * pSendBuf = "this is glassjaw";

	i = osip_init(&pOsip);
	if (i != 0)	return -1;
	SetCallbacks(pOsip);

	unsigned int ds = htonl(0x1234578);
	
	tWaveFileParams wavParams;
	wavParams.pFileName = "d:\\sipmore.wav";
	wavParams.sampleFormat.compressionCode = WAV_FMT_COMP_CODE_G711_ULAW;
	wavParams.sampleFormat.numberOfChannels = 1;
	wavParams.sampleFormat.significantBitsPerSample = 8;
	wavParams.sampleFormat.sampleRate = 8000;
	wavParams.sampleFormat.averageBytesPerSecond = 64000;
	wavParams.sampleFormat.blockAlign = 1;// SignificantBitsPerSample / 8 * NumChannels

	FileWavCreate(&wavParams);
	
	//MgsParseTest();
	while (1)
	{
		// SIP
		updRecvdSize = UdpServerProcess(sock, pUdpBuf, &sockIn);
		if (updRecvdSize != 0)
		{
			SipProcess(pOsip, pUdpBuf, updRecvdSize, sock);
		}

		// RTP
		updRecvdSize = UdpServerProcess(sockRtp, pRtpBuf, &sockInRtp);
		if (updRecvdSize != 0)
		{
#if 0
			unsigned char* pBuf = (unsigned char*)malloc(updRecvdSize + 3);
			char* pPrinfStart = pBuf;
			memcpy(pBuf, pRtpBuf, updRecvdSize);

#if 1 
			for (i = 0; i < updRecvdSize; i++)
			{
				//printf("0x%02x", pBuf[i]);
				//printf("%#2x ", pBuf[i]);
				printf("%02X ", pBuf[i]);
				//printf("%d ", pBuf[i]);
			}
#else
			for (i = 0; i < updRecvdSize; i++)
			{
				pPrinfStart += sprintf(pPrinfStart, "%02X", pBuf[i]);
			}
#endif
			//printf("%d\r\n", updRecvdSize);
			printf("\r\n");
#endif // 0
			tRtpPacket rtp;
			char* pPrinfStart;
			static int q = 0;

			RtpParse(pRtpBuf, updRecvdSize, &rtp);
			pPrinfStart = rtp.pPayload;
			printf("%d: ", rtp.header.ts);
			for (i = 0; i < 10; i++)
			{
				printf("%02X ", rtp.pPayload[i]);
				//pPrinfStart += sprintf(pPrinfStart, "%02X ", rtp.pPayload[i]);
			}
			printf("\r\n");
			q++;

			if (q==5)
			{ // send dummy RTP packet back (microphone imitation) to get ACK on it 
				char *pPacketBack;
				int packBackLen;
				RtpCompose(5026, 3545156, rtp.pPayload, rtp.len, &pPacketBack, &packBackLen);
				UdpSend(pPacketBack, packBackLen, gDestIp, PORT_RTP);
			}
			if (q>300)
			{
				FileWavFinish(&wavParams);
				return;
			}
			else
			{
				FileWavAppendData(rtp.pPayload, updRecvdSize, &wavParams);
			}


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

