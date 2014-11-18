/***************************************************************************************************************************//*
* @file    sip.c
* @author  Maxim Ivanchenko
* @version 1.0
* @date    September, 2014
* @brief   Libosib2 usage
******************************************************************************************************************************/
#include <stdio.h>
#include "ip_stack/udp_server.h"
#include "sip/sip.h"
#include "osip2/osip.h"
/*****************************************************************************************************************************/
void SipProcess(osip_t* osip, char* pBuf, int size, SOCKET sock)
{
	osip_event_t *evt = osip_parse(pBuf, size);
	int rc;
	char firstLetter = pBuf[0];

	rc = osip_find_transaction_and_add_event(osip, evt);
	if (0 != rc)
	{
		OutputDebugString(TEXT("this event has no transaction, create a new one."));
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
	osip_to_set_tag(msg->to, osip_strdup("4893693")); // set to tag in response
	osip_message_set_version(msg, osip_strdup("SIP/2.0"));
//#ifdef M_HOME_PC
	//osip_message_set_contact(msg, osip_strdup("sip:pc@192.168.1.5"));
//#else
	//osip_message_set_contact(msg, osip_strdup("sip:pc@192.168.152.33"));
	//osip_message_set_contact(msg, osip_strdup("sip:pc@192.168.43.13"));
	osip_message_set_contact(msg, osip_strdup("sip:pc@192.168.1.50"));
//#endif


	osip_message_set_user_agent(msg, osip_strdup("SipMore/0.1"));

	*response = msg;
	return 0;
}
/*****************************************************************************************************************************/
#if 0

int BuildResponse200Ok(const osip_message_t *request, osip_message_t **response)
{
	osip_message_t *msg = NULL;
	osip_body_t* pBody;

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

		via = (osip_via_t *)osip_list_get(&request->vias, pos);
		int i = osip_via_clone(via, &via2);
		if (i != 0)
		{
			osip_message_free(msg);
			return i;
		}
		osip_list_add(&(msg->vias), via2, -1);
		pos++;
	}

	osip_to_set_tag(msg->to, osip_strdup("4893693")); // set to tag in response
	osip_message_set_version(msg, osip_strdup("SIP/2.0"));
	//osip_message_set_contact(msg, osip_strdup("sip:pc@192.168.152.33"));
	osip_message_set_contact(msg, osip_strdup("sip:pc@192.168.43.13"));
	//osip_message_set_contact(msg, osip_strdup("sip:pc@192.168.1.50"));
	osip_message_set_user_agent(msg, osip_strdup("SipMore/3.2.1 (eXosip2/3.3.0)"));


	osip_body_init(&pBody);
	osip_body_clone(request->bodies.nb_elt, pBody);
	osip_body_set();

	*response = msg;
	return 0;
}
#endif // 0
/*****************************************************************************************************************************/

