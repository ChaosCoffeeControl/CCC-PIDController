#include <avr/pgmspace.h>

#include "cmdline.h"
#include "uart.h"
#include "uart_addon.h"
#include "tempsensors.h"
#include "timer.h"
#include "ds18x20.h"
#include "config.h"

void initCommandLine(void) {
  uart_init((UART_BAUD_SELECT((BAUD),F_CPU)));
  uart_puts_P( NEWLINESTR "C8H10N4O2 startup." NEWLINESTR );
}

void printStatus(void) {
  uint32_t currentTemp, currentTime;	
  char s[10];

  currentTemp=getHighResTemperature();
  currentTime=TimerRead();
  uart_puts_P("#:");
  uart_put_longint(currentTime/1000);
  DS18X20_format_from_maxres( currentTemp, s, 10 );
  uart_puts_P(" T:");
  uart_puts( s );
  uart_puts_P( NEWLINESTR );
}

void loopCommandLine(void) {
  printStatus();
}
