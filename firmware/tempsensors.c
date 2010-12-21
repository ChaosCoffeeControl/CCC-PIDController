/**
 * This file is part of C8H10N4O2.
 *
 * (c) Mathias Dalheimer <md@gonium.net>, 2010
 * This file is based on  Martin Thomas' DS18X20 library
 *  http://gandalf.arubi.uni-kl.de/avr_projects/tempsensor/index.html
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
#include <avr/interrupt.h>
#include <string.h>

#include "tempsensors.h"
#include "uart.h"
#include "uart_addon.h"
#include "onewire.h"
#include "ds18x20.h"

uint8_t nSensors, i;
volatile int16_t decicelsius;
volatile int32_t temp_eminus4;
uint8_t error;
uint8_t gSensorIDs[MAXSENSORS][OW_ROMCODE_SIZE];

uint32_t starttime;

uint8_t search_sensors(void) {
  uint8_t i;
  uint8_t id[OW_ROMCODE_SIZE];
  uint8_t diff, nSensors;
  uart_puts_P( NEWLINESTR "Scanning Bus for DS18X20" NEWLINESTR );
  ow_reset();
  nSensors = 0;
  diff = OW_SEARCH_FIRST;
  while ( diff != OW_LAST_DEVICE && nSensors < MAXSENSORS ) {
	DS18X20_find_sensor( &diff, &id[0] );
	if( diff == OW_PRESENCE_ERR ) {
	  uart_puts_P( "No Sensor found" NEWLINESTR );
	  break;
	}
	if( diff == OW_DATA_ERR ) {
	  uart_puts_P( "Bus Error" NEWLINESTR );
	  break;
	}
	for ( i=0; i < OW_ROMCODE_SIZE; i++ )
	  gSensorIDs[nSensors][i] = id[i];
	nSensors++;
  }
  return nSensors;
}

void uart_put_temp(int16_t decicelsius) {
  char s[10];
  uart_put_int( decicelsius );
  uart_puts_P(" deci°C, ");
  DS18X20_format_from_decicelsius( decicelsius, s, 10 );
  uart_puts( s );
  uart_puts_P(" °C");
}

int16_t getTemperature(void) {
  return decicelsius;
}

int32_t getHighResTemperature(void) {
  return temp_eminus4;
}

#if DS18X20_MAX_RESOLUTION

void uart_put_temp_maxres(int32_t tval) {
  char s[10];

  uart_put_longint( tval );
  uart_puts_P(" °Ce-4, ");
  DS18X20_format_from_maxres( tval, s, 10 );
  uart_puts( s );
  uart_puts_P(" °C");
}

#endif /* DS18X20_MAX_RESOLUTION */

void initTempSensors(void) {
  uart_puts_P("Searching for OneWire temperature sensors." NEWLINESTR );
  nSensors = search_sensors();
#if DS18X20_VERBOSE
  for (i = 0; i < nSensors; i++ ) {
	uart_puts_P("ID in Bus: ");
	uart_put_int( (int)i + 1);
	uart_puts_P(" - ");
	DS18X20_show_id_uart( &gSensorIDs[i][0], OW_ROMCODE_SIZE );
	uart_puts_P( NEWLINESTR );
  }
#endif
}	

void loopTempSensors(void) {
  error = 0;

  if ( nSensors == 0 ) {
	error++;
  }

  if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL ) 
	  == DS18X20_OK) {

	for ( i = 0; i < nSensors; i++ ) {
	  if ( DS18X20_read_maxres( &gSensorIDs[i][0], &temp_eminus4 )
		  == DS18X20_OK ) {
		;; // we're cool - read a value.
		//uart_put_temp_maxres( temp_eminus4 );
	  }
	  else {
		uart_puts_P( "CRC Error (lost connection?)" );
		uart_puts_P( NEWLINESTR );
		error++;
	  }
	}
  } else {
	uart_puts_P( "Start meas. failed (short circuit?)" );
	error++;
  }

  if ( error ) {
	uart_puts_P( "*** problems - rescanning bus ..." );
	nSensors = search_sensors();
	uart_put_int( (int) nSensors );
	uart_puts_P( " DS18X20 Sensor(s) available" NEWLINESTR );
	error = 0;
  }
}
