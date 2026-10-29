#include "config.h"

enum RobotState {
  IDLE,
  HOLDING,
  FOLLOWING
};

RobotState robotState = IDLE;
bool previousButtonState = false;
unsigned long lastSensorDebug = 0;
unsigned long lineLostSince = 0;
bool lineLost = false;
float lastLineDirection = 0.0f;
bool hasSearchDirection = false;
float correction;

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

bool shouldPrintSensorDebug() {
  unsigned long now = millis();
  if (now - lastSensorDebug < SENSOR_DEBUG_INTERVAL_MS) {
    return false;
  }

  lastSensorDebug = now;
  return true;
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
  lineLost = false;
  lastLineDirection = 0.0f;
  hasSearchDirection = false;
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
      if (shouldPrintSensorDebug()) {
        sensorsIr.read();
        sensorsIr.debugPrint();
      }
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
        if (linePid.calculateCorrection(correction)) {
          lineLost = false;
          float currentError = linePid.getCurrentError();
          if (currentError >= LINE_SEARCH_DIRECTION_THRESHOLD ||
              currentError <= -LINE_SEARCH_DIRECTION_THRESHOLD) {
            lastLineDirection = currentError;
            hasSearchDirection = true;
          }
          float followerSpeed = BASE_SPEED;
          float turnAmount = currentError >= 0.0f ? currentError : -currentError;
          if (turnAmount > SHARP_TURN_ERROR) {
            float turnRange = 1.0f - SHARP_TURN_ERROR;
            float turnProgress = (turnAmount - SHARP_TURN_ERROR) / turnRange;
            turnProgress = constrain(turnProgress, 0.0f, 1.0f);
            float speedFactor = 1.0f - ((1.0f - SHARP_TURN_SPEED_FACTOR) * turnProgress);
            followerSpeed *= speedFactor;
          }
          motorsControl.setCorrection(followerSpeed * correction, followerSpeed, true);
        } else {
          if (!lineLost) {
            lineLost = true;
            lineLostSince = millis();
          }

          if (millis() - lineLostSince >= LINE_LOST_CONFIRM_MS) {
            if (hasSearchDirection) {
              float searchSpeed = BASE_SPEED * LINE_SEARCH_SPEED_FACTOR;
              motorsControl.setCorrection(lastLineDirection, searchSpeed, true, true);
            } else {
              motorsControl.release();
            }
          }
        }
        motorsControl.update();
      }
      if (shouldPrintSensorDebug()) {
        sensorsIr.debugPrint();
      }
      if (buttonPressed && !previousButtonState) {
        stopFollower();
      }
      break;
  }

  previousButtonState = buttonPressed;
}