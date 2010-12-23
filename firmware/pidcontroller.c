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
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>

#include "pidcontroller.h"
#include "config.h"
#include "uart.h"
#include "tempsensors.h"
#include "uart_addon.h"

#define FALSE (0!=0)
#define TRUE  (0==0)

// pid datastructures.
typedef struct {
  float setpoint;
  float p_gain, i_gain, d_gain;
} pid_t;
pid_t _pid_data;
pid_t _ee_pid_data EEMEM;

// work variables
float _iState = 0;
float _last_temp = 0;
float _p_term, _i_term, _d_term = 0;
float _pid_value = 0; 
uint8_t debug=FALSE;

// forward declarations
void ee_save(void);
void ee_load(void);

void initPIDController(void) {
  // load configuration from EEPROM
  ee_load();
}

// copied from Tim Hirzel, see
// http://www.arduino.cc/playground/Main/BarebonesPIDForEspresso#pid
void loopPIDController(void) {
  float windup_guard;
  float curTemp=getTemperatureFloat();

  float error = _pid_data.setpoint - curTemp;

  // the pTerm is the view from now, the pgain judges 
  // how much we care about error we are this instant.
  _p_term = _pid_data.p_gain * error;

  // iState keeps changing over time; it's 
  // overall "performance" over time, or accumulated error
  _iState += error;

  // Use a static windup guard to keep the balance of the istate.
  if (_iState > WINDUP_GUARD) 
    _iState = WINDUP_GUARD;
  else if (_iState < -WINDUP_GUARD) 
    _iState = -WINDUP_GUARD;
  _i_term = _pid_data.i_gain * _iState;

  // the dTerm, the difference between the temperature now
  //  and our last reading, indicated the "speed," 
  // how quickly the temp is changing. (aka. Differential)
  _d_term = (_pid_data.d_gain * (curTemp - _last_temp));

  // now that we've use lastTemp, put the current temp in
  // our pocket until for the next round
  _last_temp = curTemp;

  // the magic feedback bit
  _pid_value = _p_term + _i_term - _d_term;

  // eventually, print debug line.
  if (debug) {
    uart_puts_P(" PID: ");
    uart_put_float(_pid_value);
    uart_puts_P(", p: ");
    uart_put_float(_p_term);
    uart_puts_P(", i: ");
    uart_put_float(_i_term);
    uart_puts_P(", d: ");
    uart_put_float(_d_term);
    uart_puts_P(", iS: ");
    uart_put_float(_iState);
    uart_puts_P(", l: ");
    uart_put_float(_last_temp);
    uart_puts_P(NEWLINESTR);
  }
}

void autotunePID(void) {
  uart_puts_P("Autotuning PID controller." NEWLINESTR);
}

void printPID(void) {
  uart_puts_P("Current PID controller configuration:" NEWLINESTR);
  uart_puts_P(" - temperature setpoint: ");
  uart_put_float(_pid_data.setpoint);
  uart_puts_P(NEWLINESTR);
  uart_puts_P(" - P gain: ");
  uart_put_float(_pid_data.p_gain);
  uart_puts_P(", I gain: ");
  uart_put_float(_pid_data.i_gain);
  uart_puts_P(", D gain: ");
  uart_put_float(_pid_data.d_gain);
  uart_puts_P(NEWLINESTR);
}

void restorePIDDefault(void) {
  uart_puts_P("Writing PID controller default configuration to EEPROM." NEWLINESTR);
  _pid_data.setpoint=DEFAULT_SETPOINT;
  _pid_data.p_gain=DEFAULT_P_GAIN;
  _pid_data.i_gain=DEFAULT_I_GAIN;
  _pid_data.d_gain=DEFAULT_D_GAIN;
  ee_save();
  // make sure the NaN values are not present any more
  _iState = 0;
  _last_temp = 0;
  _p_term, _i_term, _d_term = 0;
  _pid_value = 0; 
}

void ee_save(void) {
  eeprom_write_block(&_pid_data, &_ee_pid_data, sizeof(pid_t));
}

void ee_load(void) {
  eeprom_read_block(&_pid_data, &_ee_pid_data, sizeof(pid_t));
}

float getPIDSetpoint(void) {
  return _pid_data.setpoint;
}

float getPIDValue(void) {
  return _pid_value;
}

void togglePIDDebug(void) {
  debug=!debug;
}
