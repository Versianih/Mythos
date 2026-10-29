#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

class Motor {
private:
  const char* name;
  int pwm_pin, in1_pin, in2_pin;
  int pwm_channel;
  int current_pwm;

  float target_speed;
  float current_speed;

  void applyOutput(float speed) {
    if (speed == 0.0f) {
      digitalWrite(in1_pin, LOW);
      digitalWrite(in2_pin, LOW);
      ledcWrite(pwm_pin, 0);
      current_pwm = 0;
      return;
    }

    if (speed > 0) {
      digitalWrite(in1_pin, HIGH);
      digitalWrite(in2_pin, LOW);
    } else {
      digitalWrite(in1_pin, LOW);
      digitalWrite(in2_pin, HIGH);
    }

    int pwm_value = (int)(fabsf(speed) * PWM_MAX / 100.0f);
    ledcWrite(pwm_pin, pwm_value);
    current_pwm = pwm_value;
  }

public:
  Motor(const char* motorName, int pwm, int in1, int in2, int channel)
      : name(motorName), pwm_pin(pwm), in1_pin(in1), in2_pin(in2),
        pwm_channel(channel), current_pwm(0),
        target_speed(0.0f), current_speed(0.0f) {}

  void begin() {
    pinMode(in1_pin, OUTPUT);
    pinMode(in2_pin, OUTPUT);
    ledcAttachChannel(pwm_pin, PWM_FREQ, PWM_RESOLUTION, pwm_channel);
  }

  void setTarget(float speed) {
    target_speed = constrain(speed, -100.0f, 100.0f);
  }

  void update(float dtMs) {
    if (current_speed == target_speed) return;

    float diff = target_speed - current_speed;

    bool reducing = (current_speed > 0 && diff < 0) || (current_speed < 0 && diff > 0);
    float step = (reducing ? MOTOR_SLEW_DOWN : MOTOR_SLEW_UP) * dtMs;

    float next;
    if (fabsf(diff) <= step) next = target_speed;
    else                     next = current_speed + (diff > 0 ? step : -step);

    if (next != 0.0f && fabsf(next) < MOTOR_MIN_SPEED) {
      if (fabsf(target_speed) < MOTOR_MIN_SPEED) next = 0.0f;
      else next = (target_speed > 0) ? MOTOR_MIN_SPEED : -MOTOR_MIN_SPEED;
    }

    if (next == current_speed) return;
    current_speed = next;
    applyOutput(current_speed);
  }

  void stop() {
    target_speed = 0.0f;
    current_speed = 0.0f;
    applyOutput(0.0f);
  }

  void release() { target_speed = 0.0f; }

  float getCurrentSpeed() const { return current_speed; }
};

extern Motor motorA;
extern Motor motorB;

class MotorsControl {
private:
  Motor *rightMotor;
  Motor *leftMotor;
  unsigned long lastUpdateUs;

public:
  MotorsControl() : rightMotor(&motorA), leftMotor(&motorB), lastUpdateUs(0) {}

  void begin() {
    rightMotor->begin();
    leftMotor->begin();
    lastUpdateUs = micros();
  }

  void setCorrection(
    float correction, 
    float speed = BASE_SPEED, 
    bool useRotationMode = false, 
    bool forceRotation = false
    ) {
    float rotationLimiter = speed * ROTATION_THRESHOLD;

    if ((fabsf(correction) >= rotationLimiter && useRotationMode) || forceRotation) {
      // Modo de Giro
      float speedRotation = speed * ROTATION_RATIO * MOTOR_CONNECTION;
      if (correction > 0) {
        rightMotor->setTarget(-speedRotation);
        leftMotor->setTarget(speedRotation);
      } else if (correction < 0) {
        rightMotor->setTarget(speedRotation);
        leftMotor->setTarget(-speedRotation);
      }
    } else {
      float rightSpeed = constrain((speed - correction) * MOTOR_CONNECTION, -BASE_SPEED, BASE_SPEED);
      float leftSpeed = constrain((speed + correction) * MOTOR_CONNECTION, -BASE_SPEED, BASE_SPEED);
      rightMotor->setTarget(rightSpeed);
      leftMotor->setTarget(leftSpeed);
    }
  }

  void update() {
    unsigned long now = micros();
    float dtMs = (now - lastUpdateUs) / 1000.0f;
    lastUpdateUs = now;
    if (dtMs > MOTOR_MAX_DT_MS) dtMs = MOTOR_MAX_DT_MS;

    rightMotor->update(dtMs);
    leftMotor->update(dtMs);
  }

  void stop() {
    rightMotor->stop();
    leftMotor->stop();
  }

  void release() {
    rightMotor->release();
    leftMotor->release();
  }

  Motor *getRightMotor()  { return rightMotor; }
  Motor *getLeftMotor() { return leftMotor; }
};

#endif