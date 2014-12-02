/**************************************************************************************************************************//**
 * @file    rtp.c
 * @author  Maxim Ivanchenko
 * @brief   RFC3550 RTP protocol implementation  
******************************************************************************************************************************/
#include <stdlib.h> 
#include "rtp.h"
/**************************************************************************************************************************//**
* @brief Parsing RTP packet. pData is big-endian input data. RtpPacketDestroy must be called to free pRtp->pPayload
******************************************************************************************************************************/
int RtpParse(char *pData, int len, tRtpPacket* pRtp)
{
	int payloadLen = 0;
	char *pPayload = NULL;
	pRtp->header.version = (pData[0] >> 6) & 0x00000003;

	pRtp->header.p = (pData[0] >> 5) & 0x00000001;
	pRtp->header.x = (pData[0] >> 4) & 0x00000001;
	pRtp->header.cc = pData[0] & 0x0000000F;
	pRtp->header.m = (pData[1] >> 7) & 0x00000001;
	pRtp->header.pt = pData[1]  & 0x0000007F;
	pRtp->header.seq = ((pData[2] << 8) & 0x0000FF00) | (pData[3]  & 0x000000FF);
	pRtp->header.ts = ((pData[4] << 24) & 0xFF000000) | ((pData[5] << 16) & 0x00FF0000) | ((pData[6] << 8) & 0x0000FF00) | (pData[7]  & 0x00000FF);
	pRtp->header.ssrc = ((pData[8] << 24) & 0xFF000000) | ((pData[9] << 16) & 0x00FF0000) | ((pData[10] << 8) & 0x0000FF00) | (pData[11] & 0x00000FF);
	payloadLen = len - sizeof(tRtpHeader)-(pRtp->header.cc * 4);
	pRtp->pPayload = (char*) malloc(payloadLen);
	if (pRtp->pPayload == NULL) return 0;
	memcpy(pRtp->pPayload, &pData[len - payloadLen], payloadLen);
	pRtp->payloadLen = payloadLen;

	if (pRtp->header.version != 2)
	{
		return -1;
	}
	return 0;
}
/**************************************************************************************************************************//**
* @brief Composng RTP packet. pData is big-endian input data. RtpPacketDestroy must be called to free pRtp->pPayload
******************************************************************************************************************************/
int RtpCompose
(
	unsigned short sequenceNumber,
	unsigned int timestamp,
	char *pPayload,
	int payloadlength, // payload length in bytes
	char **ppRtpPacked, // out packet
	int *pLength  // out packet length in bytes
)
{
	int i = 0;
	*ppRtpPacked = (char*) malloc(payloadlength + 12);
	char *pPacket = *ppRtpPacked;
	if (*ppRtpPacked == NULL) return -1;

	pPacket[0] = 0x80;  // ver 2, no padding, no ext, no CSRCs
	pPacket[1] = 0x00;  // no marker, payload type - PCMU. todo: other payloads
	pPacket[2] = (char)(sequenceNumber >> 8) & 0x00FF;
	pPacket[3] = (char)sequenceNumber & 0x00FF;
	pPacket[4] = (char)(sequenceNumber >> 24) & 0x000000FF;
	pPacket[5] = (char)(sequenceNumber >> 16) & 0x000000FF;
	pPacket[6] = (char)(sequenceNumber >> 8) & 0x000000FF;
	pPacket[7] = (char)sequenceNumber & 0x000000FF;
	pPacket[8] = (char)0x01; // SSRC (sync source). Random
	pPacket[9] = (char)0xa3;
	pPacket[10] = (char)0x34;
	pPacket[11] = (char)0xfb;

	memcpy(&pPacket[12], pPayload, payloadlength);

	*pLength = payloadlength + 12;

	return 0;
}
/*****************************************************************************************************************************/
int RtpPacketDestroy(tRtpPacket* pRtp)
{
	free(pRtp->pPayload);
}
/*****************************************************************************************************************************/

