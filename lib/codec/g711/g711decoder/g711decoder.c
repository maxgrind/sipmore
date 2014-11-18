
// g711_decode.c  
// G.711 decoder C demo program  
  
#include <stdarg.h>  
#include <stdio.h>  
#include <stdlib.h>  
  
#define TEST_ITEMS  256     // input items in test  
  
// Input, output, reference filenames.  
#define INFILE      "d:\\sipmore.wav"  
#define OUTFILE     "out.bin"  
#define REFFILE     "ref.bin"   
  
// Data buffers.  
char        inbuf[TEST_ITEMS];  // G.711 input (bytes)  
int     outbuf[TEST_ITEMS]; // PCM output (words)  
int     refbuf[TEST_ITEMS]; // PCM reference  
  
// Failure: cry and die.  
void  
fatal(char *msg, ...)  
{  
    va_list ap;  
  
    va_start(ap, msg);  
    vfprintf(stderr, msg, ap);  
    va_end(ap);  
    exit(EXIT_FAILURE);  
}  
  
// Open a file, die on failure.  
FILE *  
xopen(char *name, char *mode)  
{  
    FILE    *fp;  
  
    if ((fp = fopen(name, mode)) == NULL)  
        fatal("%s: open failed\n", name);  
    return fp;  
}  
  
// Read a file, die on failure.  
void  
xread(char *name, char *mode, void *buf, int size, int count)  
{  
    FILE    *fp;  
  
    fp = xopen(name, mode);  
    if (fread(buf, size, count, fp) != count)  
        fatal("%s: read failed\n", name);  
    fclose(fp);  
}  
  
// Decode G.711 A-law to PCM.  
int  
g711_decode(unsigned char data)  
{  
    unsigned int    i, exponent, mantissa;  
  
    i = (data ^ 0x55) & 0x7F;       // re-toggle even bits, remove sign bit  
    exponent = i >> 4;            // extract exponent  
    mantissa = i & 0xF;         // extract mantissa  
    if (exponent > 0)  
        mantissa |= 0x10;       // restore hidden bit to mantissa  
    mantissa = (mantissa << 4) | 0x08;    // left justify, add 1/2 quantization stepsize  
  
    // Normalize mantissa if log encoding (not linear segment).  
    if (exponent > 1)  
        mantissa <<= exponent -1;  
  
    // Negate if sign bit was NOT set in data.  
    if ((data & 0x80) == 0)  
        mantissa = -mantissa;  
    return mantissa << 16;            // shift output to be MSB aligned in 32 bits  
}  
  
  
//int main(int argc, char *argv[]) 
int G711decode()
{  
    FILE        *fp;  
    int     i;  
  
    // Read input file.  
    xread(INFILE, "rb", inbuf, sizeof(inbuf[0]), TEST_ITEMS);  
  
    // Decode.  
    for (i = 0; i < TEST_ITEMS; i++)  
        outbuf[i] = g711_decode(inbuf[i]);  
  
    // Dump output data.  
    fp = xopen(OUTFILE, "wb");  
    if (fwrite(outbuf, sizeof(outbuf[0]), TEST_ITEMS, fp) != TEST_ITEMS)  
  
        fatal("%s: write failed\n", OUTFILE);  
    fclose(fp);  
  
    // Read reference file.  
    xread(REFFILE, "rb", refbuf, sizeof(refbuf[0]), TEST_ITEMS);  
  
    // Compare result to reference result.  
    for (i = 0; i < TEST_ITEMS; i++) {  
        if (outbuf[i] != refbuf[i])  
            fatal("result miscompares at word %d: expected 0x%X, got 0x%X\n",  
                refbuf[i], outbuf[i]);  
    }  
  
    // Done.  
    printf("Success!\n");  
    exit(EXIT_SUCCESS);  
}  
  
// end of g711_decode_c.c  