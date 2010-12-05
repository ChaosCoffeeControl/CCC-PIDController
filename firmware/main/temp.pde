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


#define TEMP_SENSOR_PIN 1

float tcSum = 0.0;
float latestReading = 0.0;
int readCount = 0;
float multiplier;
void setupTempSensor() {
  multiplier = 1.0/(1023.0) * 500.0 * 9.0 / 5.0;
}  

void updateTempSensor() {
    tcSum += analogRead(TEMP_SENSOR_PIN); //output from AD595 to analog pin 1
    readCount +=1;
}

float getFreshTemp() { 
      latestReading = tcSum* multiplier/readCount+32.0;
      readCount = 0;
      tcSum = 0.0;
  return latestReading;

}

float getLastTemp() {
  return latestReading;

}

// END Temperature Sensor
