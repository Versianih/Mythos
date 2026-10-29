#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>
#include "pins.h"

#define NUM_SENSORS_IR 7

#define IR_SETTLE_TIME_US 100
#define IR_EMA_ALPHA 0.4f
#define SENSOR_DEBUG_INTERVAL_MS 100UL

static const int IR_DEFAULT_WHITE[NUM_SENSORS_IR] = { 4095, 4095, 4095, 4095, 4095, 4095, 4095 };
static const int IR_DEFAULT_BLACK[NUM_SENSORS_IR] = { 0, 0, 0, 0, 0, 0, 0 };

#include "modules/leds.h"
#include "modules/ir.h"


// ==== LinePID ====
#define KP_LINE 1.3f
#define KI_LINE 0.001f
#define KD_LINE 0.1f

#define LINE_MIN_ACTIVE_SUM 2000
#define LINE_LOST_CONFIRM_MS 30UL
#define LINE_SEARCH_SPEED_FACTOR 0.6f
#define LINE_SEARCH_DIRECTION_THRESHOLD 0.2f
#define SHARP_TURN_ERROR 0.5f
#define SHARP_TURN_SPEED_FACTOR 0.7f

#define MAX_INTEGRAL 10.0f

#define ROTATION_THRESHOLD 0.7
#define ROTATION_RATIO 0.7
// ==== ======== ====

// ==== Motors ====
#define PWM_FREQ 7000
#define PWM_RESOLUTION 12
#define PWM_MAX 4000

#define BASE_SPEED 45.0
#define MOTOR_CONNECTION 1  // (Invertido = -1; Normal = 1)

#define MOTOR_SLEW_UP      0.40f
#define MOTOR_SLEW_DOWN    0.80f
#define MOTOR_MIN_SPEED    10.0f

#define MOTOR_MAX_DT_MS    5.0f
// ==== ====== ====

// ==== Debounce ====
#define DEBOUNCE_START 200
#define DEBOUNCE_CALIBRATION 200
// ==== ======== ====

bool loadCalibration(SensorsIR& sensorsIr);
void calibrate(Leds& leds, SensorsIR& sensorsIr);

#endif
