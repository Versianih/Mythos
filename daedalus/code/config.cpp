#include "config.h"

uint8_t pinos_ir[7] = { ESQ3_IR, ESQ2_IR, ESQ1_IR, CEN_IR, DIR1_IR, DIR2_IR, DIR3_IR };
int leituras_ir[7];

LinePID linePid(leituras_ir);
SensorsIR sensorsIr(pinos_ir, LED_IR, leituras_ir);
Leds leds(LED_VERDE, LED_VERMELHO, LED_AMARELO, LED_AZUL);

MotorsControl motorsControl;
Motor motorA("Right", PWMA, AIN1, AIN2, 0);
Motor motorB("Left", PWMB, BIN1, BIN2, 1);