#pragma once

#include "libs.h"

class Motors {
    public:
        Motors() {}

        void begin() {
            pinMode(PIN_AIN1, OUTPUT);
            pinMode(PIN_AIN2, OUTPUT);
            pinMode(PIN_PWMA, OUTPUT);

            pinMode(PIN_BIN1, OUTPUT);
            pinMode(PIN_BIN2, OUTPUT);
            pinMode(PIN_PWMB, OUTPUT);

            stop();
        }

        void drive(int speedA, int speedB) {
            driveMotor(PIN_AIN1, PIN_AIN2, PIN_PWMA, speedA);
            driveMotor(PIN_BIN1, PIN_BIN2, PIN_PWMB, speedB);
        }

        void stop() {
            drive(0, 0);
        }

        void brake() {
            brakeMotor(PIN_AIN1, PIN_AIN2, PIN_PWMA);
            brakeMotor(PIN_BIN1, PIN_BIN2, PIN_PWMB);
        }

    private:
        void driveMotor(uint8_t in1, uint8_t in2, uint8_t pwm, int speed) {
            speed = constrain(speed, -255, 255);

            if (speed > 0) {
                digitalWrite(in1, HIGH);
                digitalWrite(in2, LOW);
            } else if (speed < 0) {
                digitalWrite(in1, LOW);
                digitalWrite(in2, HIGH);
            } else {
                digitalWrite(in1, LOW);
                digitalWrite(in2, LOW);
            }

            analogWrite(pwm, abs(speed));
        }

        void brakeMotor(uint8_t in1, uint8_t in2, uint8_t pwm) {
            digitalWrite(in1, HIGH);
            digitalWrite(in2, HIGH);
            analogWrite(pwm, 0);
        }
};