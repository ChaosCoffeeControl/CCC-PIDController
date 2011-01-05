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

#ifndef CONFIG_H
#define CONFIG_H 1

/**
 * This file contains all the configuration settings needed.
 */

#define TIMER_RESOLUTION 1000 // Timer has 1000µs=1ms resolution 
#define BAUD 9600

// Onewire Bus location
#define OW_INPUT PIND
#define OW_PORT PORTD
#define OW_DDR DDRD
#define OW_PIN PD7

// Heater SSR location
#define HEATER_PORT PORTB
#define HEATER_DDR DDRB
#define HEATER_PIN PB1

// Status LED location
#define STATUSLED_PORT PORTD
#define STATUSLED_DDR DDRD
#define STATUSLED_PIN PD5


// PID default values: These lead to a small oscillation around
// 103°C.
#define DEFAULT_SETPOINT 103
// These are safe default values.
//#define DEFAULT_P_GAIN 30
//#define DEFAULT_I_GAIN 0
//#define DEFAULT_D_GAIN 0
// These have been edited for my Silvia. YMMV.
#define DEFAULT_P_GAIN 60
#define DEFAULT_I_GAIN 0.48
#define DEFAULT_D_GAIN 1800 
#define WINDUP_GUARD 1000


#endif /* CONFIG_H */

