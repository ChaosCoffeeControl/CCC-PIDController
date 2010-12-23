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
#include "pidcontroller.h"

#define OFF (0!=0) // FALSE
#define ON  (0==0) // TRUE

uint32_t _toggle_time;
uint8_t _heater_state;

// function prototypes
void heater_on(void);
void heater_off(void);

void initHeater(void) {
  _toggle_time = TimerRead();
  HEATER_DDR |= (1 << HEATER_PIN);
  heater_off();
}

void loopHeater(void) {
  uint16_t duty_cycle=500;
  if (TimerReached(&_toggle_time, duty_cycle)) {
	if (_heater_state == OFF)
	  heater_on();
	else
	  heater_off();
  }
}

void heater_on(void) {
  HEATER_PORT |= (1 << HEATER_PIN);
  _heater_state=ON;
}

void heater_off(void) {
  HEATER_PORT &= ~ (1 << HEATER_PIN);
  _heater_state=OFF;
}
