#include "ip_stack/udp_server.h"
#include "sip/sip.h"
#include "lib/rtp/rtp.h"
#include "lib/wav/wav.h"
#include "osip2/osip.h"

#define PORT_SIP 5060   
#define PORT_RTP 5070 // 5062  5070 7076

//#if defined _WIN32_WINNT_WIN7 
//#define M_HOME_PC
//#elif defined(_WIN32_WINNT_WIN8) || defined(_WIN32_WINNT_WINBLUE)
//#dfine M_WORK_LAPTOP
//#endif


IN_ADDR gDestIp;

int CbUdpSendMessage(osip_transaction_t * pTransaction, osip_message_t * pMessage, char * pChar, int port, int out_socket)
{
	char *msgP;
	size_t msgLen;
	osip_message_to_str(pMessage, &msgP, &msgLen);
	UdpSend(msgP, msgLen, gDestIp, PORT_SIP);

	return 0;
}

void CbKillTransactionClientInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
void CbKillTransactionServerInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
void CbKillTransactionClientNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
void CbKillTransactionServerNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
void CbTransportErrorClientInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
void CbTransportErrorServerInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
void CbTransportErrorClientNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
void CbTransportErrorServerNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbRcvdClientInvite1xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbRcvdClientInvite2xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbRcvdClientInvite3456xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbRcvdClientNotInvite1xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbRcvdClientNotInvite2xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbRcvdClientNotInvite3456xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
/***********************************************************************************************************************/
int CbOnIstInviteRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	osip_message_t *response = NULL;
	osip_event_t *evt = NULL;
	osip_body_t* pBody;
	int bodyLen = 0;

	//BuildResponse(pMsg, &response);//trying
	//osip_message_set_status_code(response, SIP_TRYING);
	//evt = osip_new_outgoing_sipmessage(response);
	//osip_message_set_reason_phrase(response, osip_strdup("Trying"));
	//osip_transaction_add_event(pTranaction, evt);

	//BuildResponse(pMsg, &response);//dialog establishement
	//osip_message_set_status_code(response, 101);
	//evt = osip_new_outgoing_sipmessage(response);
	//osip_message_set_reason_phrase(response, osip_strdup("Dialog Establishement"));
	//osip_transaction_add_event(pTranaction, evt);

	BuildResponse(pMsg, &response);//ringing
	osip_message_set_status_code(response, SIP_RINGING);
	evt = osip_new_outgoing_sipmessage(response);
	osip_message_set_reason_phrase(response, osip_strdup("Ringing"));
	osip_transaction_add_event(pTranaction, evt);

	BuildResponse(pMsg, &response);//ok
	osip_message_set_status_code(response, SIP_OK);
	osip_body_init(&pBody);
	
	osip_message_set_content_type(response, "application/sdp");
	//osip_message_set_content_length(response, "128");

	//osip_body_set_header(pBody, "v", "0");
	//osip_body_set_header(pBody, "o", "-1415058944 1 IN IP4 192.168.1.5");
	//osip_body_set_header(pBody, "c", "IN IP4 192.168.1.5");
	//osip_body_set_header(pBody, "m", "audio 5062 RTP/AVP 0 101");
	//osip_body_set_header(pBody, "a", "rtpmap:0 PCMU/8000/1");
	//osip_body_to_str(pBody, )
	char pBody2 []= "\
v=0\r\n\
o=- 1415058944 1 IN IP4 192.168.1.5\r\n\
c=IN IP4 192.168.1.5\r\n\
t=0 0\r\n\
m=audio 5070 RTP/AVP 0 101\r\n\
a=rtpmap:0 PCMU/8000/ 1\r\n\
";

	// 5062 5070 7076 
		

	pBody->length = 128;

	osip_message_set_body(response, pBody2, sizeof(pBody2)-1);

	evt = osip_new_outgoing_sipmessage(response);
	osip_message_set_reason_phrase(response, osip_strdup("Ok"));
	osip_transaction_add_event(pTranaction, evt);

	//osip_thread_create(0, Notify, tran);// start another thread to notify user the incoming call
}
int CbOnIstAckRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	osip_message_t *response = NULL;
	osip_event_t *evt = NULL;

	BuildResponse(pMsg, &response);
	osip_message_set_status_code(response, SIP_OK);
	evt = osip_new_outgoing_sipmessage(response);
	osip_message_set_reason_phrase(response, osip_strdup("Ok"));
	osip_transaction_add_event(pTranaction, evt);
	return 0;

}
int CbOnNistRegisterRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbOnNistByeRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	osip_message_t *response = NULL;
	osip_event_t *evt = NULL;

	BuildResponse(pMsg, &response);//ok
	osip_message_set_status_code(response, SIP_OK);
	evt = osip_new_outgoing_sipmessage(response);
	osip_message_set_reason_phrase(response, osip_strdup("Ok"));
	osip_transaction_add_event(pTranaction, evt);
	return 0;
}
int CbOnNistCancelRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbOnNistInfoRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbOnNistOptionsRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbOnNistSubscribeRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbOnNistNotifyRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
int CbOnNistUnknownRequestRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return 0;
}
/***********************************************************************************************************************/
/***********************************************************************************************************************/
void SetCallbacks(osip_t* osip)
{
	// callback called when a SIP message must be sent.
	osip_set_cb_send_message(osip, CbUdpSendMessage);
	// callback called when a SIP transaction is TERMINATED.
	osip_set_kill_transaction_callback(osip, OSIP_ICT_KILL_TRANSACTION, CbKillTransactionClientInvite);
	osip_set_kill_transaction_callback(osip, OSIP_NIST_KILL_TRANSACTION, CbKillTransactionServerInvite);
	osip_set_kill_transaction_callback(osip, OSIP_NICT_KILL_TRANSACTION, CbKillTransactionClientNotInvite);
	osip_set_kill_transaction_callback(osip, OSIP_NIST_KILL_TRANSACTION, CbKillTransactionServerNotInvite);
	// callback called when the callback to send message have failed.
	osip_set_transport_error_callback(osip, OSIP_ICT_TRANSPORT_ERROR, CbTransportErrorClientInvite);
	osip_set_transport_error_callback(osip, OSIP_IST_TRANSPORT_ERROR, CbTransportErrorServerInvite);
	osip_set_transport_error_callback(osip, OSIP_NICT_TRANSPORT_ERROR, CbTransportErrorClientNotInvite);
	osip_set_transport_error_callback(osip, OSIP_NIST_TRANSPORT_ERROR, CbTransportErrorServerNotInvite);
	// callback called when a received answer has been accepted by the transaction.
	osip_set_message_callback(osip, OSIP_ICT_STATUS_1XX_RECEIVED, CbRcvdClientInvite1xx);
	osip_set_message_callback(osip, OSIP_ICT_STATUS_2XX_RECEIVED, CbRcvdClientInvite2xx);
	osip_set_message_callback(osip, OSIP_ICT_STATUS_3XX_RECEIVED, CbRcvdClientInvite3456xx);
	osip_set_message_callback(osip, OSIP_ICT_STATUS_4XX_RECEIVED, CbRcvdClientInvite3456xx);
	osip_set_message_callback(osip, OSIP_ICT_STATUS_5XX_RECEIVED, CbRcvdClientInvite3456xx);
	osip_set_message_callback(osip, OSIP_ICT_STATUS_6XX_RECEIVED, CbRcvdClientInvite3456xx);
	// callback called when a received answer has been accepted by the transaction.
	osip_set_message_callback(osip, OSIP_NICT_STATUS_1XX_RECEIVED,CbRcvdClientNotInvite1xx);
	osip_set_message_callback(osip, OSIP_NICT_STATUS_2XX_RECEIVED,CbRcvdClientNotInvite2xx);
	osip_set_message_callback(osip, OSIP_NICT_STATUS_3XX_RECEIVED,CbRcvdClientNotInvite3456xx);
	osip_set_message_callback(osip, OSIP_NICT_STATUS_4XX_RECEIVED,CbRcvdClientNotInvite3456xx);
	osip_set_message_callback(osip, OSIP_NICT_STATUS_5XX_RECEIVED,CbRcvdClientNotInvite3456xx);
	osip_set_message_callback(osip, OSIP_NICT_STATUS_6XX_RECEIVED,CbRcvdClientNotInvite3456xx);
	// callback called when a received request has been accepted by the transaction.
	osip_set_message_callback(osip, OSIP_IST_INVITE_RECEIVED, CbOnIstInviteRcvd);
	osip_set_message_callback(osip, OSIP_IST_ACK_RECEIVED, CbOnIstAckRcvd);
	osip_set_message_callback(osip, OSIP_NIST_REGISTER_RECEIVED, CbOnNistRegisterRcvd);
	osip_set_message_callback(osip, OSIP_NIST_BYE_RECEIVED, CbOnNistByeRcvd);
	osip_set_message_callback(osip, OSIP_NIST_CANCEL_RECEIVED, CbOnNistCancelRcvd);
	osip_set_message_callback(osip, OSIP_NIST_INFO_RECEIVED, CbOnNistInfoRcvd);
	osip_set_message_callback(osip, OSIP_NIST_OPTIONS_RECEIVED, CbOnNistOptionsRcvd);
	osip_set_message_callback(osip, OSIP_NIST_SUBSCRIBE_RECEIVED, CbOnNistSubscribeRcvd);
	osip_set_message_callback(osip, OSIP_NIST_NOTIFY_RECEIVED, CbOnNistNotifyRcvd);
	osip_set_message_callback(osip, OSIP_NIST_UNKNOWN_REQUEST_RECEIVED, CbOnNistUnknownRequestRcvd);
	return;
}

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

