/*
 * usartc0.h
 *
 *  Created on: 20 gru 2013
 *      Author: krzysztof
 */

#ifndef USARTC0_H_
#define USARTC0_H_

// USARTC0 initialization
void usartc0_init();

// USARTC0 Receiver buffer
#define RX_BUFFER_SIZE_USARTC0 8

char getchar();

// USARTC0 Transmitter buffer
#define TX_BUFFER_SIZE_USARTC0 8

// Write a character to the USARTC0 Transmitter buffer
// USARTC0 is used as the default output device by the 'putchar' function

void putcharc0(char c);

#endif /* USARTC0_H_ */
