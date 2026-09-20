#ifndef CALIBRATION_H
#define CALIBRATION_H

// ==== MOTORES ====
#define MOTOR_CONNECTION 1  // (Invertido = -1; Normal = 1)

#define PWM_FREQ 7000
#define PWM_RESOLUTION 8
#define PWM_MAX ((1 << PWM_RESOLUTION) - 1)

#define NORMAL_SPEED 75
#define TURBO_SPEED 100
#define BUTTON_TURN_SPEED 100
// ==== ======= ====


// ==== JOYSTICK ====
#define LX_CENTER   4
#define LX_MIN   -508
#define LX_MAX    512
#define LY_CENTER   4
#define LY_MIN   -508
#define LY_MAX    512
#define RX_CENTER   4
#define RX_MIN   -508
#define RX_MAX    512
#define RY_CENTER   4
#define RY_MIN   -508
#define RY_MAX    512

#define AXIS_DEADZONE  40
// ==== ======== ====

#endif