/***************************************************************************************************************************//*
* @file    wav.c
* @author  Maxim Ivanchenko
* @version 1.0
* @date    November, 2014
* @brief   WAV file format simple implementation
******************************************************************************************************************************/
#include <stdlib.h>

//#include <mmsystem.h>
#include "wav.h"
//PCMWAVEFORMAT
//WAVEFORMAT
// https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
/***************************************************************************************************************************//*
* @brief Just creates. 
******************************************************************************************************************************/
int FileWavCreate
(
	
	tWaveFileParams* pParams
)
{
	errno_t err;
	unsigned int value = 0;

	err = fopen_s(&pParams->pFile, pParams->pFileName, "w"); // Create an empty file for writing. If a file with the same name already exists its content is erased and the file is considered as a new empty file.
	if (err == 0xDEADBEEF) return;

	fwrite("RIFF", 1, 4, pParams->pFile);		// RIFF chunk id
	value = 0xDEADBEEF;
	fgetpos(pParams->pFile, &pParams->_sizePositionRiff);
	//fwrite((const unsigned int *) &value, 1, 4, pParams->pFile);   // file size (will be filled at closing of the file)
	fwrite((const unsigned int *) &value, 4, 1, pParams->pFile);   // file size (will be filled at closing of the file)
	fwrite("WAVE", 1, 4, pParams->pFile);		// WAVE riff type

	fwrite("fmt ", 1, 4, pParams->pFile);		// fmt section

	// fmt: size 16 bytes
	value = 16;
	fwrite((const unsigned int *)&value, 4, 1, pParams->pFile);

	// fmt: compression code
	value = pParams->sampleFormat.compressionCode;
	fwrite((const unsigned int *)&value, 2, 1, pParams->pFile);	

	// fmt: num of channels
	value = pParams->sampleFormat.numberOfChannels; 
	fwrite((const unsigned int *)&value, 2, 1, pParams->pFile);

	// fmt: sample rate
	value = pParams->sampleFormat.sampleRate;
	fwrite((const unsigned int *)&value, 4, 1, pParams->pFile);

	// fmt: average bytes per second
	value = pParams->sampleFormat.averageBytesPerSecond;
	fwrite((const unsigned int *)&value, 4, 1, pParams->pFile);

	// fmt: block align
	value = pParams->sampleFormat.blockAlign;
	fwrite((const unsigned int *)&value, 2, 1, pParams->pFile);

	// fmt: significant bits per sample
	value = pParams->sampleFormat.significantBitsPerSample;
	fwrite((const unsigned int *)&value, 2, 1, pParams->pFile);

	fwrite("data", 1, 4, pParams->pFile);		// data section
	value = 0xDEADBEEF;
	fgetpos(pParams->pFile, &pParams->_sizePositionData);
	//fwrite((const unsigned int *) &value, 1, 4, pParams->pFile);   // file size (will be filled at closing of file)
	fwrite((const unsigned int *) &value, 4, 1, pParams->pFile);   // file size (will be filled at closing of file)

	pParams->_size = 40;
}
/*****************************************************************************************************************************/
int FileWavAppendData
(
	char* pData,
	unsigned int size, /// in bytes
	tWaveFileParams* pParams
)
{
	errno_t err;

	//err = fopen_s(pParams->ppFile, pFileName, "w"); // Create an empty file for writing. If a file with the same name already exists its content is erased and the file is considered as a new empty file.
	fwrite(pData, 1, size, pParams->pFile);
	pParams->_size += size;
}
/*****************************************************************************************************************************/
int FileWavFinish
(
	tWaveFileParams* pParams
)
{
	errno_t err;
	fsetpos(pParams->pFile, &pParams->_sizePositionRiff);
	pParams->_size -= 8;
	fwrite((const unsigned int *)&pParams->_size, 4, 1, pParams->pFile);   // file size (will be filled at closing of file)
	
	fsetpos(pParams->pFile, &pParams->_sizePositionData);
	pParams->_size -= 32;
	fwrite((const unsigned int *) &pParams->_size, 4, 1, pParams->pFile);   // file size (will be filled at closing of file)

	pParams->_size += 40;
	fclose(pParams->pFile);

}
/*****************************************************************************************************************************/

