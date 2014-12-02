/**************************************************************************************************************************//**
* @file    sip.c
* @author  Maxim Ivanchenko
* @brief   Libosib2 sip usage
******************************************************************************************************************************/
#include <stdio.h>
#include "ip_stack/udp_server.h"
#include "sip/sip.h"
#include "osip2/osip.h"
#include "config.h"
/*****************************************************************************************************************************/
void SipProcess(osip_t* osip, char* pBuf, int size, SOCKET sock)
{
	osip_event_t *evt = osip_parse(pBuf, size); // doesn't need to be free
	int rc;

	if (evt == NULL)
	{
		OutputDebugString(TEXT("unknown message received\r\n"));
		return;
	}

	rc = osip_find_transaction_and_add_event(osip, evt);
	if (0 != rc)
	{
		OutputDebugString(TEXT("this event has no transaction, create a new one\r\n"));
		if(evt->type == RCV_REQINVITE)
		{
			ProcessNewReqIst(osip, evt, sock);
		}
		else
		{
			ProcessNewReqNist(osip, evt, sock);
		};
	}
}
/*****************************************************************************************************************************/
void ProcessNewReqIst(osip_t* osip, osip_event_t *evt, int sock)
{
	osip_transaction_t *tran;
	osip_transaction_init(&tran, IST, osip, evt->sip);
	//osip_transaction_set_in_socket (tran, socket);
	osip_transaction_set_out_socket(tran, sock);
	osip_transaction_set_your_instance(tran, osip);// store osip in transaction for later usage
	osip_transaction_add_event(tran, evt);
}
/*****************************************************************************************************************************/
void ProcessNewReqNist(osip_t* osip, osip_event_t *evt, int sock)
{
	osip_transaction_t *tran;
	osip_transaction_init(&tran, NIST, osip, evt->sip);
	osip_transaction_set_out_socket(tran, sock);
	osip_transaction_set_your_instance(tran, osip);
	osip_transaction_add_event(tran, evt);
}
/*****************************************************************************************************************************/
int BuildResponse(const osip_message_t *request, osip_message_t **response)
{
	osip_message_t *msg = NULL;
	char port[6];
	char* pContact = osip_malloc(strlen(SERV_IP_ADDR) + strlen(USER_NAME) + sizeof(port) + 7 + 10 ); // 7 is "sip:...@..:. \0"    10 - zapas
	osip_message_init(&msg);

	osip_from_clone(request->from, &msg->from);
	osip_to_clone(request->to, &msg->to);
	osip_cseq_clone(request->cseq, &msg->cseq);
	osip_call_id_clone(request->call_id, &msg->call_id);

	int pos = 0;//copy vias from request to response
	while (!osip_list_eol(&request->vias, pos))
	{
		osip_via_t *via;
		osip_via_t *via2;

		via = (osip_via_t *) osip_list_get(&request->vias, pos);
		int i = osip_via_clone(via, &via2);
		if (i != 0)
		{
			osip_message_free(msg);
			return i;
		}
		osip_list_add(&(msg->vias), via2, -1);
		pos++;
	}	
	osip_message_set_max_forwards(msg, osip_strdup("70"));
	osip_to_set_tag(msg->to, osip_strdup("4893693")); // set to tag in response. todo: randomize
	osip_message_set_version(msg, osip_strdup("SIP/2.0"));
	// create contact
	strcat(pContact, osip_strdup("sip:"));
	strcat(pContact, osip_strdup(USER_NAME));
	strcat(pContact, osip_strdup("@"));
	strcat(pContact, osip_strdup(SERV_IP_ADDR));
	osip_message_set_contact(msg, osip_strdup(pContact));
	
	osip_free(pContact);
	osip_message_set_user_agent(msg, osip_strdup("SipMore/0.1"));

	*response = msg;
	return 0;
}
/*****************************************************************************************************************************/

