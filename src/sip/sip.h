/**************************************************************************************************************************//**
* @file    sip.h
* @author  Maxim Ivanchenko
* @brief   Libosib2 sip usage
******************************************************************************************************************************/
#pragma once
#include "osip2/osip.h"
/*****************************************************************************************************************************/
void SipProcess(osip_t* osip, char* pBuf, int size, SOCKET sock);
void ProcessNewReqIst(osip_t* osip, osip_event_t *evt, int sock);
void ProcessNewReqNist(osip_t* osip, osip_event_t *evt, int sock);
int BuildResponse(const osip_message_t *request, osip_message_t **response);
/*****************************************************************************************************************************/
