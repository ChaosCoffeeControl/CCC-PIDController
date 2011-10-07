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

static struct etimer pid_controller_periodic_timer;

//local variables
//static float temperature_value=0.0;
//static char temperature_string_buffer[10];

PROCESS(pid_controller_process, "HEXABUS Socket PID controller Process");
AUTOSTART_PROCESSES(&pid_controller_process);

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

	// wait 3 second
	etimer_set(&pid_controller_periodic_timer, CLOCK_CONF_SECOND*3);
	// wait until the timer has expired
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

	// set the timer to 1 sec for use in the loop
	etimer_set(&pid_controller_periodic_timer, 4*CLOCK_SECOND);

	//everytime the timer event appears, print a debug message and reset the timer
	while(1){
		PROCESS_WAIT_EVENT();
		if (ev == PROCESS_EVENT_TIMER) {
			PRINTF("pid_controller controlling\r\n");
		}
		etimer_reset(&pid_controller_periodic_timer);
	}
	PROCESS_END();
}
