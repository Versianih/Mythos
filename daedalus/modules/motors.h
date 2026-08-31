#pragma once

#include <Arduino.h>

class Motor {
private:
  const char* name;
  int pwm_pin, in1_pin, in2_pin;
  int pwm_channel;
  int current_pwm;

public:
  Motor(const char* motorName, int pwm, int in1, int in2, int channel)
      : name(motorName), pwm_pin(pwm), in1_pin(in1), in2_pin(in2),
        pwm_channel(channel), current_pwm(0) {}

  void begin() {
    pinMode(in1_pin, OUTPUT);
    pinMode(in2_pin, OUTPUT);
    ledcAttachChannel(pwm_pin, PWM_FREQ, PWM_RESOLUTION, pwm_channel);
  }

  void setSpeed(int speed) {
    speed = constrain(speed, -100, 100);

    if (speed == 0) {
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

    int pwm_value = map(abs(speed), 0, 100, 0, PWM_MAX);
    ledcWrite(pwm_pin, pwm_value);
    current_pwm = pwm_value;
  }
  
  void stop() { setSpeed(0); }
};

extern Motor motorA;
extern Motor motorB;

class MotorsControl {
private:
  Motor *rightMotor;
  Motor *leftMotor;

public:
  MotorsControl() : rightMotor(&motorA), leftMotor(&motorB) {}

  void begin() {
    rightMotor->begin();
    leftMotor->begin();
  }

  void setCorrection(
    float correction, 
    float speed = BASE_SPEED, 
    bool useRotationMode = false, 
    bool forceRotation = false
    ) {
    float rotationLimiter = speed * ROTATION_THRESHOLD;

    if ((abs(correction) >= rotationLimiter && useRotationMode) || forceRotation) {
      // Modo de Giro
      float speedRotation = speed * ROTATION_RATIO * MOTOR_CONNECTION;
      if (correction > 0) {
        rightMotor->setSpeed(-speedRotation);
        leftMotor->setSpeed(speedRotation);
      } else if (correction < 0) {
        rightMotor->setSpeed(speedRotation);
        leftMotor->setSpeed(-speedRotation);
      }
    } else {
      float rightSpeed = constrain((speed - correction) * MOTOR_CONNECTION, -BASE_SPEED, BASE_SPEED);
      float leftSpeed = constrain((speed + correction) * MOTOR_CONNECTION, -BASE_SPEED, BASE_SPEED);
      rightMotor->setSpeed(rightSpeed);
      leftMotor->setSpeed(leftSpeed);
    }
  }

  void stop() {
    rightMotor->stop();
    leftMotor->stop();
  }

  Motor *getRightMotor()  { return rightMotor; }
  Motor *getLeftMotor() { return leftMotor; }
};