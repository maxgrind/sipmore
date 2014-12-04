/**************************************************************************************************************************//**
* @file    rtp_handling.h
* @author  Maxim Ivanchenko
* @brief   rtp handling 
******************************************************************************************************************************/
#pragma once
#include "lib/rtp/rtp.h"
#include "osip2/osip.h"
#if defined(_WIN32) // Visual Studio and MinGW
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#endif
#include <stdio.h>
/*****************************************************************************************************************************/
// recording params. todo: make them configurable
#define SAMPLES_IN_RTP_PACKET	160
#define RTP_SAMPLE_SIZE_BYTE	8	 // PCMU encoded
#define RTP_INTERVAL_MS			20
#define SAMPLE_RATE				8000 // PCMU

#define PLAY_IN_RTP_RCV_THREAD
/*****************************************************************************************************************************/
int RtpProcess(osip_t* osip, char* pBuf, int size, SOCKET sock);
DWORD WINAPI RtpReceivingThread(LPVOID p);
/*****************************************************************************************************************************/
