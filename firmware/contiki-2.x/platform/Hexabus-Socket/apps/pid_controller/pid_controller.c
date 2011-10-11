/*
 * pid_controller.c
 *
 *  Created on: 12.08.2011
 *      Author: Mathias Dalheimer
 */

#include "pid_controller.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "sys/clock.h"
#include "contiki.h"
#include "dev/leds.h"
#include <avr/eeprom.h>
#include "eeprom_variables.h"
#include "dev/leds.h"
#include "debug.h"
#include "config.h"
#include "temperature.h"
#include "relay.h"

static struct etimer pid_controller_periodic_timer;

//local variables
//static float temperature_value=0.0;
static char string_buffer[10];

PROCESS(pid_controller_process, "HEXABUS Socket PID controller Process");
AUTOSTART_PROCESSES(&pid_controller_process);
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
uint8_t debug=1;//FALSE;
uint16_t loopcounter=0;
int16_t pidval =0;

// forward declarations
void ee_save(void);
void ee_load(void);

void initPIDController(void) {
  // load configuration from EEPROM
  ee_load();
}

float getPIDSetpoint(void) {
  return _pid_data.setpoint;
}

void setPIDSetpoint(float new) {
  _pid_data.setpoint=new;
}

float getPID_P(void) {
  return _pid_data.p_gain;
}

float getPID_I(void) {
  return _pid_data.i_gain;
}

float getPID_D(void) {
  return _pid_data.d_gain;
}

void setPID_P(float new) {
  _pid_data.p_gain=new;
}

void setPID_I(float new) {
  _pid_data.i_gain=new;
}

void setPID_D(float new) {
  _pid_data.d_gain=new;
}

void savePIDConfig(void) {
PRINTF("Writing PID controller configuration to EEPROM.\r\n");
  ee_save();
}

// copied from Tim Hirzel, see
// http://www.arduino.cc/playground/Main/BarebonesPIDForEspresso#pid

void autotunePID(void) {
  PRINTF("Autotuning PID controller.");
}



void restorePIDDefault(void) {
  PRINTF("Writing PID controller default configuration to EEPROM.\r\n");
  _pid_data.setpoint=DEFAULT_SETPOINT;
  _pid_data.p_gain=  DEFAULT_P_GAIN;
  _pid_data.i_gain=  DEFAULT_I_GAIN;
  _pid_data.d_gain=  DEFAULT_D_GAIN;
  //ee_save();
  // make sure the NaN values are not present any more
  _iState =	0;
  _last_temp =	0;
  _p_term=	0;
  _i_term=	0;
  _d_term =	0;
  _pid_value =	0; 
}

void ee_save(void) {
  eeprom_write_block(&_pid_data, &_ee_pid_data, sizeof(pid_t));
}

void ee_load(void) {
  eeprom_read_block(&_pid_data, &_ee_pid_data, sizeof(pid_t));
}



int16_t get_duty_cycle(void) {
  int16_t retval=(int16_t) _pid_value;
  if (retval > TIMER_RESOLUTION - 1)
    retval=(TIMER_RESOLUTION - 1);
  if (retval < 0)
    retval=0;
  return retval >> 2;
}

void togglePIDDebug(void) {
  debug=!debug;
}
/*---------------------------------------------------------------------------*/
static void
pollhandler(void) {
	PRINTF("----Socket_pid_controller_handler: Process polled\r\n");
}

static void
exithandler(void) {
	PRINTF("----Socket_pid_controller_handler: Process exits.\r\n");
}
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(pid_controller_process, ev, data) {
	PROCESS_POLLHANDLER(pollhandler());
	PROCESS_EXITHANDLER(exithandler());

	// see: http://senstools.gforge.inria.fr/doku.php?id=contiki:examples
	PROCESS_BEGIN();
	PRINTF("pid_controller: process startup.\r\n");
	PROCESS_PAUSE();

	// wait 3 second
	etimer_set(&pid_controller_periodic_timer, CLOCK_CONF_SECOND*4);
	// wait until the timer has expired
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

	// set the timer to 1 sec for use in the loop
	etimer_set(&pid_controller_periodic_timer, 0.5*CLOCK_SECOND);
	ENABLE_RELAY_PWM();
	PRINTF("HORST\r\n");
	//everytime the timer event appears, print a debug message and reset the timer
	while(1){
		PROCESS_YIELD();
		if(etimer_expired(&pid_controller_periodic_timer)) {
			etimer_reset(&pid_controller_periodic_timer);

			//float curTemp=getTemperatureFloat();
			float curTemp=temperature_get();
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
			pidval=get_duty_cycle();

			if (pidval > 0){
			//	ENABLE_RELAY_PWM();
				SET_RELAY_PWM(255-pidval);
			} else{
			//	DISABLE_RELAY_PWM();
				SET_RELAY_PWM(255);
			}

			// eventually, print debug line.
			if (debug) {
				dtostrf(error, 8, 4, &string_buffer);
				PRINTF("e = %s", string_buffer);
				dtostrf(_pid_value, 8, 4, &string_buffer);
				PRINTF(", pid value = %s", string_buffer);
				dtostrf(_last_temp, 9, 4, &string_buffer);
				PRINTF(", temp = %s °C", string_buffer); 
				PRINTF(", pidval = %i", pidval); 

/*
				dtostrf(_pid_value, 9, 4, &string_buffer);
				PRINTF(" PID: %s", string_buffer);
				dtostrf(_p_term, 9, 4, &string_buffer);
				PRINTF(", p: %s", string_buffer);
				dtostrf(_i_term, 9, 4, &string_buffer);
				PRINTF(", i: %s", string_buffer);
				dtostrf(_d_term, 9, 4, &string_buffer);
				PRINTF(", d: %s", string_buffer);
				dtostrf(_iState, 9, 4, &string_buffer);
				PRINTF(", iS: %s", string_buffer);
				dtostrf(_last_temp, 9, 4, &string_buffer);
				PRINTF(", l: %s", string_buffer); 
*/
				PRINTF("\r\n");
			}
			loopcounter++;
		}
	}
	PROCESS_END();
}
