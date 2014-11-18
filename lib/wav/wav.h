/***************************************************************************************************************************//*
 * @file    wav.h
 * @author  Maxim Ivanchenko
 * @version 1.0
 * @date    November, 2014
 * @brief   WAV file format simple implementation  
******************************************************************************************************************************/
#ifndef __FILE_WAV_H__
#define __FILE_WAV_H__
/*****************************************************************************************************************************/
#include <stdio.h>
/*****************************************************************************************************************************/
typedef enum sCompressionCode
{
	WAV_FMT_COMP_CODE_UNKNOWN		= 0x0000,	// Unknown
	WAV_FMT_COMP_CODE_PCM			= 0x0001,	// PCM/uncompressed
	WAV_FMT_COMP_CODE_ADPCM			= 0x0002,	// Microsoft ADPCM
	WAV_FMT_COMP_CODE_G711_ALAW		= 0x0006,	// ITU G.711 a-law
	WAV_FMT_COMP_CODE_G711_ULAW		= 0x0007,	// ITU G.711 u-law
	WAV_FMT_COMP_CODE_IMA_ADPCM		= 0x0011,	// IMA ADPCM
	WAV_FMT_COMP_CODE_G723_ADPCM	= 0x0016,	// ITU G.723 ADPCM(Yamaha)
	WAV_FMT_COMP_CODE_GSM			= 0x0031,	// GSM 6.10
	WAV_FMT_COMP_CODE_G721_ADPCM	= 0x0040,	// ITU G.721 ADPCM
	WAV_FMT_COMP_CODE_MPEG			= 0x0050,	// MPEG
	WAV_FMT_COMP_CODE_EXPERIM		= 0xFFFF	// Experimental
}
tCompressionCode;
/*****************************************************************************************************************************/
typedef struct sWaveFileFmtSection
{
	tCompressionCode	compressionCode;
	unsigned short		numberOfChannels;
	unsigned int		sampleRate;
	unsigned int		averageBytesPerSecond;
	unsigned short		blockAlign;
	unsigned short		significantBitsPerSample;
	//unsigned short		extraFormatBytes;
}
tWaveFileFmtSection;
/*****************************************************************************************************************************/
typedef struct sWaveFileParams
{
	// os file props
	const char* pFileName;
	FILE* pFile;
	unsigned int _size; // current size of file including "RIFF" chunk id and size (8 bytes)(internal var)
	fpos_t _sizePositionRiff; // internal var to store position of size section of first chunk
	fpos_t _sizePositionData; // internal var to store position of size section of second chunk
	
	// sample format info
	tWaveFileFmtSection sampleFormat;
}
tWaveFileParams;
/*****************************************************************************************************************************/
int FileWavCreate
(
	tWaveFileParams* pParams
);
/*****************************************************************************************************************************/
int FileWavAppendData
(
	void* pData,
	unsigned int size, /// in bytes
	tWaveFileParams* pParams
);
/*****************************************************************************************************************************/
int FileWavFinish
(
	tWaveFileParams* pParams
);
/*****************************************************************************************************************************/
#endif // __FILE_WAV_H__