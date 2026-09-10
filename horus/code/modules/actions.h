#pragma once

#include "libs.h"
#include "motors.h"
#include "leds.h"


class Actions {
    public:
        Actions(Motors &motors, Leds &leds)
            : _motors(motors), _leds(leds) {}

        void begin() {
            _leds.setMode(LedMode::WAITING);
        }

        void onConnect() {
            _leds.setMode(LedMode::CONNECTED);
        }

        void onDisconnect() {
            _motors.stop();
            _leds.setMode(LedMode::WAITING);
        }

        void onDrive(int8_t speedA, int8_t speedB) {
            _motors.drive(map(speedA, -100, 100, -255, 255),
                          map(speedB, -100, 100, -255, 255));
            _leds.setMode(LedMode::ACTIVE);
            _lastCommand = millis();
        }

        void update() {
            if (millis() - _lastCommand > WATCHDOG_TIMEOUT_MS) {
                _motors.stop();
            }
        }

    private:
        Motors &_motors;
        Leds &_leds;
        unsigned long _lastCommand = 0;

        static const unsigned long WATCHDOG_TIMEOUT_MS = 500;
};