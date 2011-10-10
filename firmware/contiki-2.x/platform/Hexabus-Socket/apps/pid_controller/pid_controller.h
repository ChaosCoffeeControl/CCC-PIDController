/*
 * pid_controller.h
 *
 *  Created on: 20.04.2011
 *      Author: Mathias Dalheimer
 */

#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H 1



#include <stdint.h>
#include <stdbool.h>
#include "process.h"

#define PRINTF(...) printf(__VA_ARGS__)

PROCESS_NAME(pid_controller_process);

extern void initPIDController(void);
extern void loopPIDController(void);
extern void autotunePID(void);
extern void restorePIDDefault(void);
extern void savePIDConfig(void);
extern float getPIDSetpoint(void);
extern void setPIDSetpoint(float);
extern void setPID_P(float);
extern void setPID_I(float);
extern void setPID_D(float);
extern float getPID_P(void);
extern float getPID_I(void);
extern float getPID_D(void);
extern void togglePIDDebug(void);
extern int16_t get_duty_cycle(void);


#endif /* PID_CONTROLLER_H */
