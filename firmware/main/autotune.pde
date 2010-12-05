// Auto-tuning functions. The normal controller operation is suspended
// while the autotuning is in progress.


float setpoint=20.0;
float oldTemp=0.0;
float last_t_u, last_k_u = 0.0;

void _setupAutoTuning() {
  setpoint = getTargetTemp();
  updateTempSensor();
  oldTemp = getFreshTemp();
  // initialize heater process.
  if (oldTemp < setpoint)
    _turnHeatElementOnOff(1);
  else
    _turnHeatElementOnOff(0);
}

// the process is considered steady if it changes less than 3 percent.
bool _is_steady(unsigned long t_u, float k_u) {
  Serial.println("Checking: Are we steady?");
  unsigned long delta_t = abs(t_u - last_t_u);
  float delta_k = abs(k_u - last_k_u);
  last_t_u = t_u;
  last_k_u = k_u;
  if ((delta_t < (0.03 * t_u)) && (delta_k < (0.03 * k_u)))
    return true;
  else
    return false;
}

float _calc_ku(float min_temp, float max_temp) {
  return (4/(3.141596*(max_temp - min_temp))); 
}

unsigned long getTime() {
  return (millis() / 1000);
}  

void printPessenPIDParameters(unsigned long t_u, float k_u) {
  Serial.print("P = ");
  Serial.print(0.7 * k_u);
  Serial.print(", I = ");
  Serial.print(0.4 * t_u);
  Serial.print(", D = ");
  Serial.print(0.15 * t_u);  
}

void runAutoTuning() {
  bool steady_state=false;
  float currentTemp;
  float min_temp=1000;
  float max_temp=0;
  unsigned long t_u = 0;
  float k_u = 0.0;
  unsigned long starttime=getTime();
  Serial.println("Starting autotuning process. Normal operation is suspended. Please wait.");
  _setupAutoTuning();
  while (not steady_state) {
    updateTempSensor();
    currentTemp=getFreshTemp();
    // record min and max temps
      min_temp = min(min_temp, currentTemp);
      max_temp = max(max_temp, currentTemp);
    // check if we pass the setpoint
    if ((oldTemp < setpoint) && (currentTemp >= setpoint)) {
      Serial.println("Reached setpoint - turning heater off & stopping time.");
      _turnHeatElementOnOff(0);
      // now, calculate t_u and k_u
      unsigned long endtime=getTime();
      t_u=endtime - starttime;
      k_u = _calc_ku(min_temp, max_temp);
      steady_state = _is_steady(t_u, k_u);
      // reset runtime vars for next loop, if needed
      starttime=endtime;
      min_temp = 1000; max_temp=0;
    } 
    if ((oldTemp > setpoint) && (currentTemp <= setpoint)) {
      Serial.println("Reached setpoint - turning heater on.");
      _turnHeatElementOnOff(1);
    } 
    oldTemp=currentTemp;
    // sleep a second
    delay(1000);
  }
  Serial.println("We have reached a steady state with:");
  Serial.print("t_u=");
  Serial.print(t_u);
  Serial.print("k_u=");
  Serial.println(k_u);
  printPessenPIDParameters(t_u, k_u);
  Serial.println("Resuming normal operation.");
}
