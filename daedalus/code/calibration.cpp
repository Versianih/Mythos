#include "calibration.h"
#include <Preferences.h>

static void waitButtonPress() {
  while (digitalRead(BTN) == HIGH) { delay(10); }
  delay(DEBOUNCE_CALIBRATION);
  while (digitalRead(BTN) == LOW) { delay(10); }
  delay(DEBOUNCE_CALIBRATION);
}

static void blinkAll(Leds &leds, uint8_t times) {
  for (uint8_t i = 0; i < times; i++) {
    leds.setGreen(true);
    leds.setYellow(true);
    leds.setRed(true);
    delay(200);
    leds.off();
    delay(200);
  }
}

static void saveCalibration(SensorsIR &sensorsIr) {
  int calWhite[NUM_SENSORS_IR], calBlack[NUM_SENSORS_IR];
  sensorsIr.getCalWhite(calWhite);
  sensorsIr.getCalBlack(calBlack);

  Preferences prefs;
  prefs.begin("daedalus", false);
  prefs.putBytes("calWhite", calWhite, sizeof(calWhite));
  prefs.putBytes("calBlack", calBlack, sizeof(calBlack));
  prefs.end();
}

bool loadCalibration(SensorsIR &sensorsIr) {
  Preferences prefs;
  prefs.begin("daedalus", true);

  int calWhite[NUM_SENSORS_IR], calBlack[NUM_SENSORS_IR];
  bool hasWhite = prefs.getBytesLength("calWhite") == sizeof(calWhite);
  bool hasBlack = prefs.getBytesLength("calBlack") == sizeof(calBlack);

  if (hasWhite && hasBlack) {
    prefs.getBytes("calWhite", calWhite, sizeof(calWhite));
    prefs.getBytes("calBlack", calBlack, sizeof(calBlack));
    sensorsIr.setCalibration(calWhite, calBlack);
  }

  prefs.end();
  return hasWhite && hasBlack;
}

void calibrate(Leds& leds, SensorsIR& sensorsIr) {
  struct Step {
    bool irWhite;
  };

  static constexpr Step STEPS[] = {
    { false },
    { true  },
  };

  for (const Step& step : STEPS) {
    waitButtonPress();

    leds.setYellow(false);
    sensorsIr.calibrate(step.irWhite);
    leds.setYellow(true);
  }

  saveCalibration(sensorsIr);

  blinkAll(leds, 3);

  delay(200);
  ESP.restart();
}