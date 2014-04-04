/*
 * main.c
 *
 *  Created on: 18 gru 2013
 *      Author: krzysztof
 */

#include <avr/io.h>
#include <util/delay.h>
//#include <avr/interrupt.h>
#include "pcb.h"
#include "./USART/usartc0.h"
#include "./USART/usartd0.h"
#include "./XMEGA_CLK/xmega_clk.h"

int main()
{
	unsigned char n;



	// Interrupt system initialization
	// Optimize for speed

	// Make sure the interrupts are disabled
	asm("cli");
	// Low level interrupt: On
	// Round-robin scheduling for low level interrupt: Off
	// Medium level interrupt: Off
	// High level interrupt: Off
	// The interrupt vectors will be placed at the start of the Application FLASH section

	n=(PMIC.CTRL & (~(PMIC_RREN_bm | PMIC_IVSEL_bm | PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm))) |
		PMIC_LOLVLEN_bm;
	CCP=CCP_IOREG_gc;
	PMIC.CTRL=n;
	// Set the default priority for round-robin scheduling
		PMIC.INTPRI=0x00;
		// Restore optimization for size if needed

		system_clocks_init();
		usartc0_init();
		usartd0_init();

		PORTC.DIRSET = 0X10;
		PORTC.OUTTGL = 0X10;

		asm("sei");

		CONFIG_RADIO_DIR_OUT;
		CONFIG_RADIO_OFF;
		_delay_ms(4000);
		CONFIG_RADIO_ON;

		while(1){
//			putchard0(0x41);
//			_delay_ms(1000);
		}

}


