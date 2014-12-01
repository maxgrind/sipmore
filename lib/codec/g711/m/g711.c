/*
http://www.cisco.com/c/en/us/support/docs/voice/voice-quality/7934-bwidth-consume.html
Codec Information																							Bandwidth Calculations
Codec & Bit Rate (Kbps)	Codec Sample Size (Bytes)	Codec Sample Interval (ms)	Mean Opinion Score (MOS)	Voice Payload Size (Bytes)	Voice Payload Size (ms)	Packets Per Second (PPS)	Bandwidth MP or FRF.12 (Kbps)	Bandwidth w/cRTP MP or FRF.12 (Kbps)	Bandwidth Ethernet (Kbps)
G.711 (64 Kbps)			80 Bytes					10 ms						4.1							160 Bytes					20 ms					50							82.8 Kbps						67.6 Kbps								87.2 Kbps
*/


signed short MuLaw_Decode(char number)
{
	const unsigned short MULAW_BIAS = 33;
	unsigned char sign = 0, position = 0;
	signed short decoded = 0;
	number = ~number;
	if (number & 0x80)
	{
		number &= ~(1 << 7);
		sign = -1;
	}
	position = ((number & 0xF0) >> 4) + 5;
	decoded = ((1 << position) | ((number & 0x0F) << (position - 4)) | (1 << (position - 5))) - MULAW_BIAS;
   return (sign == 0) ? (decoded) : (-(decoded));
}

//signed short MuLaw_Decode(char number)
////signed short ALaw_Decode(char number)
//{
//	unsigned char sign = 0x00;
//	unsigned char position = 0;
//	signed short decoded = 0;
//	number ^= 0x55;
//	if (number & 0x80)
//	{
//		number &= ~(1 << 7);
//		sign = -1;
//	}
//	position = ((number & 0xF0) >> 4) + 4;
//	if (position != 4)
//	{
//		decoded = ((1 << position) | ((number & 0x0F) << (position - 4))
//			| (1 << (position - 5)));
//	}
//	else
//	{
//		decoded = (number << 1) | 1;
//	}
//	return (sign == 0) ? (decoded) : (-decoded);
//}

//int16_t ALaw_Decode(int8_t number)
//{
//	uint8_t sign = 0x00;
//	uint8_t position = 0;
//	int16_t decoded = 0;
//	number ^= 0x55;
//	if (number & 0x80)
//	{
//		number &= ~(1 << 7);
//		sign = -1;
//	}
//	position = ((number & 0xF0) >> 4) + 4;
//	if (position != 4)
//	{
//		decoded = ((1 << position) | ((number & 0x0F) << (position - 4))
//			| (1 << (position - 5)));
//	}
//	else
//	{
//		decoded = (number << 1) | 1;
//	}
//	return (sign == 0) ? (decoded) : (-decoded);
//}
//int16_t MuLaw_Decode(int8_t number)
//{
//	const uint16_t MULAW_BIAS = 33;
//	uint8_t sign = 0, position = 0;
//	int16_t decoded = 0;
//	number = ~number;
//	if (number & 0x80)
//	{
//		number &= ~(1 << 7);
//		sign = -1;
//	}
//	position = ((number & 0xF0) >> 4) + 5;
//	decoded = ((1 << position) | ((number & 0x0F) << (position - 4))
//		| (1 << (position - 5))) - MULAW_BIAS;
//	return (sign == 0) ? (decoded) : (-(decoded));
//}