#include <avr/io.h>
#include <avr/pgmspace.h>
#include "status.h"
#include "config.h"

#define FALSE (0!=0) // FALSE
#define TRUE  (0==0) // TRUE

char is_on;

void statusLEDOn(void){
  STATUSLED_PORT |= (1 << STATUSLED_PIN);
  is_on=TRUE;
}

void statusLEDOff(void){
  STATUSLED_PORT &= ~ (1 << STATUSLED_PIN);
  is_on=FALSE;
}

void initStatus(void) {
  STATUSLED_DDR |= (1 << STATUSLED_PIN);
  statusLEDOn();
}

void loopStatus(void) {
  if (is_on)
	statusLEDOff();
  else
	statusLEDOn();
}

