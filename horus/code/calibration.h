#ifndef CALIBRATION_H
#define CALIBRATION_H

// ==== MOTORES ====
#define MOTOR_CONNECTION -1  // (Invertido = -1; Normal = 1)
#define TURN_DIRECTION 1

#define PWM_FREQ 7000
#define PWM_RESOLUTION 8
#define PWM_MAX ((1 << PWM_RESOLUTION) - 1)

#define NORMAL_SPEED 75
#define TURBO_SPEED 100
#define BUTTON_TURN_SPEED 100
// ==== ======= ====


// ==== BLE ====
#define BLE_DEVICE_NAME "Mythos: HORUS"
#define BLE_TX_POWER ESP_PWR_LVL_P9

#define STICK_DEADZONE 8

#define WATCHDOG_TIMEOUT_MS 300

// #define DEBUG_LOG_MS 250  // Comentar para silenciar o serial
// ==== === ====

#endif