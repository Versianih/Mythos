#include "config.h"

void setup() {
    Serial.begin(115200);
    delay(100);

    leds.begin();
    motors.begin();
    actions.begin();
    ble.begin();
}

void loop() {
    ble.update();
    actions.update();
    leds.update();
    delay(1);
}