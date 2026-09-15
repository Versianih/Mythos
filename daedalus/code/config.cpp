#include "config.h"

uint8_t irPins[7] = { ESQ3_IR, ESQ2_IR, ESQ1_IR, CEN_IR, DIR1_IR, DIR2_IR, DIR3_IR };
int irReadings[7];

LinePID linePid(irReadings);
SensorsIR sensorsIr(irPins, LED_IR, irReadings);
Leds leds(LED_VERDE, LED_VERMELHO, LED_AMARELO);

MotorsControl motorsControl;
Motor motorA("Right", PWMA, AIN1, AIN2, 0);
Motor motorB("Left", PWMB, BIN1, BIN2, 1);