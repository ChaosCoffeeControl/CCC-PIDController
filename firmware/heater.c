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

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "heater.h"
#include "config.h"
#include "timer.h"
#include "uart.h"
#include "uart_addon.h"
#include "pidcontroller.h"

#define FALSE (0!=0) // FALSE
#define TRUE  (0==0) // TRUE

uint32_t _toggle_time;
// Start with heater switched off.
uint32_t _on_cycle=0; 
uint32_t _off_cycle=1000; 

// function prototypes
void heater_on(void);
void heater_off(void);

void initHeater(void) {
  _toggle_time = TimerRead();
  HEATER_DDR |= (1 << HEATER_PIN);
  heater_off();
}

// call directly after the PID algorithm has been updated.
// a full cycle is always 1000ms. The heating occurs always in the first
// phase, the heater is then switched of for the second phase of the cycle.
void updateHeater(void) {
  if (get_duty_cycle() > 0) {
	_on_cycle=TimerRead();
	_off_cycle=_on_cycle + get_duty_cycle();
  } else {
	_on_cycle=TimerRead();
	_off_cycle=_on_cycle; 
  }
  /*uart_puts_P("UPDATE: timer: ");
  uart_put_longint(TimerRead());
  uart_puts_P(" on: ");
  uart_put_longint(_on_cycle);
  uart_puts_P(" off: ");
  uart_put_longint(_off_cycle);
  uart_puts_P(NEWLINESTR);*/
}

// TODO: Consider moving this to an interrupt-based routine.
void loopHeater(void) {
  uint32_t time=TimerRead();
  if (time > _on_cycle && time < _off_cycle) {
	  heater_on();
  } else {
	heater_off();
  }
}

void heater_on(void) {
  HEATER_PORT |= (1 << HEATER_PIN);
}

void heater_off(void) {
  HEATER_PORT &= ~ (1 << HEATER_PIN);
}
