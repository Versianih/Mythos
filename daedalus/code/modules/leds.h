#pragma once

#include <Arduino.h>

class Leds {
public:
    explicit Leds(uint8_t greenPin, uint8_t redPin, uint8_t yellowPin, uint8_t bluePin)
        : greenPin_(greenPin), redPin_(redPin), yellowPin_(yellowPin), bluePin_(bluePin) {
        states_[0].ledPin = greenPin_;
        states_[1].ledPin = redPin_;
        states_[2].ledPin = yellowPin_;
        states_[3].ledPin = bluePin_;

        pinMode(greenPin_, OUTPUT);
        pinMode(redPin_, OUTPUT);
        pinMode(yellowPin_, OUTPUT);
        pinMode(bluePin_, OUTPUT);

        off();
    }

    void green() { activateLed(0); }
    void red() { activateLed(1); }
    void yellow() { activateLed(2); }
    void blue() { activateLed(3); }


    void off() {
        for (uint8_t i = 0; i < 4; ++i) {
            digitalWrite(states_[i].ledPin, LOW);
            states_[i].isActive = false;
        }
    }

private:
    struct ledState {
        uint8_t ledPin;
        bool isActive;
    };

    uint8_t greenPin_;
    uint8_t redPin_;
    uint8_t yellowPin_;
    uint8_t bluePin_;
    ledState states_[4] = {{0, false}, {0, false}, {0, false}, {0, false}};

    void activateLed(uint8_t index) {
        states_[index].isActive = true;
        digitalWrite(states_[index].ledPin, HIGH);
    }
};