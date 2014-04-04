/*
 * usarte0.c
 *
 *  Created on: 21 gru 2013
 *      Author: krzysztof
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "usarte0.h"

char rx_buffer_usarte0[RX_BUFFER_SIZE_USARTE0];

// This flag is set on USARTe0 Receiver buffer overflow
char rx_buffer_overflow_usarte0=0;

char tx_buffer_usarte0[TX_BUFFER_SIZE_USARTE0];

unsigned char rx_wr_index_usarte0=0,rx_rd_index_usarte0=0,rx_counter_usarte0=0;
unsigned char tx_wr_index_usarte0=0,tx_rd_index_usarte0=0,tx_counter_usarte0=0;

// USARTe0 initialization
void usarte0_init()
{

// Note: the correct PORTC direction for the RxD, TxD and XCK signals
// is configured in the ports_init function

// Transmitter is enabled
// Set TxD=1
PORTC.OUTSET=0x08;
PORTC.DIRSET=0X08;

#ifdef UARTE0_DE_PORT
		// inicjalizujemy liniê steruj¹c¹ nadajnikiem
		UARTE0_DE_DIR |= UARTE0_DE_BIT;
		UARTE0_DE_ODBIERANIE;
	#endif

// Communication mode: Asynchronous USART
// Data bits: 8
// Stop bits: 1
// Parity: Disabled
USARTE0.CTRLC=USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;

// Receive complete interrupt: Low Level
// Transmit complete interrupt: Low Level
// Data register empty interrupt: Disabled
USARTE0.CTRLA=(USARTE0.CTRLA & (~(USART_RXCINTLVL_gm | USART_TXCINTLVL_gm | USART_DREINTLVL_gm))) |
	USART_RXCINTLVL_LO_gc | USART_TXCINTLVL_LO_gc | USART_DREINTLVL_OFF_gc;

// Required Baud rate: 115200
// Real Baud Rate: 115211,5 (x1 Mode), Error: 0,0 %
USARTE0.BAUDCTRLA=0x2E;
USARTE0.BAUDCTRLB=((0x09 << USART_BSCALE_gp) & USART_BSCALE_gm) | 0x08;

// Receiver: On
// Transmitter: On
// Double transmission speed mode: Off
// Multi-processor communication mode: Off
USARTE0.CTRLB=(USARTE0.CTRLB & (~(USART_RXEN_bm | USART_TXEN_bm | USART_CLK2X_bm | USART_MPCM_bm | USART_TXB8_bm))) |
	USART_RXEN_bm | USART_TXEN_bm;
}




// USARTE0 Receiver interrupt service routine
//interrupt [USARTE0_RXC_vect] void usarte0_rx_isr(void)
ISR(USARTE0_RXC_vect)
{
unsigned char status;
char data;

status=USARTE0.STATUS;
data=USARTE0.DATA;
if ((status & (USART_FERR_bm | USART_PERR_bm | USART_BUFOVF_bm)) == 0)
   {
   rx_buffer_usarte0[rx_wr_index_usarte0++]=data;
   if (rx_wr_index_usarte0 == RX_BUFFER_SIZE_USARTE0) rx_wr_index_usarte0=0;
   if (++rx_counter_usarte0 == RX_BUFFER_SIZE_USARTE0)
      {
      rx_counter_usarte0=0;
      rx_buffer_overflow_usarte0=1;
      }
   }
}



char getchare0(void)
{
char data;

while (rx_counter_usarte0==0);
data=rx_buffer_usarte0[rx_rd_index_usarte0++];
if (rx_rd_index_usarte0 == RX_BUFFER_SIZE_USARTE0) rx_rd_index_usarte0=0;
asm("cli");
--rx_counter_usarte0;
asm("sei");
return data;
}

// USARTE0 Transmitter interrupt service routine
ISR (USARTE0_TXC_vect)
{
PORTC.OUTTGL = 0X10;
	if (tx_counter_usarte0)
	   {
	   --tx_counter_usarte0;
	   USARTE0.DATA=tx_buffer_usarte0[tx_rd_index_usarte0++];
	   if (tx_rd_index_usarte0 == TX_BUFFER_SIZE_USARTE0) tx_rd_index_usarte0=0;
	   }
	else{
#ifdef UARTE0_DE_PORT

		UARTE0_DE_PORT &= ~UARTE0_DE_BIT;	// zablokuj nadajnik RS485

#endif
	}
}


void putchare0(char c)
{
while (tx_counter_usarte0 == TX_BUFFER_SIZE_USARTE0);
asm("cli");
if (tx_counter_usarte0 || ((USARTE0.STATUS & USART_DREIF_bm)==0))
   {
   tx_buffer_usarte0[tx_wr_index_usarte0++]=c;
   if (tx_wr_index_usarte0 == TX_BUFFER_SIZE_USARTE0) tx_wr_index_usarte0=0;
   ++tx_counter_usarte0;
   }
else{
#ifdef UARTE0_DE_PORT
		UARTE0_DE_PORT |= UARTE0_DE_BIT;	// zablokuj nadajnik RS485
#endif
	USARTE0.DATA=c;
}
asm("sei");
}


