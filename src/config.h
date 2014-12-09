/***************************************************************************************************************************//*
* @file    config.h
* @author  Maxim Ivanchenko
* @brief   Common project config
******************************************************************************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__
/*****************************************************************************************************************************/
#include <stdio.h>

#ifdef _WIN32
 #include <stdlib.h>
#else
 todo: #define _itoa_s xxx
#endif
/*****************************************************************************************************************************/
#define PORT_SIP 5060   
#define PORT_RTP 7076 // 5062  7076
/*****************************************************************************************************************************/
#define SERV_UDP_PORT 5060//6543
//#define SERV_TCP_PORT 6543

#ifdef M_HOME_PC
#define SERV_IP_ADDR "192.168.1.5"
#else
#if 0
 #define SERV_IP_ADDR "192.168.1.50"
#else
 #define SERV_IP_ADDR "192.168.43.13"
#endif
#endif
/*****************************************************************************************************************************/
#define UA_NAME			"sipmore/0.1"
#define USER_NAME		"MorSvyazAvtomatika"
/*****************************************************************************************************************************/
#define HTONL(A) ((((unsigned int)(A) & 0xff000000) >> 24) | \
	(((unsigned int)(A)& 0x00ff0000) >> 8) | \
	(((unsigned int)(A)& 0x0000ff00) << 8) | \
	(((unsigned int)(A)& 0x000000ff) << 24))
#endif // __CONFIG_H__