#include "config.h"

enum RobotState {
  IDLE,
  HOLDING,
  FOLLOWING
};

RobotState robotState = IDLE;
bool previousButtonState = false;

bool readButtonDebounced() {
  static bool stableState = false;
  static bool lastReading = false;
  static unsigned long lastChange = 0;

  bool reading = (digitalRead(BTN) == LOW);

  if (reading != lastReading) {
    lastChange = millis();
    lastReading = reading;
  }

  if (millis() - lastChange > DEBOUNCE_START) {
    stableState = reading;
  }

  return stableState;
}

unsigned long lastIdleBlink = 0;
bool idleYellowOn = false;
const unsigned long IDLE_BLINK_INTERVAL = 400;

void enterIdle() {
  robotState = IDLE;
  leds.off();
  idleYellowOn = true;
  leds.setYellow(true);
  lastIdleBlink = millis();
}

void updateIdle() {
  unsigned long now = millis();
  if (now - lastIdleBlink >= IDLE_BLINK_INTERVAL) {
    lastIdleBlink = now;
    idleYellowOn = !idleYellowOn;
    leds.setYellow(idleYellowOn);
  }
}

const unsigned long HOLD_STEP_MS = 1000;
const unsigned long HOLD_BLINK_INTERVAL = 500;

unsigned long holdStartTime = 0;
uint8_t holdLedStep = 0;
bool holdCompleted = false;
unsigned long lastHoldBlink = 0;
bool holdLedsOn = true;

void enterHolding() {
  robotState = HOLDING;
  holdStartTime = millis();
  holdLedStep = 0;
  holdCompleted = false;
  leds.off();
}

void updateHolding() {
  unsigned long elapsed = millis() - holdStartTime;

  if (!holdCompleted) {
    uint8_t step = elapsed / HOLD_STEP_MS;
    if (step > 3) step = 3;

    if (step != holdLedStep) {
      holdLedStep = step;
      leds.off();
      if (holdLedStep == 1)      leds.setRed(true);
      else if (holdLedStep == 2) leds.setYellow(true);
      else if (holdLedStep == 3) leds.setGreen(true);
    }

    if (elapsed >= 3 * HOLD_STEP_MS) {
      holdCompleted = true;
      lastHoldBlink = millis();
      holdLedsOn = true;
      leds.setGreen(true);
      leds.setYellow(true);
      leds.setRed(true);
    }
  } else {
    unsigned long now = millis();
    if (now - lastHoldBlink >= HOLD_BLINK_INTERVAL) {
      lastHoldBlink = now;
      holdLedsOn = !holdLedsOn;
      leds.setGreen(holdLedsOn);
      leds.setYellow(holdLedsOn);
      leds.setRed(holdLedsOn);
    }
  }
}

void startFollower() {
  leds.off();
  leds.setGreen(true);
  linePid.reset();
  robotState = FOLLOWING;
}

void stopFollower() {
  motorsControl.stop();
  enterIdle();
}

void enterCalibration() {
  motorsControl.stop();
  leds.off();
  leds.setRed(true);
  leds.setYellow(true);

  calibrate(leds, sensorsIr);
}

void setup() {
  Serial.begin(115200);

  leds.off();
  leds.setRed(true);

  leds.setYellow(true);
  pinMode(BTN, INPUT_PULLUP);

  leds.setYellow(false);
  analogReadResolution(10);

  leds.setYellow(true);
  motorsControl.begin();

  leds.setYellow(false);
  sensorsIr.begin();
  loadCalibration(sensorsIr);

  leds.setYellow(true);
  delay(100);

  leds.setRed(false);
  enterIdle();
}

void loop() {
  bool buttonPressed = readButtonDebounced();

  switch (robotState) {
    case IDLE:
      updateIdle();
      if (buttonPressed && !previousButtonState) {
        enterHolding();
      }
      break;

    case HOLDING:
      updateHolding();
      if (!buttonPressed && previousButtonState) {
        if (holdCompleted) {
          startFollower();
        } else {
          enterCalibration();
        }
      }
      break;

    case FOLLOWING:
      sensorsIr.read();
      {
        float correction = linePid.calculateCorrection();
        motorsControl.setCorrection(BASE_SPEED * correction, BASE_SPEED, true);
      }
      if (buttonPressed && !previousButtonState) {
        stopFollower();
      }
      break;
  }

  previousButtonState = buttonPressed;
}