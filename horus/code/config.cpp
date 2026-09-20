#include "config.h"

Motors motors(
	PIN_AIN1, PIN_AIN2, PIN_PWMA,
	PIN_BIN1, PIN_BIN2, PIN_PWMB,
	MOTOR_PWM_CHANNEL_A, MOTOR_PWM_CHANNEL_B,
	PWM_FREQ, PWM_RESOLUTION, PWM_MAX
);

Leds leds(PIN_LED_GREEN, PIN_LED_YELLOW, PIN_LED_RED);
Actions actions(motors, leds);
GamepadController gamepad(actions);