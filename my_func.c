
#include "my_func.h"

const uint8_t asciiTab[16]  = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46 };


/*************************************
return int with asci
0x34  --> 0x04    'A' -> 0x0a
*************************************/
unsigned char ascii2bin(unsigned char data)
{
data=data-0x30;
if(data>0x10) data = data - 7;
return data;
}


/*************************************
return int with asci buffer long 4
0x31 0x34 0x39 'A'  --> 0x149a
*************************************/
unsigned int atobin(unsigned char *buffer)
{
unsigned char i;
unsigned int ii;
ii=0;
i = ascii2bin(buffer[0]);
ii += i; ii = ii<<4;
i = ascii2bin(buffer[1]);
ii += i; ii = ii<<4;
i = ascii2bin(buffer[2]);
ii += i; ii = ii<<4;
i = ascii2bin(buffer[3]);
ii += i;
return ii;
}



/*************************************
return string with bcd value
0x45  ->   0x34 0x35
*************************************/
void bcdtostr (unsigned char data, unsigned char *buffer)
{
unsigned char low,hi;
low = data & 0x0f;
hi  = (data>>4);

low += 0x30;
hi  += 0x30;

buffer[0] = hi;
buffer[1] =low;

}


/*************************************
return string with unsigned char value
145  ->   0x31 0x34 0x35
*************************************/
void uctostr(unsigned char data,unsigned char *buffer)
{
unsigned char i,j;

j=1;
buffer[2]=0x20;
if(data>=200){data -= 200; buffer[0]=0x32;j=0;}
if(data>=100){data -= 100; buffer[0]=0x31;j=0;}
i=(unsigned char) data/10;
buffer[1-j] = asciiTab[i]; if(i==0 && j==1)j=2;
data = data - i*10;
buffer[2-j] = asciiTab[data];
}
