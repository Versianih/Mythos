#ifndef PID_H
#define PID_H

#include <Arduino.h>

class LinePID {
public:
  explicit LinePID(int* arrayReadings)
    : readings(arrayReadings),
      kp(KP_LINE), ki(KI_LINE), kd(KD_LINE),
      previousError(0.0f), integral(0.0f) {

    int center = (NUM_SENSORS_IR - 1) / 2;
    for (int i = 0; i < NUM_SENSORS_IR; i++) {
      weights[i] = i - center;
    }
  }

  void setPidGains(float kpValue, float kiValue, float kdValue) {
    kp = kpValue; ki = kiValue; kd = kdValue;
  }

  bool calculateCorrection(float& correction) {
    int32_t weightedSum = 0;
    int32_t activeSum = 0;
    for (int i = 0; i < NUM_SENSORS_IR; i++) {
      int32_t value = 4095 - readings[i];
      weightedSum += value * weights[i];
      activeSum += value;
    }

    if (activeSum < LINE_MIN_ACTIVE_SUM) {
      return false;
    }

    float maxWeight = (NUM_SENSORS_IR - 1) / 2.0f;
    float error = weightedSum / (activeSum * maxWeight);

    integral += error;
    integral = constrain(integral, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);

    float derivative = error - previousError;
    previousError = error;

    float output = (kp * error) + (ki * integral) + (kd * derivative);
    correction = constrain(output, -1.0f, 1.0f);
    return true;
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
  int8_t weights[NUM_SENSORS_IR];

  static constexpr float INTEGRAL_LIMIT = MAX_INTEGRAL;
};

#endif