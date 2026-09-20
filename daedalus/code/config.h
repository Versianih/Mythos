#ifndef CONFIG_H
#define CONFIG_H

#include "libs.h"

#include "modules/ir.h"
#include "modules/leds.h"
#include "modules/motors.h"
#include "modules/pid.h"

extern uint8_t irPins[7];
extern int irReadings[7];

extern LinePID linePid;
extern SensorsIR sensorsIr;
extern MotorsControl motorsControl;
extern Leds leds;

#endif