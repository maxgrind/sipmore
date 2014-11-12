/***************************************************************************************************************************//*
 * @file    callbacks.c
 * @author  Maxim Ivanchenko
 * @version 1.0
 * @date    September, 2014
 * @brief   RFC3550 RTP protocol implementation  
******************************************************************************************************************************/
#include "ip_stack/udp_server.h"
#include "sip/sip.h"
#include "lib/rtp/rtp.h"
#include "lib/wav/wav.h"
#include "osip2/osip.h"
#include "config.h"
/******************************************************************************************************************************/
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
	
}
void CbKillTransactionServerInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	int i;
	//int i = osip_remove_transaction(pOsip, pTranaction);
	return;
}
void CbKillTransactionClientNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	return;
}
void CbKillTransactionServerNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
	int i;
	return;
}
void CbTransportErrorClientInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg)
{
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
	char pBody2[] = "\
v=0\r\n\
o=1415058944 1 IN IP4 192.168.43.13\r\n\
s=sipmore\r\n\
c=IN IP4 192.168.43.13\r\n\
t=0 0\r\n\
m=audio 7076 RTP/AVP 0 101\r\n\
a=rtpmap:101 telephone-event/8000\r\n\
a=fmtp:101 0-15\r\n\
";
//m=audio 5062 RTP/AVP 0\r\n\
//a=rtpmap:0 PCMU/8000/ 1\r\n\
//a=rtpmap:110 speex/8000\r\n\	
//a=fmtp:110 vbr=on


//m=audio 7076 RTP/AVP 0 101\r\n\
//a=rtpmap:101 telephone-event/8000\r\n\
//a=fmtp : 101 0-15\r\n\
//					   ^ means PCMU
	// 5062 7076 
	// 192.168.1.5  192.168.43.13

	pBody->length = sizeof(pBody2);//128;

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