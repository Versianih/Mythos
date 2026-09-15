#pragma once

#include <Arduino.h>
#include "pins.h"

#define NUM_SENSORS_IR 7

#include "modules/leds.h"
#include "modules/ir.h"


// ==== LinePID ====
#define KP_LINE 3.0f
#define KI_LINE 0.0f
#define KD_LINE 0.3f

#define MAX_INTEGRAL 10.0f

#define ROTATION_THRESHOLD 0.8
#define ROTATION_RATIO 0.7
// ==== ======== ====

// ==== Motors ====
#define PWM_FREQ 7000
#define PWM_RESOLUTION 12
#define PWM_MAX 4000

#define BASE_SPEED 90.0
#define MOTOR_CONNECTION 1  // (Invertido = -1; Normal = 1)
// ==== ====== ====

// ==== Debounce ====
#define DEBOUNCE_START 200
#define DEBOUNCE_CALIBRATION 200
// ==== ======== ====

bool loadCalibration(SensorsIR& sensorsIr);
void calibrate(Leds& leds, SensorsIR& sensorsIr);