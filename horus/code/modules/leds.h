#pragma once

#include "libs.h"

enum class LedMode {
    WAITING,
    CONNECTED,
    ACTIVE
};

class Leds {
    public:
        Leds() {}

        void begin() {
            pinMode(PIN_LED_STATUS, OUTPUT);
            digitalWrite(PIN_LED_STATUS, LOW);
        }

        void setMode(LedMode mode) {
            _mode = mode;
        }

        void update() {
            unsigned long interval = blinkInterval();

            if (interval == 0) {
                digitalWrite(PIN_LED_STATUS, HIGH);
                return;
            }

            unsigned long now = millis();
            if (now - _lastToggle >= interval) {
                _lastToggle = now;
                _state = !_state;
                digitalWrite(PIN_LED_STATUS, _state);
            }
        }

    private:
        LedMode _mode = LedMode::WAITING;
        unsigned long _lastToggle = 0;
        bool _state = false;

        unsigned long blinkInterval() {
            switch (_mode) {
                case LedMode::WAITING:   return 500;
                case LedMode::ACTIVE:    return 100;
                case LedMode::CONNECTED: return 0;
            }
            return 500;
        }
};