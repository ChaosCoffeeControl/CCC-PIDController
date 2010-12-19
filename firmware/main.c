/**
 * This file is part of C8H10N4O2.
 *
 * (c) Mathias Dalheimer <md@gonium.net>, 2010
 *
 * C8H10N4O2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * C8H10N4O2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with C8H10N4O2. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <avr/version.h>
#if __AVR_LIBC_VERSION__ < 10606UL
#error "please update to avrlibc 1.6.6 or newer, not tested with older versions"
#endif

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"
#include "uart_addon.h"
#include "tempsensors.c"
#include "timer.h"
#include "config.h"


int main( void ) {
  uint32_t currentTemp;	
  uint32_t starttime;
  char s[10];

  // Interrupt-sensitive initializations, before the interrupts are enabled.
  uart_init((UART_BAUD_SELECT((BAUD),F_CPU)));
  ow_set_bus(&PIND,&PORTD,&DDRD,PD7);
  Timer0Init();
  sei();

  // Initialize everything else.
  uart_puts_P( NEWLINESTR "C8H10N4O2 startup." NEWLINESTR );
  initTempSensors();

  starttime = TimerRead();
  for(;;) {   // main loop
    if (TimerReached(&starttime, 1000)) {
      loopTempSensors();
      currentTemp=getHighResTemperature();
      DS18X20_format_from_maxres( currentTemp, s, 10 );
      uart_put_longint(starttime);
      uart_puts_P(" T:");
      uart_puts( s );
      uart_puts_P( NEWLINESTR );
    }
    //_delay_ms(1000); 
  }
}
