#pragma once

#include "libs.h"

#include "modules/ir.h"
#include "modules/leds.h"
#include "modules/motors.h"
#include "modules/pid.h"

extern uint8_t pinos_ir[7];
extern int leituras_ir[7];

extern LinePID linePid;
extern SensorsIR sensorsIr;
extern MotorsControl motorsControl;
extern Leds leds;

// void calibrate(IRSensors& irSensors);