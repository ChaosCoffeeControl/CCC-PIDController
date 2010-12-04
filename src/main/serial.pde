//serialInterface
// Tim Hirzel February 2008
// This is a very basic serial interface for controlling the PID loop.
// thanks to the Serial exampe code  

// All code released under
// Creative Commons Attribution-Noncommercial-Share Alike 3.0 

#define AUTO_PRINT_INTERVAL 200  // milliseconds
#define MAX_DELTA  100
#define MIN_DELTA  0.01
#define PRINT_PLACES_AFTER_DECIMAL 2  // set to match MIN_DELTA


int incomingByte = 0;
float delta = 1.0;
boolean autoupdate;
boolean printmode = 0;

unsigned long lastUpdateTime = 0;
void setupSerialInterface()  {
  Serial.begin(115200);
  Serial.println("\nWelcome to the BBCC, the Bare Bones Coffee Controller for Arduino");
  Serial.println("Send back one or more characters to setup the controller.");
  Serial.println("If this is your initial run, please enter 'R' to Reset the EEPROM.");
  Serial.println("Enter '?' for help.  Here's to a great cup!");
}

void printHelp() {
  Serial.println("Send these characters for control:");
  Serial.println("<space> : print status now");
  Serial.println("u : toggle periodic status update");
  Serial.println("g : toggle update style between human and graphing mode");
  Serial.println("R : reset/initialize PID gain values");
  Serial.println("b : print PID debug values");
  Serial.println("? : print help");  
  Serial.println("+/- : adjust delta by a factor of ten");
  Serial.println("P/p : up/down adjust p gain by delta");
  Serial.println("I/i : up/down adjust i gain by delta");
  Serial.println("D/d : up/down adjust d gain by delta");
  Serial.println("T/t : up/down adjust set temp by delta");


}

void updateSerialInterface() {
  while(Serial.available()){

    incomingByte = Serial.read();
    if (incomingByte == 'R') {
      setP(30.0); // make sure to keep the decimal point on these values
      setI(0.0);  // make sure to keep the decimal point on these values
      setD(0.0);  // make sure to keep the decimal point on these values
      setTargetTemp(200.0); // here too
    } 
    if (incomingByte == 'P') {
      setP(getP() + delta);
    } 
    if (incomingByte == 'p') {
      setP(getP() - delta);
    } 
    if (incomingByte == 'I') {
      setI(getI() + delta);
    } 
    if (incomingByte == 'i') {
      setI(getI() - delta);
    } 
    if (incomingByte == 'D') {
      setD(getD() + delta);
    } 
    if (incomingByte == 'd' ){
      setD(getD() - delta);
    } 
    if (incomingByte == 'T') {
      setTargetTemp(getTargetTemp() + delta);
    } 
    if (incomingByte == 't') {
      setTargetTemp(getTargetTemp() - delta);
    }
    if (incomingByte == '+') {
      delta *= 10.0;
      if (delta > MAX_DELTA)
        delta = MAX_DELTA;
    } 
    if (incomingByte == '-') {
      delta /= 10.0;
      if (delta < MIN_DELTA)
        delta = MIN_DELTA;

    }
    if (incomingByte == 'u') {
      // toggle updating

      autoupdate = not autoupdate;
    }
    if (incomingByte == 'g') {
      // toggle updating

      printmode = not printmode;
    }
    if (incomingByte == ' ') {
      // toggle updating

      printStatus();
    }
    if (incomingByte == '?') {
      printHelp(); 
    }
    if (incomingByte == 'b') {
      printPIDDebugString(); 
      Serial.println();
    }
  }

  if (millis() < lastUpdateTime) {
    lastUpdateTime = 0;
  }
  if ((millis() - lastUpdateTime) > AUTO_PRINT_INTERVAL) {
    // this is triggers every slightly more than a second from the delay between these two millis() calls
    lastUpdateTime += AUTO_PRINT_INTERVAL;
    if (autoupdate) {
      if (printmode) {
        printStatusForGraph();
      }
      else {
        printStatus();
      }
    } 
  }
}

void printStatus() { 
  // A means for getting feedback on the current system status and controllable parameters
  Serial.print(" SET TEMP:");
  printFloat(getTargetTemp(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", CUR TEMP:");
  printFloat(getLastTemp(),PRINT_PLACES_AFTER_DECIMAL);

  Serial.print(", GAINS p:");
  printFloat(getP(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(" i:");
  printFloat(getI(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(" d:");
  printFloat(getD(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", Delta: ");
  printFloat(delta,PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", Power: ");
  printFloat((float)getHeatCycles(), 0);

  Serial.print(" \n");
}

void printStatusForGraph() {
  printFloat(getTargetTemp(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", ");
  printFloat(getLastTemp(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", ");
  printFloat(getP(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", ");
  printFloat(getI(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", ");
  printFloat(getD(),PRINT_PLACES_AFTER_DECIMAL);
  Serial.print(", ");
  printFloat((float)getHeatCycles(), 0);
  Serial.println();
}

// printFloat prints out the float 'value' rounded to 'places' places after the decimal point
void printFloat(float value, int places) {
  // this is used to cast digits 
  int digit;
  float tens = 0.1;
  int tenscount = 0;
  int i;
  float tempfloat = value;

  // make sure we round properly. this could use pow from <math.h>, but doesn't seem worth the import
  // if this rounding step isn't here, the value  54.321 prints as 54.3209

  // calculate rounding term d:   0.5/pow(10,places)  
  float d = 0.5;
  if (value < 0)
    d *= -1.0;
  // divide by ten for each decimal place
  for (i = 0; i < places; i++)
    d/= 10.0;    
  // this small addition, combined with truncation will round our values properly 
  tempfloat +=  d;

  // first get value tens to be the large power of ten less than value
  // tenscount isn't necessary but it would be useful if you wanted to know after this how many chars the number will take

  if (value < 0)
    tempfloat *= -1.0;
  while ((tens * 10.0) <= tempfloat) {
    tens *= 10.0;
    tenscount += 1;
  }


  // write out the negative if needed
  if (value < 0)
    Serial.print('-');

  if (tenscount == 0)
    Serial.print(0, DEC);

  for (i=0; i< tenscount; i++) {
    digit = (int) (tempfloat/tens);
    Serial.print(digit, DEC);
    tempfloat = tempfloat - ((float)digit * tens);
    tens /= 10.0;
  }

  // if no places after decimal, stop now and return
  if (places <= 0)
    return;

  // otherwise, write the point and continue on
  Serial.print('.');  

  // now write out each decimal place by shifting digits one by one into the ones place and writing the truncated value
  for (i = 0; i < places; i++) {
    tempfloat *= 10.0; 
    digit = (int) tempfloat;
    Serial.print(digit,DEC);  
    // once written, subtract off that digit
    tempfloat = tempfloat - (float) digit; 
  }
}

// END Serial Interface
