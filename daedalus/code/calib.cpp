#include "calib.h"

void waitBtnPress(Leds leds, const char* message) {
  Serial.println(message);
  leds.blue();
  while (digitalRead(BTN) == HIGH) { delay(10); }
  delay(DEBOUNCE_CALIBRATION);
  while (digitalRead(BTN) == LOW) { delay(10); }
  delay(DEBOUNCE_CALIBRATION);
  leds.blue();
  delay(500);
}

void calibrate(Leds& leds, SensorsIR& sensorsIr) {
  struct Step {
    const char* msg;
    bool irWhite;
  };

  static constexpr Step STEPS[] = {
    { "PRETO",    false },
    { "BRANCO",   true  },
  };

  Serial.println();

  for (const Step& step : STEPS) {
    Serial.printf("\nPosicione o robô sobre o %s.\n", step.msg);
    waitBtnPress(leds, "Pressione o botão para capturar.");

    sensorsIr.calibrate(step.irWhite);

    Serial.printf("%s capturado.\n", step.msg);
  }

  int calWhite[NUM_SENSORS_IR], calBlack[NUM_SENSORS_IR];
  sensorsIr.getCalWhite(calWhite);
  sensorsIr.getCalBlack(calBlack);

  Serial.println("\n -- Resultados ----------------------");
  Serial.printf("int _calWhite[NUM_SENSORS] = { %4d, %4d, %4d, %4d, %4d, %4d, %4d };\n",
                calWhite[0], calWhite[1], calWhite[2], calWhite[3], calWhite[4], calWhite[5], calWhite[6]);
  Serial.printf("int _calBlack[NUM_SENSORS] = { %4d, %4d, %4d, %4d, %4d, %4d, %4d };\n",
                calBlack[0], calBlack[1], calBlack[2], calBlack[3], calBlack[4], calBlack[5], calBlack[6]);
}