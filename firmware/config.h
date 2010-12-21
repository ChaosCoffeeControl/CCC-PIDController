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

#endif /* CONFIG_H */

