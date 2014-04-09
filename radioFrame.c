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

volatile uint8_t inFrameBufferWrIndex;
volatile uint8_t inFrameBufferRdIndex;
uint8_t inFrameBuffer[INFRAMERADIOBUFFERSIZE];		// sk³adowane s¹ adresy i kody zdarzeñ z modu³ów które zosta³y odpytane

void sendAskFrameRadio(uint8_t didAddress){
	setDestinationAddres(didAddress);
	putcharc0(didAddress);
	putcharc0(0);
	putcharc0('?');
}

uint8_t inFrameBufferWrIndexIncrement(){
	if(inFrameBufferWrIndex == sizeof(inFrameBuffer)-1) return 1;
	inFrameBufferWrIndex++;
	return 0;
}

uint8_t getFrameRadio() {

	uint8_t i, j, address;
	uint16_t iData;

	if (rx_counter_usartc0) {
		iData = getcharc0Time(10);			//destination address
		if ((iData & 0x0100) == 0)
			return 1;
		iData = getcharc0Time(10);			// my radio address
		address = (uint8_t) iData;
		iData = getcharc0Time(10);			// data size
		i = (uint8_t) iData;
		for (j = 0; j != i; j++) {
			iData = getcharc0Time(10);
			inFrameBuffer[inFrameBufferWrIndex] = address;
			if (inFrameBufferWrIndexIncrement())
				return 2;
			inFrameBuffer[inFrameBufferWrIndex++] = (uint8_t) iData;
			if (inFrameBufferWrIndexIncrement())
				return 2;
		}
		return 0;
	} else
		return 1;
}


