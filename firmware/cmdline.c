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

#define FALSE               (1!=1)
#define TRUE                (1==1)

uint32_t cmdline_looptime;
int menuEnabled;
float delta = 1.0;
#define MAX_DELTA  100
#define MIN_DELTA  0.01


void initCommandLine(void) {
  uart_puts_P( NEWLINESTR "C8H10N4O2 startup." NEWLINESTR );
  cmdline_looptime = TimerRead();
  menuEnabled=0;
}

void uart_print_delta(void) {
  uart_puts_P("current delta: ");
  uart_put_float(delta);
  uart_puts_P(NEWLINESTR );
}

void printHelp(void) {
  uart_puts_P("C8H10N4O2 menu" NEWLINESTR );
  uart_puts_P("Press 'x' to leave, '?' for help" NEWLINESTR );
  uart_puts_P(" b: print PID configuration" NEWLINESTR );
  uart_puts_P(" o: toggle PID debug output" NEWLINESTR );
  uart_puts_P(" +/-: increase/decrease adjustment delta, ");
  uart_print_delta();
  uart_puts_P(" p/P: increase/decrease p gain" NEWLINESTR );
  uart_puts_P(" i/I: increase/decrease i gain" NEWLINESTR );
  uart_puts_P(" d/D: increase/decrease d gain" NEWLINESTR );
  uart_puts_P(" r: reset PID configuration to default values" NEWLINESTR );
  uart_puts_P(" s: save PID configuration to EEPROM" NEWLINESTR );
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
  uart_puts_P(" D:");
  uart_put_longint( get_duty_cycle() );
  uart_puts_P( NEWLINESTR );
}

void printPID(void) {
  uart_puts_P("Current PID controller configuration:" NEWLINESTR);
  uart_puts_P(" - temperature setpoint: ");
  uart_put_float(getPIDSetpoint());
  uart_puts_P(NEWLINESTR);
  uart_puts_P(" - P gain: ");
  uart_put_float(getPID_P());
  uart_puts_P(", I gain: ");
  uart_put_float(getPID_I());
  uart_puts_P(", D gain: ");
  uart_put_float(getPID_D());
  uart_puts_P(NEWLINESTR);
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
    if (! menuEnabled) {
      menuEnabled=1;
      printHelp();
    }
    switch((char) c) {
      case 'x': case 'X':  // terminates menu
        uart_puts_P("Leaving menu." NEWLINESTR);
        menuEnabled=0; break;
      case 'b': case 'B':  // show PID values
        printPID(); break;
      case 'r': case 'R':  // reset PID values
        restorePIDDefault(); break;
      case 'o': case 'O':  // print out PID debug data
        togglePIDDebug(); break;
      case 'p':
        setPID_P(getPID_P() - delta); printPID(); break;
      case 'P':
        setPID_P(getPID_P() + delta); printPID();break;
      case 'i':
        setPID_I(getPID_I() - delta); printPID(); break;
      case 'I':
        setPID_I(getPID_I() + delta); printPID(); break;
      case 'd':
        setPID_D(getPID_D() - delta); printPID(); break;
      case 'D':
        setPID_D(getPID_D() + delta); printPID(); break;
      case 't':
        setPIDSetpoint(getPIDSetpoint() - delta); printPID(); break;
      case 'T':
        setPIDSetpoint(getPIDSetpoint() + delta); printPID(); break;
      case 's': case 'S':
        savePIDConfig(); break;
      case '+':  {// adjust delta
          delta *= 10.0; 
          if (delta > MAX_DELTA) delta = MAX_DELTA;
          uart_print_delta();
          break;
        }
      case '-':  {// adjust delta
          delta /= 10.0; 
          if (delta < MIN_DELTA) delta = MIN_DELTA;
          uart_print_delta();
          break;
        }

      case '?': // show menu
        printHelp(); break;
    }
  }
}
