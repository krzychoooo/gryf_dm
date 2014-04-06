
#ifndef _MYFUNC_H_
#define _MYFUNC_H_

#include <avr/io.h>

extern const uint8_t asciiTab[16];

unsigned char ascii2bin(unsigned char data);

unsigned int atobin(unsigned char *buffer);

void bcdtostr (unsigned char data, unsigned char *buffer);

void uctostr(unsigned char data,unsigned char *buffer);

#endif
