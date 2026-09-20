#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

class Leds {
public:
    explicit Leds(uint8_t greenPin, uint8_t redPin, uint8_t yellowPin)
        : greenPin_(greenPin), redPin_(redPin), yellowPin_(yellowPin) {
        pinMode(greenPin_, OUTPUT);
        pinMode(redPin_, OUTPUT);
        pinMode(yellowPin_, OUTPUT);
        off();
    }

    void setGreen(bool on)  { digitalWrite(greenPin_, on ? HIGH : LOW); }
    void setRed(bool on)    { digitalWrite(redPin_, on ? HIGH : LOW); }
    void setYellow(bool on) { digitalWrite(yellowPin_, on ? HIGH : LOW); }

    void green()  { off(); setGreen(true); }
    void red()    { off(); setRed(true); }
    void yellow() { off(); setYellow(true); }

    void off() {
        setGreen(false);
        setRed(false);
        setYellow(false);
    }

private:
    uint8_t greenPin_;
    uint8_t redPin_;
    uint8_t yellowPin_;
};

#endif