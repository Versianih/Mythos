#ifndef IR_H
#define IR_H

#include <Arduino.h>

class SensorsIR {
public:
  explicit SensorsIR(uint8_t pins[], uint8_t led_pin, int readings[])
    : _led(led_pin), _readings(readings) {
    for (int i = 0; i < NUM_SENSORS; i++) {
      _pins[i] = pins[i];
      _ema[i]  = 0.0f;
    }
  }

  void begin() {
    for (int i = 0; i < NUM_SENSORS; i++) {
      pinMode(_pins[i], INPUT);
    }
    pinMode(_led, OUTPUT);
    digitalWrite(_led, LOW);
    preComputeScale();
  }

  void read() {
    uint16_t ir_on[NUM_SENSORS];
    uint16_t ir_off[NUM_SENSORS];

    digitalWrite(_led, HIGH);
    delayMicroseconds(LED_DELAY_ON_OFF);
    for (int i = 0; i < NUM_SENSORS; i++) {
      ir_on[i] = analogRead(_pins[i]);
    }

    digitalWrite(_led, LOW);
    delayMicroseconds(LED_DELAY_ON_OFF);
    for (int i = 0; i < NUM_SENSORS; i++) {
      ir_off[i] = analogRead(_pins[i]);
    }

    for (int i = 0; i < NUM_SENSORS; i++) {
      int32_t raw = (ir_on[i] > ir_off[i]) ? (ir_on[i] - ir_off[i]) : (ir_off[i] - ir_on[i]);

      _ema[i] = (EMA_ALPHA * raw) + ((1.0f - EMA_ALPHA) * _ema[i]);

      int32_t baseline = (_calBlack[i] < _calWhite[i]) ? _calBlack[i] : _calWhite[i];
      int32_t norm = (int32_t)((_ema[i] - baseline) * _calScale[i]);

      _readings[i] = (norm < 0) ? 0 : ((norm > 4095) ? 4095 : norm);
    }
  }

  void calibrate(bool onWhite) {
    const int N = 50;
    uint32_t acc[NUM_SENSORS] = {0};
    uint16_t ir_on[NUM_SENSORS];
    uint16_t ir_off[NUM_SENSORS];

    for (int s = 0; s < N; s++) {
      digitalWrite(_led, HIGH);
      delayMicroseconds(LED_DELAY_ON_OFF);
      for (int i = 0; i < NUM_SENSORS; i++) {
        ir_on[i] = analogRead(_pins[i]);
      }

      digitalWrite(_led, LOW);
      delayMicroseconds(LED_DELAY_ON_OFF);
      for (int i = 0; i < NUM_SENSORS; i++) {
        ir_off[i] = analogRead(_pins[i]);
        int32_t diff = (ir_on[i] > ir_off[i]) ? (ir_on[i] - ir_off[i]) : (ir_off[i] - ir_on[i]);
        acc[i] += diff;
      }
    }

    for (int i = 0; i < NUM_SENSORS; i++) {
      int val = acc[i] / N;
      if (onWhite) _calWhite[i] = val;
      else         _calBlack[i] = val;
    }

    preComputeScale();
  }

  void debugPrint() const {
    for (int i = 0; i < NUM_SENSORS; i++) {
      Serial.print(_readings[i]);
      Serial.print(' ');
    }
    Serial.println();
  }

  void getCalWhite(int out[]) const {
    for (int i = 0; i < NUM_SENSORS; i++) out[i] = _calWhite[i];
  }

  void getCalBlack(int out[]) const {
    for (int i = 0; i < NUM_SENSORS; i++) out[i] = _calBlack[i];
  }

  void setCalibration(const int white[], const int black[]) {
    for (int i = 0; i < NUM_SENSORS; i++) {
      _calWhite[i] = white[i];
      _calBlack[i] = black[i];
    }
    preComputeScale();
  }

private:
  static constexpr int   NUM_SENSORS      = NUM_SENSORS_IR;
  static constexpr int   LED_DELAY_ON_OFF = 150;
  static constexpr float EMA_ALPHA        = 0.4f;

  uint8_t _pins[NUM_SENSORS];
  uint8_t _led;
  int*    _readings;

  float _ema[NUM_SENSORS];
  float _calScale[NUM_SENSORS];

  int _calWhite[NUM_SENSORS] = {  109,  139,  187,  184,  263,  201,  113 };
  int _calBlack[NUM_SENSORS] = {    0,    0,    0,    0,    0,    0,    0 };

  void preComputeScale() {
    for (int i = 0; i < NUM_SENSORS; i++) {
      int32_t range = _calWhite[i] > _calBlack[i] ? (_calWhite[i] - _calBlack[i]) : (_calBlack[i] - _calWhite[i]);
      _calScale[i] = (range > 0) ? (4095.0f / (float)range) : 0.0f;
    }
  }
};

#endif