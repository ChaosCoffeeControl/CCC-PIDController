// HeaterControl
// Tim Hirzel 
// Dec 2007
// 
// This file is for controlling a heater via a solid state zero crossing relay

// since these are zero-crossing relays, it makes sense to just match my local
// AC frequency, 60hz
//
// All code released under
// Creative Commons Attribution-Noncommercial-Share Alike 3.0 

#define HEAT_RELAY_PIN 9

float heatcycles; // the number of millis out of 1000 for the current heat amount (percent * 10)

boolean heaterState = 0;

unsigned long heatCurrentTime, heatLastTime;

void setupHeater() {
  pinMode(HEAT_RELAY_PIN , OUTPUT);
}


void updateHeater() {
  boolean h;
  heatCurrentTime = millis();
  if(heatCurrentTime - heatLastTime >= 1000 or heatLastTime > heatCurrentTime) { //second statement prevents overflow errors
    // begin cycle
    _turnHeatElementOnOff(1);  // 
    heatLastTime = heatCurrentTime;   
  } 
  if (heatCurrentTime - heatLastTime >= heatcycles) {
    _turnHeatElementOnOff(0);
  }
}

void setHeatPowerPercentage(float power) {
  if (power <= 0.0) {
    power = 0.0;
  }	
  if (power >= 1000.0) {
    power = 1000.0;
  }
  heatcycles = power;
}

float getHeatCycles() {
  return heatcycles;
}

void _turnHeatElementOnOff(boolean on) {
  digitalWrite(HEAT_RELAY_PIN, on);	//turn pin high
  heaterState = on;
}

// End Heater Control
