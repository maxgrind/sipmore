/**************************************************************************************************************************//**
 * @file    rtp.h
 * @author  Maxim Ivanchenko
 * @brief   RFC3550 RTP protocol implementation  
******************************************************************************************************************************/
#ifndef __RTP_H__
#define __RTP_H__
/*****************************************************************************************************************************/
typedef struct sRtpHeader{
	unsigned int version : 2;	// protocol version 
	unsigned int p : 1;			// padding flag 
	unsigned int x : 1;			// header extension flag 
	unsigned int cc : 4;		// contributing source (CSRC) identifiers count 
	unsigned int m : 1;			// marker bit 
	unsigned int pt : 7;		// payload type 
	unsigned int seq : 16;		// sequence number 
	unsigned int ts;			// timestamp 
	unsigned int ssrc;			// synchronization source identifier
}
tRtpHeader;
/*****************************************************************************************************************************/
typedef struct sRtpPacket
{
	tRtpHeader header;
	unsigned int csrc[16];	// optional contributing source (CSRC) identifiers
	char *pPayload;
	int payloadLen; // payload length in bytes
}
tRtpPacket;
/*****************************************************************************************************************************/
int RtpParse(char *pData, tRtpPacket* pRtp);
/*****************************************************************************************************************************/
int RtpPacketDestroy(tRtpPacket* pRtp);
/*****************************************************************************************************************************/
int RtpCompose
(
	unsigned short sequenceNumber,
	unsigned int timestamp,
	char *pPayload,
	int payloadlength, // in bytes
	char **ppRtpPacked, // out packet
	int *pLength  // out packet's payload length in bytes
);
/*****************************************************************************************************************************/
#endif // __RTP_H__