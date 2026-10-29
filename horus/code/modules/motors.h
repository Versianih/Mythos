#ifndef MOTORS_H
#define MOTORS_H

#include "../libs.h"

#ifndef ESP_ARDUINO_VERSION_MAJOR
#define ESP_ARDUINO_VERSION_MAJOR 2
#endif

class Motors {
    public:
        Motors(
            uint8_t leftIn1, uint8_t leftIn2, uint8_t leftPwm,
            uint8_t rightIn1, uint8_t rightIn2, uint8_t rightPwm,
            uint8_t leftChannel, uint8_t rightChannel,
            uint32_t pwmFrequency, uint8_t pwmResolution, uint16_t pwmMax
        ): 
            _leftIn1(leftIn1), _leftIn2(leftIn2), _leftPwm(leftPwm),
            _rightIn1(rightIn1), _rightIn2(rightIn2), _rightPwm(rightPwm),
            _leftChannel(leftChannel), _rightChannel(rightChannel),
            _pwmFrequency(pwmFrequency), _pwmResolution(pwmResolution), _pwmMax(pwmMax) 
        {}

        void begin() {
            pinMode(_leftIn1, OUTPUT);
            pinMode(_leftIn2, OUTPUT);
            pinMode(_rightIn1, OUTPUT);
            pinMode(_rightIn2, OUTPUT);

#if ESP_ARDUINO_VERSION_MAJOR >= 3
            ledcAttach(_leftPwm, _pwmFrequency, _pwmResolution);
            ledcAttach(_rightPwm, _pwmFrequency, _pwmResolution);
#else
            ledcSetup(_leftChannel, _pwmFrequency, _pwmResolution);
            ledcSetup(_rightChannel, _pwmFrequency, _pwmResolution);
            ledcAttachPin(_leftPwm, _leftChannel);
            ledcAttachPin(_rightPwm, _rightChannel);
#endif

            stop();
        }

        void drive(int speedA, int speedB) {
            speedA = constrain(speedA * MOTOR_CONNECTION, -_pwmMax, _pwmMax);
            speedB = constrain(speedB * MOTOR_CONNECTION, -_pwmMax, _pwmMax);

            if (_hasCommand && speedA == _speedA && speedB == _speedB) return;

            driveMotor(_leftIn1, _leftIn2, _leftPwm, _leftChannel, speedA);
            driveMotor(_rightIn1, _rightIn2, _rightPwm, _rightChannel, speedB);
            _speedA = speedA;
            _speedB = speedB;
            _hasCommand = true;
        }

        void stop() {
            _hasCommand = false;
            drive(0, 0);
        }

    private:
        const uint8_t _leftIn1;
        const uint8_t _leftIn2;
        const uint8_t _leftPwm;
        const uint8_t _rightIn1;
        const uint8_t _rightIn2;
        const uint8_t _rightPwm;
        const uint8_t _leftChannel;
        const uint8_t _rightChannel;
        const uint32_t _pwmFrequency;
        const uint8_t _pwmResolution;
        const uint16_t _pwmMax;

        int _speedA = 0;
        int _speedB = 0;
        bool _hasCommand = false;

        void driveMotor(uint8_t in1, uint8_t in2, uint8_t pwmPin, uint8_t channel, int speed) {
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

#if ESP_ARDUINO_VERSION_MAJOR >= 3
            (void)channel;
            ledcWrite(pwmPin, abs(speed));
#else
            (void)pwmPin;
            ledcWrite(channel, abs(speed));
#endif
        }
};

#endif