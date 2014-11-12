#pragma once
#include "osip2/osip.h"


void SipProcess(osip_t* osip, char* pBuf, int size, SOCKET sock);
void ProcessNewReqIst(osip_t* osip, osip_event_t *evt, int sock);
void ProcessNewReqNist(osip_t* osip, osip_event_t *evt, int sock);
int BuildResponse(const osip_message_t *request, osip_message_t **response);
//int BuildResponse200Ok(const osip_message_t *request, osip_message_t **response);

/*

INVITE sip:UserB@there.com SIP/2.0
Via: SIP/2.0/UDP ss1.wcom.com:5060;branch=230f2.1
Via: SIP/2.0/UDP here.com:5060
Record-Route: <sip:UserB@there.com;maddr=ss1.wcom.com>
From: BigGuy <sip:UserA@here.com>
To: LittleGuy <sip:UserB@there.com>
Call-ID: 12345600@here.com
CSeq: 3 INVITE
Contact: <sip:UserA@100.101.102.103>
Proxy-Authorization:Digest username="UserA", realm="MCI SIP",
nonce="c1e22c41ae6cbe5ae983a9c8e88d359", opaque="",
uri="sip:ss2.mci.com", response="f44ab22f150c6a56071bce8"
Content-Type: application/sdp
Content-Length: 147

v=0
o=UserA 2890844526 2890844526 IN IP4 here.com
s=Session SDP
c=IN IP4 100.101.102.103
t=0 0
m=audio 49172 RTP/AVP 0
a=rtpmap:0 PCMU/8000


*/


