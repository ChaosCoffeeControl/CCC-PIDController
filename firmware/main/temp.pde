// With the AD 595, this process is just a matter of doing some math on an 
// analog input
//
// Thanks to Karl Gruenewald for the conversion formula 
// All code released under
// Creative Commons Attribution-Noncommercial-Share Alike 3.0 

// This current version is based on sensing temperature with 
// an AD595 and thermocouple through an A/D pin.  Any other 
// sensor could be used by replacing this one function.
// feel free to use degrees C as well, it will just give a different 
// PID tuning than those from F.
// 

#include <OneWire.h>
#define TEMP_SENSOR_PIN 7

/* DS18S20 Temperature chip i/o */
OneWire  ds(TEMP_SENSOR_PIN); 
byte addr[8];

float latestReading = 0.0;

void setupTempSensor() {
  byte i;
  byte present = 0;
  byte data[12];
  
  
  if ( !ds.search(addr)) {
    //Serial.print("No more addresses.\n");
    ds.reset_search();
    delay(250);
    return;
  }
  
  //Serial.print("R=");
  //for( i = 0; i < 8; i++) {
  //  Serial.print(addr[i], HEX);
  //  Serial.print(" ");
  //}

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
  }
  
  if ( addr[0] != 0x10) {
      Serial.print("Device is not a DS18S20 family device.\n");
      return;
  }
  

}  

void convertAndPrint(int LowByte, int HighByte) {
  int TReading, SignBit, Tc_100, Whole, Fract;
  char buf[20];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (TReading*100/2);
  latestReading = Tc_100/100.0;  
  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 100;
  //sprintf(buf, "%c%d.%d deg C     ",SignBit ? '-' : '+', Whole, Fract < 10 ? 0 : Fract);
  //Serial.print(buf); 
}

// called continuously from the main loop. Detect when to start update.
void updateTempSensor() {
  byte i;
  byte present = 0;
  byte data[12];
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44,0);         // start conversion, with parasite power on at the end
  
  delay(100);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print("P=");
  //Serial.print(present,HEX);
  //Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print( OneWire::crc8( data, 8), HEX);
  
  //Serial.print(millis()/1000);
  //Serial.print(": ");
  convertAndPrint(data[0], data[1]);
  
  //Serial.println();
  
}

// return the last reading.
float getFreshTemp() {   
  return latestReading;
}

float getLastTemp() {
  return latestReading;
}

// END Temperature Sensor
