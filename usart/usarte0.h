/*
 * usarte0.h
 *
 *  Created on: 21 gru 2013
 *      Author: krzysztof
 */

#ifndef USARTE0_H_
#define USARTE0_H_

// USARE0 initialization
void usarte0_init();

// USARTE0 Receiver buffer
#define RX_BUFFER_SIZE_USARTE0 8


char getchar();

// USARTE0 Transmitter buffer
#define TX_BUFFER_SIZE_USARTE0 8

// Write a character to the USARTE0 Transmitter buffer
// USARTE0 is used as the default output device by the 'putchar' function

void putchare0(char c);

#endif /* USARTE0_H_ */
