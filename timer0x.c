/*
 * timer0.c
 *
 *  Created on: 8 kwi 2014
 *      Author: krzysztofklimas@interia.pl
 */

#include "timer0x.h"
#include <util/atomic.h>
#include "pcb.h"

volatile uint8_t *timer0T;
volatile uint8_t ledTimer;

// Timer/Counter TCC0 initialization
void tcc0_init(void)
{
unsigned char s;
unsigned char n;

// Note: The correct PORTC direction for the Compare Channels
// outputs is configured in the ports_init function.

// Save interrupts enabled/disabled state
s=SREG;
// Disable interrupts
cli();

// Disable and reset the timer/counter just to be sure
//tc0_disable(&TCC0);  problem
// Clock source: ClkPer/256
TCC0.CTRLA=TC_CLKSEL_DIV256_gc;
// Mode: Normal Operation, Overflow Int./Event on TOP
// Compare/Capture on channel A: Off
// Compare/Capture on channel B: Off
// Compare/Capture on channel C: Off
// Compare/Capture on channel D: Off
TCC0.CTRLB=(0<<TC0_CCDEN_bp) | (0<<TC0_CCCEN_bp) | (0<<TC0_CCBEN_bp) | (0<<TC0_CCAEN_bp) |
	TC_WGMODE_NORMAL_gc;
// Capture event source: None
// Capture event action: None
TCC0.CTRLD=TC_EVACT_OFF_gc | TC_EVSEL_OFF_gc;

// Set Timer/Counter in Normal mode
TCC0.CTRLE=(0<<TC0_BYTEM_bp);

// Overflow interrupt: Low Level
// Error interrupt: Disabled
TCC0.INTCTRLA=TC_ERRINTLVL_OFF_gc | TC_OVFINTLVL_LO_gc;

// Compare/Capture channel A interrupt: Disabled
// Compare/Capture channel B interrupt: Disabled
// Compare/Capture channel C interrupt: Disabled
// Compare/Capture channel D interrupt: Disabled
TCC0.INTCTRLB=TC_CCDINTLVL_OFF_gc | TC_CCCINTLVL_OFF_gc | TC_CCBINTLVL_OFF_gc | TC_CCAINTLVL_OFF_gc;

// High resolution extension: Off
//HIRESC.CTRLA&= ~HIRES_HREN0_bm;   problem

// Advanced Waveform Extension initialization
// Optimize for speed

// Disable locking the AWEX configuration registers just to be sure
n=MCU.AWEXLOCK & (~MCU_AWEXCLOCK_bm);
CCP=CCP_IOREG_gc;
MCU.AWEXLOCK=n;
// Restore optimization for size if needed

// Pattern generation: Off
// Dead time insertion: Off
AWEXC.CTRL=(0<<AWEX_PGM_bp) | (0<<AWEX_CWCM_bp) | (0<<AWEX_DTICCDEN_bp) | (0<<AWEX_DTICCCEN_bp) |
	(0<<AWEX_DTICCBEN_bp) | (0<<AWEX_DTICCAEN_bp);

// Fault protection initialization
// Fault detection on OCD Break detection: On
// Fault detection restart mode: Latched Mode
// Fault detection action: None (Fault protection disabled)
AWEXC.FDCTRL=(AWEXC.FDCTRL & (~(AWEX_FDDBD_bm | AWEX_FDMODE_bm | AWEX_FDACT_gm))) |
	(0<<AWEX_FDDBD_bp) | (0<<AWEX_FDMODE_bp) | AWEX_FDACT_NONE_gc;
// Fault detect events:
// Event channel 0: Off
// Event channel 1: Off
// Event channel 2: Off
// Event channel 3: Off
// Event channel 4: Off
// Event channel 5: Off
// Event channel 6: Off
// Event channel 7: Off
//AWEXC.FDEMASK=0b00000000;  problem
// Make sure the fault detect flag is cleared
AWEXC.STATUS|=AWEXC.STATUS & AWEX_FDF_bm;

// Clear the interrupt flags
//TCC0.INTFLAGS=TCC0.INTFLAGS; problem
// Set Counter register
TCC0.CNT=0x0000;
// Set Period register
TCC0.PER=0x04E2;
// Set channel A Compare/Capture register
TCC0.CCA=0x0000;
// Set channel B Compare/Capture register
TCC0.CCB=0x0000;
// Set channel C Compare/Capture register
TCC0.CCC=0x0000;
// Set channel D Compare/Capture register
TCC0.CCD=0x0000;

// Restore interrupts enabled/disabled state
SREG=s;
}

void registerTimerInTimer0( volatile uint8_t * time){
	timer0T = time;
}

// Timer/Counter TCC0 Overflow/Underflow interrupt service routine
ISR (TCC0_OVF_vect)
{
	if(*timer0T)timer0T--;
	ledTimer++;
	LED1_TOGLE;

}
