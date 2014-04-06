/*
 * radioFrame.c
 *
 *  Created on: 6 kwi 2014
 *      Author: krzysztofklimas@interia.pl
 */

/*
 * 			RC1180 CONNECT TO USART C
 */

#include "radioFrame.h"
#include "radio_config.h"
#include "USART/usartc0.h"


uint8_t inFrameBuffer[INFRAMERADIOBUFFERSIZE];

void sendAskFrameRadio(uint8_t didAddress){
	setDestinationAddres(didAddress);
	putcharc0('0');
	putcharc0(0);
	putcharc0('?');
	putcharc0(0);
}



uint8_t getFrameRadio(){

	uint8_t data;
	uint16_t iData;

	iData = getcharc0Time(10);
	if((iData & 0x0100) == 0) return 0;


	return 1;
}
