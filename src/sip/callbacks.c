/**************************************************************************************************************************//**
 * @file    callbacks.c
 * @author  Maxim Ivanchenko
 * @brief   RFC3550 RTP protocol implementation  
******************************************************************************************************************************/
#include <assert.h>
#include "ip_stack/udp_server.h"
#include "sip/sip.h"
#include "lib/rtp/rtp.h"
#include "lib/wav/wav.h"
#include "osip2/osip.h"
#include "osipparser2/sdp_message.h"
#include "config.h"
/******************************************************************************************************************************/
IN_ADDR					gDestIp;
extern char				gSpdPort[6];
extern tWaveFileParams	gWavParams;
extern int				gWavIsWriting;
extern osip_t*			gpOsip;
extern char				gRtpSessionActive;
/******************************************************************************************************************************/
int CbUdpSendMessage(osip_transaction_t * pTransaction, osip_message_t * pMessage, char * pChar, int port, int out_socket)
{
	char *msgP;
	size_t msgLen;
	osip_message_to_str(pMessage, &msgP, &msgLen);
	UdpSend(msgP, msgLen, gDestIp, PORT_SIP);

	return 0;
}
/******************************************************************************************************************************/
void CbKillTransactionClientInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	int i;
	return;
}
void CbKillTransactionServerInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	int i;
	if (gpOsip != NULL)
	{
		i = osip_remove_transaction(gpOsip, pTranaction);
	}
	return;
}
void CbKillTransactionClientNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	int i;
	return;
}
void CbKillTransactionServerNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	int i;
	if (gpOsip != NULL)
	{
		i = osip_remove_transaction(gpOsip, pTranaction);
	}
	return;
}
void CbTransportErrorClientInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	int i;
	return;
}
void CbTransportErrorServerInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return;
}
void CbTransportErrorClientNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return;
}
void CbTransportErrorServerNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return;
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
	osip_message_t* msgParsed = NULL;
	osip_message_t *response = NULL;
	osip_event_t *evt = NULL;
	osip_body_t* pBody;
	sdp_message_t* pSdpMsgInput = NULL;

	int bodyLen = 0;
	int bodiesQntty = pMsg->bodies.nb_elt;
	int* pPackedSdpMsg = pMsg->bodies.node->element;


	sdp_media_t* pSdpMedia;
	char* pSdpMediaType = (char*) osip_malloc(20);
	int zeroMeansEqual;

	char* pSdpPort = (char*) osip_malloc(20);
	char* pSdpStr = NULL;

	sdp_message_init(&pSdpMsgInput);
	sdp_message_parse(pSdpMsgInput, *pPackedSdpMsg); // (const char*)


	sdp_media_init(&pSdpMedia);
	pSdpMediaType = sdp_message_m_media_get(pSdpMsgInput, 0);

	zeroMeansEqual = strcmp(pSdpMediaType, "audio");
	osip_free(pSdpMediaType);
	if (zeroMeansEqual != 0)
	{
		// todo: send CANCEL
		printf("Other than \"audio\" format received. Not supported for now");
	}


	pSdpPort = sdp_message_m_port_get(pSdpMsgInput, 0);
	strcpy(gSpdPort, pSdpPort);

	BuildResponse(pMsg, &response);//ringing
	osip_message_set_status_code(response, SIP_RINGING);
	evt = osip_new_outgoing_sipmessage(response);
	osip_message_set_reason_phrase(response, osip_strdup("Ringing"));
	osip_transaction_add_event(pTranaction, evt);

	BuildResponse(pMsg, &response);//ok
	osip_message_set_status_code(response, SIP_OK);

	osip_body_init(&pBody);

	osip_message_set_content_type(response, "application/sdp");

	sdp_message_t* pSdp;
	sdp_message_init(&pSdp);
	sdp_message_v_version_set(pSdp, osip_strdup("0"));
	sdp_message_o_origin_set(pSdp, osip_strdup(USER_NAME), osip_strdup("281085"), osip_strdup("0"), osip_strdup("0"), osip_strdup("IP4"), osip_strdup(SERV_IP_ADDR));
	sdp_message_s_name_set(pSdp, osip_strdup(UA_NAME));
	sdp_message_t_time_descr_add(pSdp, osip_strdup("0"), osip_strdup("0"));
	//sdp_message_c_connection_add(pSdp, 0, osip_strdup("IN"), osip_strdup("IP4"), osip_strdup(SERV_IP_ADDR), NULL, NULL);
	sdp_message_m_media_add(pSdp, osip_strdup("audio"), osip_strdup(pSdpPort), NULL, osip_strdup("RTP/AVP 0 101")); // todo:  0 101??
	sdp_message_m_payload_add(pSdp, 1, osip_strdup("rtpmap:0 PCMU/8000/1"));
	sdp_message_m_payload_add(pSdp, 2, osip_strdup("rtpmap:101 telephone-event/8000"));
	sdp_message_m_payload_add(pSdp, 3, osip_strdup("fmtp:101 0-15"));
	sdp_message_m_payload_add(pSdp, 4, osip_strdup("prime:100"));
	sdp_message_c_connection_add(pSdp, 0, osip_strdup("IN"), osip_strdup("IP4"), osip_strdup(SERV_IP_ADDR), NULL, NULL); // why here, after media?
	osip_free(pSdpPort);

	sdp_message_to_str(pSdp, &pSdpStr);
	if (pSdpStr != NULL)
	{
		osip_message_set_body(response, pSdpStr, strlen(pSdpStr));
		osip_free(pSdpStr);
	}
	else
	{
	}
	
//m=audio 7076 RTP/AVP 0 101\r\n\
//					   ^ 0 - means PCMU (98 - dynamic RTP / AVP payload)
//						 ^ 101 - means DTMF support
// a=ptime:20  means samples per packet / packetization time
// see rfc 3551: RTP Profile for Audio and Video Conferences with Minimal Control. or here http://www.iana.org/assignments/rtp-parameters/rtp-parameters.xhtml for payload types
// and rfc 4733: RTP Payload for DTMF Digits, Telephony Tones, and Telephony Signals
//a=rtpmap:101 telephone-event/8000\r\n\
//a=fmtp : 101 0-15\r\n\

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

	gRtpSessionActive = 0;

	BuildResponse(pMsg, &response);//ok
	osip_message_set_status_code(response, SIP_OK);
	evt = osip_new_outgoing_sipmessage(response);
	osip_message_set_reason_phrase(response, osip_strdup("Ok"));
	osip_transaction_add_event(pTranaction, evt);
	
	if (gWavIsWriting == 1)
	{
		FileWavFinish(&gWavParams);
		gWavIsWriting = 0;
	}
	return 0;
}
int CbOnNistCancelRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	osip_message_t *response = NULL;
	osip_event_t *evt = NULL;

	//gRtpSessionActive = 0;

	BuildResponse(pMsg, &response);//ok
	osip_message_set_status_code(response, SIP_OK);
	evt = osip_new_outgoing_sipmessage(response);
	osip_message_set_reason_phrase(response, osip_strdup("Ok"));
	osip_transaction_add_event(pTranaction, evt);

	if (gWavIsWriting == 1)
	{
		FileWavFinish(&gWavParams);
		gWavIsWriting = 0;
	}
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
	osip_set_kill_transaction_callback(osip, OSIP_IST_KILL_TRANSACTION, CbKillTransactionServerInvite);
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
	osip_set_message_callback(osip, OSIP_NICT_STATUS_1XX_RECEIVED, CbRcvdClientNotInvite1xx);
	osip_set_message_callback(osip, OSIP_NICT_STATUS_2XX_RECEIVED, CbRcvdClientNotInvite2xx);
	osip_set_message_callback(osip, OSIP_NICT_STATUS_3XX_RECEIVED, CbRcvdClientNotInvite3456xx);
	osip_set_message_callback(osip, OSIP_NICT_STATUS_4XX_RECEIVED, CbRcvdClientNotInvite3456xx);
	osip_set_message_callback(osip, OSIP_NICT_STATUS_5XX_RECEIVED, CbRcvdClientNotInvite3456xx);
	osip_set_message_callback(osip, OSIP_NICT_STATUS_6XX_RECEIVED, CbRcvdClientNotInvite3456xx);
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
/******************************************************************************************************************************/
