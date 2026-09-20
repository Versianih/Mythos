#include "config.h"

void setup() {
    Serial.begin(115200);
    delay(100);

    leds.begin();
    motors.begin();
    gamepad.begin();
    actions.begin();
}

void loop() {
    gamepad.update();
    actions.update();
    leds.update();
    yield();
}