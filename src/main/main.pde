// BBCC Main
// Tim Hirzel
// February 2008
//
// Main file for the Bare Bones Coffee Controller PID 
// setup for Arduino.
// This project is set up such that each tab acts as a 
// "module" or "library" that incporporates some more 
// functionality.  Each tab correlates 
// to a particular device (Nunchuck),  protocol (ie. SPI), 
// or Algorithm (ie. PID).

// The general rule for any of these tabs/sections is that 
// if they include a setup* or update* function, those should be added
// into the main setup and main loop functions. Also, in main loop, and in 
// extra code, delays should probably be avoided.
// Instead, use millis() and check for a certain interval to have passed.
//
// All code released under
// Creative Commons Attribution-Noncommercial-Share Alike 3.0 


// These are addresses into EEPROM memory.  The values to be stores are floats which 
// need 4 bytes each.  Thus 0,4,8,12,...
#define PGAIN_ADR 0
#define IGAIN_ADR 4
#define DGAIN_ADR 8

#define ESPRESSO_TEMP_ADDRESS 12
//#define STEAM_TEMP_ADDRESS 12  // steam temp currently not used with bare bones setup

#define PID_UPDATE_INTERVAL 200 // milliseconds


float targetTemp;  //current temperature goal
float heatPower; // 0 - 1000  milliseconds on per second

unsigned long lastPIDTime;  // most recent PID update time in ms 

void setup()
{
  setupPID(PGAIN_ADR, IGAIN_ADR, DGAIN_ADR ); // Send addresses to the PID module 
  targetTemp = readFloat(ESPRESSO_TEMP_ADDRESS); // from EEPROM. load the saved value
  lastPIDTime = millis();
  // module setup calls
  setupHeater();
  setupSerialInterface();
  setupTempSensor();
}

void setTargetTemp(float t) {
  targetTemp = t;
  writeFloat(t, ESPRESSO_TEMP_ADDRESS);
}

float getTargetTemp() {
  return targetTemp;
}


void loop()
{  
  // this call interprets characters from the serial port
  // its a very basic control to allow adjustment of gain values, and set temp
  updateSerialInterface(); 
  updateTempSensor();

  // every second, udpate the current heat control, and print out current status

  // This checks for rollover with millis()
  if (millis() < lastPIDTime) {
    lastPIDTime = 0;
  }
  if ((millis() - lastPIDTime) > PID_UPDATE_INTERVAL) {
    lastPIDTime +=  PID_UPDATE_INTERVAL;
    heatPower = updatePID(targetTemp, getFreshTemp());
    setHeatPowerPercentage(heatPower);

  }  
  updateHeater();

}

// END BBCC Main
