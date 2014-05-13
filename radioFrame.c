/*
 * radioFrame.c
 *
 *  Created on: 6 kwi 2014
 *      Author: krzysztofklimas@interia.pl
 */

/*
 * 			RC1180 CONNECT TO USART C
 */

#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>
#include "radioFrame.h"
#include "radio_config.h"
#include "USART/usartc0.h"
#include "timer0x.h"
#include "radio_config.h"

volatile uint8_t inFrameBufferWrIndex;
volatile uint8_t inFrameBufferRdIndex;
uint8_t inFrameBuffer[INFRAMERADIOBUFFERSIZE];		// sk³adowane s¹ adresy i kody zdarzeñ z modu³ów które zosta³y odpytane

void sendAskFrameRadio(uint8_t didAddress){
	setDestinationAddres(didAddress);				// set DID in RC1180
	_delay_ms(5);
	putcharc0(didAddress);
	putcharc0(0);
	putcharc0('?');
	putcharc0(0);
}

void sendAskFramesRadio(){
	uint8_t i, result;
	for(i=0; i!=numberSlaveModules; i++){
		sendAskFrameRadio(i);
		_delay_ms(inFrameTime);
		result = getFrameRadio();
		printf(" read radio=%d\n",result);
	}
}

uint8_t inFrameBufferWrIndexIncrement(){
	if(inFrameBufferWrIndex == sizeof(inFrameBuffer)-1) return 1;
	inFrameBufferWrIndex++;
	return 0;
}

void alarmSimulate(void){
	uint8_t i,j;

	j=random()%5;
	for(i=0; i!=j; i++){
		inFrameBuffer[inFrameBufferWrIndex++] = i+1;
		inFrameBuffer[inFrameBufferWrIndex++] = random()%4 + 1;
	}
}

uint8_t getFrameRadio() {

	uint8_t i, j, address, error;
	uint16_t iData;

	error = 0;
	if (rx_counter_usartc0) {
		iData = getcharc0Time(10);			//destination address
//		if ((iData & 0x0100) == 0)
//			return 1;
		iData = getcharc0Time(10);			// mm radio address
		address = (uint8_t) iData;
//		printf(" dra=%d", address);
		iData = getcharc0Time(10);			// data size
		i = (uint8_t) iData;
		for (j = 0; j != i; j++) {
			iData = getcharc0Time(10);
			//printf("kod alarmu = %02x ",(uint8_t) iData);
			if (((uint8_t) iData) != 0) {
				inFrameBuffer[inFrameBufferWrIndex] = address;
				if (inFrameBufferWrIndexIncrement())
					error |= 2;
				inFrameBuffer[inFrameBufferWrIndex] = (uint8_t) iData;
				if (inFrameBufferWrIndexIncrement())
					error |= 2;
			}
		}
		iData = getcharc0Time(10);			// get RSSI
		i = (uint8_t) iData;
		//printf("power=%d ", (uint8_t) i / 2);
		while (rx_counter_usartc0)
			getcharc0();
		return error;
	} else
		return 1;
}


