/**************************************************************************************************************************//**
* @file    udp_server.c
* @author  Maxim Ivanchenko
* @brief
******************************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
//#include <arpa/inet.h>
#include "udp_server.h"
#include<stdio.h>
#include<winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <ws2def.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
/*****************************************************************************************************************************/
#define BUFLEN 1600  //Max length of buffer
/*****************************************************************************************************************************/
extern IN_ADDR gDestIp;
/*****************************************************************************************************************************/
SOCKET UdpServerCreate(char** ppBuf, unsigned short port)
{
	// http://www.binarytides.com/udp-socket-programming-in-winsock/
	struct sockaddr_in server;
	*ppBuf = (char*) malloc(BUFLEN);
	WSADATA wsa;
	SOCKET s;
	unsigned long ulMode;

	//clear the buffer by filling null, it might have previously received data
	memset(*ppBuf, '\0', BUFLEN);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	ulMode = 1;
	ioctlsocket(s, FIONBIO, (unsigned long*)&ulMode);

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	printf("Waiting for data...");
	fflush(stdout);
	return s;
}
/*****************************************************************************************************************************/
void UdpServerDelete(pSocket)
{
	closesocket(pSocket);
	WSACleanup();
}
/*****************************************************************************************************************************/
int UdpServerProcess(SOCKET socket, char* pBuf, SOCKADDR_IN * pSockIn)
{
	int slen = sizeof(*pSockIn);
	WSADATA wsa;
	int recv_len;

	//try to receive some data, this is a blocking call
	recv_len = recvfrom(socket, pBuf, BUFLEN, 0, (struct sockaddr *) pSockIn, &slen);
	if (recv_len == SOCKET_ERROR)
	{
		recv_len = 0;
		//printf("recvfrom() failed with error code : %d", WSAGetLastError());
		//exit(EXIT_FAILURE);
	}
	else
	{
		gDestIp.S_un.S_addr = pSockIn->sin_addr.S_un.S_addr;
	}
	return recv_len;
}
/*****************************************************************************************************************************/
int UdpSend(char* pBuf, int len, IN_ADDR destIp, unsigned short port)
{
	SOCKET  s = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addr;


	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = destIp;

#if 0
	unsigned long ulMode;
	WSADATA wsa;
	ulMode = 1;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	ioctlsocket(s, FIONBIO, (unsigned long*) &ulMode);
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
#endif
	sendto(s, pBuf, len, 0, (SOCKADDR*) &addr, sizeof(SOCKADDR_IN));
	return 0;
}
/*****************************************************************************************************************************/
int UdpSendExistSock(SOCKET sock, char* pBuf, int len, IN_ADDR destIp, unsigned short port)
{
	SOCKADDR_IN addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = destIp;

	sendto(sock, pBuf, len, 0, (SOCKADDR*) &addr, sizeof(SOCKADDR_IN));
	return 0;
}
/*****************************************************************************************************************************/
