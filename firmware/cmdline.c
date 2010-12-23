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
#include <avr/pgmspace.h>

#include "cmdline.h"
#include "pidcontroller.h"
#include "uart.h"
#include "uart_addon.h"
#include "tempsensors.h"
#include "timer.h"
#include "ds18x20.h"
#include "config.h"


uint32_t cmdline_looptime;
int menuEnabled;

void initCommandLine(void) {
  uart_puts_P( NEWLINESTR "C8H10N4O2 startup." NEWLINESTR );
  cmdline_looptime = TimerRead();
  menuEnabled=0;
}

void printHelp(void) {
  uart_puts_P("C8H10N4O2 menu" NEWLINESTR );
  uart_puts_P("Press 'x' to leave, '?' for help" NEWLINESTR );
  uart_puts_P(" p: print PID configuration" NEWLINESTR );
  uart_puts_P(" r: reset PID configuration to default values" NEWLINESTR );
  uart_puts_P(" d: toggle PID debug output" NEWLINESTR );
}
  
void printStatus(void) {
  uint32_t currentTemp, currentTime;	
  uint16_t setpoint;
  char s[10];

  currentTime=TimerRead();
  currentTemp=getHighResTemperature();
  setpoint=getPIDSetpoint();
  uart_puts_P("#:");
  uart_put_longint(currentTime/1000);
  DS18X20_format_from_maxres( currentTemp, s, 10 );
  uart_puts_P(" T:");
  uart_puts( s );
  uart_puts_P(" S:");
  uart_put_float( setpoint );
  uart_puts_P( NEWLINESTR );
}

void loopCommandLine(void) {
  // print status if we're not in the menu
  if (! menuEnabled) {
    if (TimerReached(&cmdline_looptime, 1000)) {
      printStatus();
    }
  } 
  // check for serial command
  unsigned int c = uart_getc();
  if (!(c & UART_NO_DATA)) {
    uart_puts_P("Received: ");
    uart_putc(c);
    uart_puts_P( NEWLINESTR );
    if (! menuEnabled) {
      menuEnabled=1;
      printHelp();
    }
    switch((char) c) {
      case 'x': case 'X':  // terminates menu
        uart_puts_P("Leaving menu." NEWLINESTR);
        menuEnabled=0; break;
      case 'p': case 'P':  // show PID values
        printPID(); break;
      case 'r': case 'R':  // reset PID values
        restorePIDDefault(); break;
      case 'd': case 'D':  // print out PID debug data
        togglePIDDebug(); break;
      case '?': // show menu
        printHelp(); break;
    }
  }
}
