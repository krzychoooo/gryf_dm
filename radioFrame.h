/*
 * radioFrame.h
 *
 *  Created on: 6 kwi 2014
 *      Author: krzysztofklimas@interia.pl
 */

#ifndef RADIOFRAME_H_
#define RADIOFRAME_H_

#include <avr/io.h>

#define INFRAMERADIOBUFFERSIZE 64

extern volatile uint8_t inFrameBufferWrIndex;
extern volatile uint8_t inFrameBufferRdIndex;
extern uint8_t inFrameBuffer[INFRAMERADIOBUFFERSIZE];

void sendAskFrameRadio(uint8_t didAddress1);
uint8_t getFrameRadio();

#endif /* RADIOFRAME_H_ */
