/* we're not going to use arguments for this example */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
//#include <arpa/inet.h>

#include "udp_server.h"



#define SERV_UDP_PORT 6543
#define SERV_TCP_PORT 6543
#ifdef M_HOME_PC
#define SERV_HOST_ADDR "192.168.1.5"
#else
#define SERV_HOST_ADDR "192.168.1.50"
//#define SERV_HOST_ADDR "192.168.55.48"
#endif



//
//void StartServer(void)
//{
//	int socket_server, socket_client, child_pid;
//	struct sockaddr_in address_client, address_server;
//	socklen_t client_address_size;
//	FILE *stream_client;
//	time_t now;
//	struct tm *tm;
//
//	/* open a tcp socket to listen on */
//
//	if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//		err_dump("server: can't open stream socket");
//
//	/* bind to our local address  ans start listening so that clients can find us */
//
//	bzero((void *)&address_server, (size_t)sizeof(address_server));
//
//	address_server.sin_family = AF_INET;
//	address_server.sin_addr.s_addr = htonl(INADDR_ANY);
//	address_server.sin_port = htons(SERV_TCP_PORT);
//
//	if (bind(
//		socket_server,
//		(struct sockaddr *) &address_server,
//		sizeof(address_server))
//		< 0)
//		err_dump("server: can't bind local address");
//
//	listen(socket_server, 5);
//
//	for (;;) {
//
//		/* wait for client connection and accpt it when it comes */
//
//		client_address_size = sizeof(address_client);
//		socket_client = accept(
//			socket_server,
//			(struct sockaddr *) &address_client,
//			&client_address_size);
//
//		if (socket_client < 0)
//			err_dump("server: accept error");
//
//		if ((child_pid = fork()) < 0)
//			err_dump("server: fork error");
//
//		/* specific client process handling starts here */
//
//		else if (child_pid == 0) {
//
//			/* client process has no need of the server socket */
//
//			close(socket_server);
//
//			/* generate a file stream from the client socket */
//
//			if ((stream_client = fdopen(socket_client, "w")) == NULL) {
//				perror("daytimed fdopen");
//				return 5;
//			}
//
//			/* write to the client stream as per normal then close it */
//
//			if ((now = time(NULL)) < 0) {
//				perror("daytimed time");
//
//				return 6;
//			}
//
//			tm = gmtime(&now);
//			fprintf(stream_client, "%.4i-%.2i-%.2iT%.2i:%.2i:%.2iZ\n",
//				tm->tm_year + 1900,
//				tm->tm_mon + 1,
//				tm->tm_mday,
//				tm->tm_hour,
//				tm->tm_min,
//				tm->tm_sec);
//
//			fclose(stream_client);
//
//			exit(EXIT_SUCCESS);
//
//			/* client process has left the building */
//		}
//
//		/* the server has no need to keep the client socket open */
//
//		close(socket_client);
//	}
//
//	return 0;
//}

#include<stdio.h>
#include<winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <ws2def.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 1600  //Max length of buffer
#define PORT 5060   //The port on which to listen for incoming data

void UpdServer(void)
{
	// http://www.binarytides.com/udp-socket-programming-in-winsock/
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;

	slen = sizeof(si_other);

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

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	//keep listening for data
	while (1)
	{
		printf("Waiting for data...");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n", buf);

		//now reply the client with the same data
		if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

	closesocket(s);
	WSACleanup();
}

SOCKET UdpServerCreate(char** ppBuf)
{
	// http://www.binarytides.com/udp-socket-programming-in-winsock/
	struct sockaddr_in server;
	*ppBuf = (char*) malloc(BUFLEN);
	WSADATA wsa;
	SOCKET s;

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

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

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

void UdpServerDelete(pSocket)
{
	closesocket(pSocket);
	WSACleanup();
}

int UdpServerProcess(SOCKET socket, char* pBuf)
{
	struct sockaddr_in  si_other;
	int slen = sizeof(si_other);
	WSADATA wsa;
	int recv_len;

	//try to receive some data, this is a blocking call
	recv_len = recvfrom(socket, pBuf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen); 
	if (recv_len == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//print details of the client/peer and the data received
	//printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	//printf("Data: %s\n", pBuf);

	return recv_len;
}



UdpSend(SOCKET socket, char* pData, int size)
{
}