/***************************************************************************************************************************//*
* @file    sip.h
* @author  Maxim Ivanchenko
* @version 1.0
* @date    September, 2014
* @brief   Libosib2 usage
******************************************************************************************************************************/
#pragma once
#include "osip2/osip.h"
/*****************************************************************************************************************************/
void SipProcess(osip_t* osip, char* pBuf, int size, SOCKET sock);
void ProcessNewReqIst(osip_t* osip, osip_event_t *evt, int sock);
void ProcessNewReqNist(osip_t* osip, osip_event_t *evt, int sock);
int BuildResponse(const osip_message_t *request, osip_message_t **response);
/*****************************************************************************************************************************/
