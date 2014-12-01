/***************************************************************************************************************************//*
 * @file    callbacks.h
 * @author  Maxim Ivanchenko
 * @brief   WAV file format simple implementation  
******************************************************************************************************************************/
#ifndef __LOBOSIP2_CALBACKS_H__
#define __LOBOSIP2_CALBACKS_H__
/*****************************************************************************************************************************/
#include "osip2/osip.h"
/*****************************************************************************************************************************/
int CbUdpSendMessage(osip_transaction_t * pTransaction, osip_message_t * pMessage, char * pChar, int port, int out_socket);
void CbKillTransactionClientInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
void CbKillTransactionServerInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
void CbKillTransactionClientNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
void CbKillTransactionServerNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
void CbTransportErrorClientInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
void CbTransportErrorServerInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
void CbTransportErrorClientNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
void CbTransportErrorServerNotInvite(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbRcvdClientInvite1xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbRcvdClientInvite2xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbRcvdClientInvite3456xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbRcvdClientNotInvite1xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbRcvdClientNotInvite2xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbRcvdClientNotInvite3456xx(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
/*****************************************************************************************************************************/
int CbOnIstInviteRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbOnIstAckRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbOnNistRegisterRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbOnNistByeRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbOnNistCancelRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbOnNistInfoRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbOnNistOptionsRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbOnNistSubscribeRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbOnNistNotifyRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
int CbOnNistUnknownRequestRcvd(int type, osip_transaction_t * pTranaction, osip_message_t * pMsg);
/*****************************************************************************************************************************/
void SetCallbacks(osip_t* osip);
/*****************************************************************************************************************************/
#endif // __LOBOSIP2_CALBACKS_H__