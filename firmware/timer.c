/*********************************************
Target: AVR-GCC
Functions for a universal timer

Copyright (C) 2004 2009 Patrick Crombach Paul van der Hoeven.
Patched 2010 Mathias Dalheimer <md@gonium.net>
This is free software, licensed under the terms of the GNU General
Public License as published by the Free Software Foundation.
**********************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>
#include "timer.h"

volatile int16_t TimerTicks;

//---------------------------------------------------------------------------
void Timer0Init(void)
/* Initializes the Prescaler for Hardware Timer0. To save opcodes we do
NOT preload the Counter with the #define'd TIMEROFFSET. This introduces a
trivial error < 1024x256 CLK_TICKS before the first overflow of the 
counter. That's 0,03s @8Mhz.  */
{
#if defined(__AVR_ATtiny2313__) || defined(__AVR_ATmega168__)
  TCCR0B = PRESCALER;
#else
  TCCR0  = PRESCALER;
#endif
#if defined(__AVR_ATmega168__)
  TIMSK0 |=_BV(TOIE0);	// Enable Interrupt on overflow Timer0
#else
  TIMSK |=_BV(TOIE0);	// Enable Interrupt on overflow Timer0
#endif
//  sei();		// Do this manually. (If everything else is initialized)
}

//---------------------------------------------------------------------------
int8_t TimerReached(int16_t *pEndTime,const int16_t Delay)
/* Non Blocking Timer function. Returns 1 if "Delay" time has passed.
Returns 0 if this amount of time has not yet passed. *pEndTime is used for
storage of the timing period. */
{
  int16_t Now;
  int16_t Diff;

  Now=TimerRead();
  Diff=*pEndTime-Now;

  if( Diff > Delay)		// Error in Calculation,
  { 
    *pEndTime = Now+Delay;	// Recalculate and exit.
    return 0;
  }  

  if(Diff<=0)		// EndTime reached: Calculate new time and exit
  { 
    *pEndTime = Now+Delay;	// Calculate new expire time.

    if(Delay+Diff > 0)		// Is Diff within range to do a time correction?
      *pEndTime += Diff;	// Remember: Diff is 0 or it's Negative!
    return 1;
  }
  return 0;
}

//---------------------------------------------------------------------------
int16_t TimerRead(void)
/* Atomic read of TimerTicks  */
{
  int16_t Result;
  uint8_t InterruptState=SREG; 	// Save previous interrupt state.

  cli();			// And Disable Global Interrupts.
  Result = TimerTicks;
  SREG=InterruptState;         	// Restore previous interrupt state.
  return Result;
}

//---------------------------------------------------------------------------
SIGNAL(SIG_OVERFLOW0)
/* This Interrupt routine is called if the hardware timer0 overflows.
We reload the timer with a #define'd constant and increment the software
counter. */
{
  TCNT0-=TIMEROFFSET;
  TimerTicks++;
}

/*--------------------------------------------------------------------------
            End of all code.      Below this line is only comment.
-----------------------------------------------------------------------------

Revision List:
2009-04-19 Added GPL.
2009-04-08 Added a check for Diff is within range for a valid correction.
2008-06-20 Added some timer resolutions for 9.216Mhz Crystal.
2008-04-26 Added #define for TCCR0B for the attiny2313.
	Changed definition of "TICKS_PER_SECOND" to "TPS".
2007-09-15 if TimerReached() is called and the time that the timer will expire
	is to far in the future (Bigger then the delay time). Then the funcion
	will Calculate a new delay time.
2006-10-29 Removed functions TimerSet() and TimerDelay(). It's a waste of Flash.
	SIGNAL(SIG_OVERFLOW0) updated with idea from 2006-10-26
2006-10-26 Small change in SIGNAL(SIG_OVERFLOW0) I have to think about that.
	If I'm gonna change this it's probaly also a good idea to change
	the definition of TIMEROFFSET with the: (256-...) stuff.
   TimerDelay() has an error!!!
2006-10-25 Added: #define TICKS_PER_SECOND in timer.h
	Corrected error in PRESCALER & TIMER_OFFSET for 3.6864MHz.
	Added #error PRESCALER cq TIMER_OFFSET not defined in timer.h
2006-10-11 Changed TimerTicks from union to int16_t
2006-10-06 TimerReached(), Removed: if(*pEndTime==0){*pEndTime=Now+Delay;}
2005-10-27 Added TimerSet()
2005-10-26 Removed all u08 etc. typedefs	   
2005-01-29 Changed counter into a Union an placed it in Timer.h
        Added comment to each function in this library.
        Changed signal(sig_overflow0) to reload the counter first.
        Changed Now+Delay-Diff to Now+Delay+Diff in: TimerReached()
        TimerRead disables global interrupt. Was: Timer0overflow int.

---------------------------------------------------------------------------*/


