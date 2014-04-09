/*
 * rs485Frame.h
 *
 *  Created on: 7 kwi 2014
 *      Author: krzysztofklimas@interia.pl
 */

#ifndef RS485FRAME_H_
#define RS485FRAME_H_

#include <avr/io.h>
#include "USART/usarte0.h"

typedef struct moduleConfig{
	uint8_t myAddress;
	uint8_t baudRate;		// uwaga nie oprogramowane ustawienie prêdkoci siedzi w uart
} TModuleConfig;

extern uint8_t state;	//stan modu³u  ewentualnie przerobiæ na struct

void copyModuleConfigEEpromToRam();
void copyModuleConfigRamToEEprom();
uint8_t getFrameFromMc();
void userSetRs485();

void sendAlarmFrame();
void userSetRs485();

#endif /* RS485FRAME_H_ */
