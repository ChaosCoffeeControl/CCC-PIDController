/*********************************************
P. vd Hoeven.
Target: AVR-GCC
Functions for a universal timer

Copyright (C) 2004 2009 Patrick Crombach Paul van der Hoeven.
This is free software, licensed under the terms of the GNU General
Public License as published by the Free Software Foundation.
**********************************************/
#ifndef TIMER_H
#define TIMER_H 1

#include <inttypes.h>

#ifndef F_CPU
#error F_CPU not defined in timer.h
#endif

/*---------------------------------------------------------------------------
PRESCALER:	0x00	Disabled.
PRESCALER:	0x01	div1
PRESCALER:	0x02	div8
PRESCALER:	0x03	div64
PRESCALER:	0x04	div256
PRESCALER:	0x05	div1024
PRESCALER:	0x06	Falling Edge
PRESCALER:	0x07	Rising Edge
Calculate:
Target: 5000us	= 5ms
F_CPU	3686400	Hz

3686400/5000 = 737.28 klok cycles.
Prescaler	= div8
TIMEROFFSET	= 92.16


 Hz * RESOLUTION
________________ = OFFSET
      PRE 

8000000 * 5000E-6
_________________ = 156.25
      256
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------
#if F_CPU == 1000000
//---------------------------------------------------------------------------
#if TIMER_RESOLUTION ==1000		// (8*125)/1000000 = 1ms
#  define PRESCALER 0x02
#  define TIMEROFFSET 125
#elif TIMER_RESOLUTION == 2000		// (8*250)/1000000 = 2ms
#  define PRESCALER 0x02
#  define TIMEROFFSET 250		
#elif TIMER_RESOLUTION == 2500		// (64*39)/1000000 = 2.496ms
#  define PRESCALER 0x03
#  define TIMEROFFSET 39
#elif TIMER_RESOLUTION == 5000		// (64*78)/1000000 = 4.992ms
#  define PRESCALER 0x03
#  define TIMEROFFSET 78
#elif TIMER_RESOLUTION == 10000		// (64*156)/1000000 = 9.984ms
#  define PRESCALER 0x03
#  define TIMEROFFSET 156
#elif TIMER_RESOLUTION == 20000		// (256*78)/1000000 = 19.968ms
#  define PRESCALER 0x04
#  define TIMEROFFSET 78
#elif TIMER_RESOLUTION == 50000		// (1024*49)/1000000 = 10.176ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 49
#elif TIMER_RESOLUTION == 100000	// (1024*98)/1000000 = 100.352ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 98
#elif TIMER_RESOLUTION == 200000	// (1024*195)/1000000 = 199.68ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 195
#elif TIMER_RESOLUTION == 250000	// (1024*244)/1000000 = 249.856ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 244
#else
#  error TIMER_RESOLUTION not defined @ 1Mhz
#endif
//---------------------------------------------------------------------------
#elif F_CPU == 3686400
//---------------------------------------------------------------------------
#if TIMER_RESOLUTION == 500
#  define PRESCALER 0x02
#  define TIMEROFFSET 230		// (8*230)/3686400 = 499.132us
#elif TIMER_RESOLUTION ==1000
#  define PRESCALER 0x03
#  define TIMEROFFSET 58		// (64*58)/3686400 = 1006.944us
#elif TIMER_RESOLUTION == 2000
#  define PRESCALER 0x03
#  define TIMEROFFSET 115		// (64*115)/3686400 = 1995.528us
#elif TIMER_RESOLUTION == 2500		// Exact 2.5ms.
#  define PRESCALER 0x05
#  define TIMEROFFSET 9
#elif TIMER_RESOLUTION == 5000		// Exact 5ms.
#  define PRESCALER 0x05
#  define TIMEROFFSET 18
#elif TIMER_RESOLUTION == 10000		// Exact 10ms.
#  define PRESCALER 0x05
#  define TIMEROFFSET 36
#elif TIMER_RESOLUTION == 20000		// Exact 20ms.
#  define PRESCALER 0x05
#  define TIMEROFFSET 72
#elif TIMER_RESOLUTION == 50000		// Exact 50ms.
#  define PRESCALER 0x05
#  define TIMEROFFSET 180
#else
#  error TIMER_RESOLUTION not defined @ 3.6864Mhz
#endif
//---------------------------------------------------------------------------
#elif F_CPU == 4000000
//---------------------------------------------------------------------------
#if TIMER_RESOLUTION == 100		// (8*50)/4000000 = 100us
#  define PRESCALER 0x02
#  define TIMEROFFSET 50
#elif TIMER_RESOLUTION == 125		// (8*62)/4000000 = 124us
#  define PRESCALER 0x02
#  define TIMEROFFSET 62
#elif TIMER_RESOLUTION == 200		// (8*100)/4000000 = 200us
#  define PRESCALER 0x02
#  define TIMEROFFSET 100
#elif TIMER_RESOLUTION == 250		// (8*125)/4000000 = 250us
#  define PRESCALER 0x02
#  define TIMEROFFSET 125
#elif TIMER_RESOLUTION == 500		// (8*250)/4000000 = 500us
#  define PRESCALER 0x02
#  define TIMEROFFSET 250
#elif TIMER_RESOLUTION == 1000		// (64*62)/4000000 = 992us
#  define PRESCALER 0x03
#  define TIMEROFFSET 62
#elif TIMER_RESOLUTION == 2000		// (64*125)/4000000 = 2ms
#  define PRESCALER 0x03
#  define TIMEROFFSET 125
#elif TIMER_RESOLUTION == 2500		// (64*156)/4000000 = 2.496ms
#  define PRESCALER 0x03
#  define TIMEROFFSET 156
#elif TIMER_RESOLUTION == 5000		// (256*78)/4000000 = 4.992ms
#  define PRESCALER 0x04
#  define TIMEROFFSET 78
#elif TIMER_RESOLUTION == 10000		// (256*156)/4000000 = 9.984ms
#  define PRESCALER 0x04
#  define TIMEROFFSET 156
#elif TIMER_RESOLUTION == 20000		// (1024*78)/4000000 = 19.968ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 78
#else
#  error TIMER_RESOLUTION not defined @ 4Mhz
#endif
//---------------------------------------------------------------------------
#elif F_CPU == 8000000
//---------------------------------------------------------------------------
#if TIMER_RESOLUTION ==500		// (64*62)/8000000 = 496us
#  define PRESCALER 0x03
#  define TIMEROFFSET 62
#elif TIMER_RESOLUTION ==1000		// (64*125)/8000000 = 1ms
#  define PRESCALER 0x03
#  define TIMEROFFSET 125
#elif TIMER_RESOLUTION == 2000		// (64*250)/8000000 = 2ms
#  define PRESCALER 0x03
#  define TIMEROFFSET 250		
#elif TIMER_RESOLUTION == 2500		// (256*78)/8000000 = 2.496ms
#  define PRESCALER 0x04
#  define TIMEROFFSET 78
#elif TIMER_RESOLUTION == 5000		// (256*156)/8000000 = 4.992ms
#  define PRESCALER 0x04
#  define TIMEROFFSET 156
#elif TIMER_RESOLUTION == 10000		// (1024*78)/8000000 = 9.984ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 78
#elif TIMER_RESOLUTION == 20000		// (1024*156)/8000000 = 19.968ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 156
#elif TIMER_RESOLUTION == 25000		// (1024*195)/8000000 = 24.96ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 195
#else
#  error TIMER_RESOLUTION not defined @ 8Mhz
#endif
//---------------------------------------------------------------------------
#elif F_CPU == 9216000
//---------------------------------------------------------------------------
#if TIMER_RESOLUTION == 100		// (8*115)/9.216MHz = 99.826us
#  define PRESCALER 0x02
#  define TIMEROFFSET 115
#elif TIMER_RESOLUTION == 125		// (64*18)/9.216Mhz = 125us
#  define PRESCALER 0x03
#  define TIMEROFFSET 18
#elif TIMER_RESOLUTION == 200		// (64*29)/9.216MHz = 201.3888us
#  define PRESCALER 0x03
#  define TIMEROFFSET 29
#elif TIMER_RESOLUTION == 250		// (64*36)/9.216Mhz = 250us
#  define PRESCALER 0x03
#  define TIMEROFFSET 36
#elif TIMER_RESOLUTION == 500		// (64*72)/9.216Mhz = 500us
#  define PRESCALER 0x03
#  define TIMEROFFSET 72
#elif TIMER_RESOLUTION == 1000		// (1024*9)/ 9.216Mhz = 1.000ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 9
#elif TIMER_RESOLUTION == 2000		// (1024*18)/ 9.216Mhz = 2.000ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 18
#elif TIMER_RESOLUTION == 2500		// (256*90)/ 9.216Mhz = 2.500ms
#  define PRESCALER 0x04
#  define TIMEROFFSET 90
#elif TIMER_RESOLUTION == 5000		// (1024*45)/ 9.216Mhz = 5.000ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 45
#elif TIMER_RESOLUTION == 10000		// (1024*90)/ 9.216Mhz = 10.000ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 90
#elif TIMER_RESOLUTION == 20000		// (1024*180)/ 9.216Mhz = 20.000ms
#  define PRESCALER 0x05
#  define TIMEROFFSET 180
#else
#  error TIMER_RESOLUTION not defined @ 9.216Mhz
#endif
//---------------------------------------------------------------------------
#elif F_CPU == 10000000
//---------------------------------------------------------------------------
#if TIMER_RESOLUTION == 1000
#  define PRESCALER 0x05
#  define TIMEROFFSET 10	// (1024*10)/10MHz= 1.024ms
#elif TIMER_RESOLUTION == 2000
#  define PRESCALER 0x05
#  define TIMEROFFSET 20	// (1024*20)/10MHz=2.048ms
#elif TIMER_RESOLUTION == 5000
#  define PRESCALER 0x05
#  define TIMEROFFSET 49	// (1024*49)/10MHz=5.0176ms
#elif TIMER_RESOLUTION == 10000
#  define PRESCALER 0x05
#  define TIMEROFFSET 98	// (1024*98)/10MHz=10.0352ms
#elif TIMER_RESOLUTION == 20000
#  define PRESCALER 0x05
#  define TIMEROFFSET 195	// (1024*195)/10MHz=19.968ms
#else
#  error TIMER_RESOLUTION not defined @ 10Mhz
#endif
//---------------------------------------------------------------------------
#elif F_CPU == 11059200
//---------------------------------------------------------------------------
#if TIMER_RESOLUTION == 10
#  define PRESCALER 0x01		// Cnt wit F_CPU
#  define TIMEROFFSET 111	// 111/11.0592MHz= 10.0369us
#elif TIMER_RESOLUTION == 20
#  define PRESCALER 0x01
#  define TIMEROFFSET 221	// 221/11.0592MHz= 19.9983us
#elif TIMER_RESOLUTION == 50
#  define PRESCALER 0x02
#  define TIMEROFFSET 69	// (8*69)/11.0592MHz= 49.9132us
#elif TIMER_RESOLUTION == 100
#  define PRESCALER 0x02
#  define TIMEROFFSET 138	// (8*138)/11.0592MHz= 99.826us
#elif TIMER_RESOLUTION == 200
#  define PRESCALER 0x03
#  define TIMEROFFSET 35	// (64*35)/11.0592MHz= 202.546us
#elif TIMER_RESOLUTION == 250
#  define PRESCALER 0x03
#  define TIMEROFFSET 43	// (64*43)/11.0592MHz= 248.843us
#elif TIMER_RESOLUTION == 500
#  define PRESCALER 0x03
#  define TIMEROFFSET 86	// (64*86)/11.0592MHz= 497.685us
#elif TIMER_RESOLUTION == 1000
#  define PRESCALER 0x03
#  define TIMEROFFSET 173	// (64*173)/11.0592MHz= 1.001157ms
#elif TIMER_RESOLUTION == 2500 
#  define PRESCALER 0x05
#  define TIMEROFFSET 27	// (1024*27)/11.0592MHz=  2.500ms
#elif TIMER_RESOLUTION == 5000
#  define PRESCALER 0x05
#  define TIMEROFFSET 54	// (1024*54)/11.0592MHz=  5.000ms
#elif TIMER_RESOLUTION == 10000
#  define PRESCALER 0x05
#  define TIMEROFFSET 108	// (1024*108)/11.0592MHz= 10.000ms
#elif TIMER_RESOLUTION == 20000
#  define PRESCALER 0x05
#  define TIMEROFFSET 216	// (1024*216)/11.0592MHz= 20.000ms
#else
#  error TIMER_RESOLUTION not defined @ 11.0592Mhz
#endif
//---------------------------------------------------------------------------
#elif F_CPU == 16000000
//---------------------------------------------------------------------------
#if TIMER_RESOLUTION == 10000
#  define PRESCALER 0x05
#  define TIMEROFFSET 98	// (1024*98)/10MHz=10.0352ms
#elif TIMER_RESOLUTION == 20000
#  define PRESCALER 0x05
#  define TIMEROFFSET 195	// (1024*195)/10MHz=19.968ms
#else
#  error TIMER_RESOLUTION not defined @ 16Mhz
#endif

//---------------------------------------------------------------------------
#else
#  error F_CPU is undefined or defined to wrong val
#endif
//---#if F_CPU == ...... ----------------------------------------------------

// Just an extra precaution to make sure we catch these errors.
#ifndef PRESCALER
#error  PRESCALER not defined in timer.h
#endif

#ifndef TIMEROFFSET
#error  TIMEROFFSET not defined in timer.h
#endif

#define TPS	((1000000L)/(TIMER_RESOLUTION))	// Ticks Per Second.

volatile int16_t TimerTicks;
//---------------------------------------------------------------------------
// Using this small library is very simple.
// At startup the hardware timer has to be enabled with the function:
void Timer0Init(void);
// The proper function of this timer depends on 3 items:
// 1). #define F_CPU
// 2). #define TIMER_RESOLUTION
// 3). Global interrupts must be enabled with sei()

// If the hardware timer is running, the function:
int8_t TimerReached(int16_t *, int16_t);
// returns 1 in the specified period has expired or 0 otherwise.

int16_t TimerRead(void);

#endif  // #ifdef TIMER_H


