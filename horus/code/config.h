#ifndef CONFIG_H
#define CONFIG_H

#include "libs.h"
#include "calibration.h"

#include "modules/motors.h"
#include "modules/leds.h"
#include "modules/actions.h"
#include "modules/ble.h"

extern Motors motors;
extern Leds leds;
extern Actions actions;
extern BleLink ble;

#endif