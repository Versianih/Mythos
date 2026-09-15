#pragma once

#include <Arduino.h>

class LinePID {
public:
  explicit LinePID(int* arrayReadings)
    : readings(arrayReadings),
      kp(KP_LINE), ki(KI_LINE), kd(KD_LINE),
      previousError(0.0f), integral(0.0f) {

    float offset = (NUM_SENSORS_IR - 1) / 2.0f;
    for (int i = 0; i < NUM_SENSORS_IR; i++) {
      weights[i] = i - offset;
    }
  }

  void setPidGains(float kpValue, float kiValue, float kdValue) {
    kp = kpValue; ki = kiValue; kd = kdValue;
  }

  float calculateError() {
    float weightedSum = 0.0f;
    float activeSum = 0.0f;

    for (int i = 0; i < NUM_SENSORS_IR; i++) {
      float value = 4095.0f - readings[i];
      weightedSum += value * weights[i];
      activeSum += value;
    }

    if (activeSum < MIN_ACTIVE_SUM) {
      return 0.0f;
    }

    float maxWeight = (NUM_SENSORS_IR - 1) / 2.0f;
    float error = weightedSum / (activeSum * maxWeight);
    return constrain(error, -1.0f, 1.0f);
  }

  float calculateCorrection() {
    float error = calculateError();

    integral += error;
    integral = constrain(integral, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);

    float derivative = error - previousError;
    previousError = error;

    float output = (kp * error) + (ki * integral) + (kd * derivative);
    return constrain(output, -1.0f, 1.0f);
  }

  void reset() {
    previousError = 0.0f;
    integral = 0.0f;
  }

  float getCurrentError() const { return previousError; }
  float getIntegral() const { return integral; }

private:
  int* readings;
  float kp, ki, kd;
  float previousError;
  float integral;
  float weights[NUM_SENSORS_IR];

  static constexpr float MIN_ACTIVE_SUM = 2000.0f;
  static constexpr float INTEGRAL_LIMIT = MAX_INTEGRAL;
};