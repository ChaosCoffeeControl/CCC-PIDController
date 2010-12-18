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

#ifndef TEMPSENSORS_H
#define TEMPSENSORS_H 1

#define MAXSENSORS 5

static uint8_t search_sensors(void);
static void uart_put_temp(int16_t decicelsius);
static void uart_put_temp_maxres(int32_t tval);
static void initTempSensors(void);
static void loopTempSensors(void);
static int32_t getHighResTemperature(void);
static int16_t getTemperature(void);

#endif /* TEMPSENSORS_H */

