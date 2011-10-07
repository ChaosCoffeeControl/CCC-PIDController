/*
 * Copyright (c) 2011, Fraunhofer ESK
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 * Author: 	G�nter Hildebrandt <guenter.hildebrandt@esk.fraunhofer.de>
 *
 * @(#)$$
 */
#include "metering.h"
#include <avr/io.h>
#include <util/delay.h>
#include "sys/clock.h"
#include "contiki.h"
#include "dev/leds.h"
#include <avr/eeprom.h>
#include "eeprom_variables.h"
#include "dev/leds.h"
#include "relay.h"

/** \brief This is a file internal variable that contains the 16 MSB of the
 *         metering pulse period.
 *
 *         The metering pulse period (32-bit) is in METERING_TIMER_RESOLUTION. For the
 *         AVR microcontroller implementation this is solved by using a 16-bit
 *         timer (Timer1) with a clock frequency of 8 MHz and a prescaler of 1/1024.
 *         The metering_pulse_period is incremented when the 16-bit timer
 *         overflows, representing the 16 MSB. The timer value it self (TCNT1)
 *         is then the 16 LSB.
 *
 */

//local variables
static clock_time_t metering_pulse_period = 0;
static clock_time_t clock_old;
static uint16_t metering_calibration_power;
static uint16_t metering_reference_value;
static uint8_t metering_calibration_state = 0;
static uint16_t metering_power;
static bool metering_calibration = false;

/*calculates the consumed energy on the basis of the counted pulses (num_pulses) in i given period (integration_time)
 * P = 247.4W/Hz * f_CF */
uint16_t
calc_power(uint16_t pulse_period)
{
  uint16_t P;
  P = metering_reference_value / pulse_period;

  return P;
}

void
metering_init(void)
{
  /* Load reference values from EEPROM */
  metering_reference_value = eeprom_read_word((void*) EE_METERING_REF );
  metering_calibration_power = eeprom_read_word((void*)EE_METERING_CAL_LOAD);

  SET_METERING_INT();

  metering_start();
}

void
metering_start(void)
{
  /*Reset variables used in file.*/
  metering_pulse_period = 0;
  metering_power = 0;
  clock_old = clock_time();

  ENABLE_METERING_INTERRUPT(); /* Enable external interrupt. */
}

void
metering_stop(void)
{
  DISABLE_METERING_INTERRUPT(); /* Disable external capture interrupt. */
  metering_reset();
}

void
metering_reset(void)
{
  metering_power = 0;
}

uint16_t
metering_get_power(void)
{
  uint16_t tmp;
  /*check whether measurement is up to date */
  if (clock_time() > clock_old)
	  tmp = (clock_time() - clock_old);
  else
	  tmp = (0xFFFF - clock_old + clock_time() + 1);

  if (tmp > OUT_OF_DATE_TIME * CLOCK_SECOND)
	  metering_power = 0;
  else if (metering_power != 0 && tmp > 2 * (metering_reference_value / metering_power))
	  metering_power = calc_power(tmp);

  return metering_power;
}

bool
metering_calibrate(void)
{
	unsigned char cal_flag = eeprom_read_byte((void*)EE_METERING_CAL_FLAG);
	if (cal_flag == 0xFF) {
		metering_calibration = true;
		leds_on(LEDS_ALL);
		return true;
	}
	else
		return false;
}

//interrupt service routine for the metering interrupt
ISR(METERING_VECT)
{
  //calibration
  if (metering_calibration == true)    {
	  //do 10 measurements
      if (metering_calibration_state < 10)
        {
          if (metering_calibration_state == 0)
            {
              //reset Timer
              metering_pulse_period = clock_time();
            }
          metering_calibration_state++;
        }
      else {
		  //get mean pulse period over 10 cycles
		  if (clock_time() > metering_pulse_period)
				metering_pulse_period = clock_time() - metering_pulse_period;
		  else //overflow of clock_time()
			  metering_pulse_period = (0xFFFF - metering_pulse_period) + clock_time() + 1;

		  metering_pulse_period /= 10;

		  metering_reference_value = metering_pulse_period * metering_calibration_power;

		  //store calibration in EEPROM
		  eeprom_write_word((uint16_t*) EE_METERING_REF, metering_reference_value);

		  //lock calibration by setting flag in eeprom
		  eeprom_write_byte((uint8_t*) EE_METERING_CAL_FLAG, 0x00);

		  metering_calibration_state = 0;
		  metering_calibration = false;
		  relay_leds();
      }
    }

  //measurement
  else
    {
	  //get pulse period
	 if (clock_time() > clock_old)
		 metering_pulse_period = clock_time() - clock_old;
	 else //overflow of clock_time()
	  	 metering_pulse_period = (0xFFFF - clock_old) + clock_time() + 1;

	  clock_old = clock_time();
      //calculate and set electrical power
      metering_power = calc_power(metering_pulse_period);
    }
}



