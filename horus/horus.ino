#include "config.h"

#define DEVICE_NAME "HORUS"

Motors motors;
Leds leds;
Actions actions(motors, leds);
BLE ble(actions);

void setup() {
    Serial.begin(115200);
    delay(100);

    motors.begin();
    leds.begin();
    actions.begin();
    ble.begin(DEVICE_NAME);
}

void loop() {
    actions.update();
    leds.update();
    delay(10);
}