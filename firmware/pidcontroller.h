/**
 * This file is part of C8H10N4O2.
 *
 * (c) Mathias Dalheimer <md@gonium.net>, 2010
 *
 * C8H10N4O2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * C8H10N4O2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with C8H10N4O2. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H 1

#include <inttypes.h>

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

#endif /* PIDCONTROLLER_H */

