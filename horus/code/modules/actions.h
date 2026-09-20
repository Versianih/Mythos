#ifndef ACTIONS_H
#define ACTIONS_H

#include "../libs.h"
#include "motors.h"
#include "leds.h"

enum class Button {
    L1,
    L2,
    R1,
    R2,
    A
};

class Actions {
    public:
        Actions(Motors &motors, Leds &leds)
            : _motors(motors), _leds(leds) {}

        void begin() {
            _leds.setMode(LedMode::WAITING);
        }

        void onConnect() {
            reset();
            _leds.setMode(LedMode::CONNECTED);
        }

        void onDisconnect() {
            reset();
            _motors.stop();
            _leds.setMode(LedMode::WAITING);
        }

        void onMove(int8_t forward, int8_t turn) {
            _stickForward = forward;
            _stickTurn = turn;
            _lastCommand = millis();
        }

        void onButton(Button button, bool pressed) {
            switch (button) {
                case Button::R1:
                    _turbo = pressed;
                    break;

                case Button::L1:
                    _movementLocked = pressed;
                    break;

                case Button::L2:
                    _turnLeft = pressed;
                    break;

                case Button::R2:
                    _turnRight = pressed;
                    break;

                case Button::A:
                    _connectionTest = pressed;
                    break;
            }
            _lastCommand = millis();
        }

        void update() {
            unsigned long now = millis();

            if (now - _lastCommand > WATCHDOG_TIMEOUT_MS) {
                _motors.stop();
                return;
            }

            if (_movementLocked) {
                _motors.stop();
                _leds.setMode(LedMode::CONNECTED);
                updateConnectionTestIndicator();
                return;
            }

            int forward = _stickForward;
            int turn = _stickTurn;

            if (_turnLeft || _turnRight) {
                turn = (_turnRight ? BUTTON_TURN_SPEED : 0)
                    - (_turnLeft ? BUTTON_TURN_SPEED : 0);
            }

            applyMix(forward, turn);
        }

    private:
        Motors &_motors;
        Leds &_leds;

        int8_t _stickForward = 0;
        int8_t _stickTurn = 0;


        bool _turbo = false;
        bool _movementLocked = false;
        bool _turnLeft = false;
        bool _turnRight = false;
        bool _connectionTest = false;

        unsigned long _lastCommand = 0;

        static const unsigned long WATCHDOG_TIMEOUT_MS = 100;

        void reset() {
            _stickForward = 0;
            _stickTurn = 0;
            _turbo = false;
            _movementLocked = false;
            _turnLeft = _turnRight = false;
            _connectionTest = false;
        }

        int speedLimit() const {
            if (_turbo) return TURBO_SPEED;
            return NORMAL_SPEED;
        }

        void applyMix(int forward, int turn) {
            int left = forward + turn;
            int right = forward - turn;

            int magnitude = max(abs(left), abs(right));
            if (magnitude > 100) {
                left = left * 100 / magnitude;
                right = right * 100 / magnitude;
            }

            int limit = speedLimit();
            left = left * limit / 100;
            right = right * limit / 100;

            _motors.drive(left * 255 / 100, right * 255 / 100);

            if (!left && !right)  _leds.setMode(LedMode::CONNECTED);
            else if (_turbo)      _leds.setMode(LedMode::TURBO);
            else                  _leds.setMode(LedMode::ACTIVE);

            updateConnectionTestIndicator();
        }

        void updateConnectionTestIndicator() {
            _leds.setGreen(_connectionTest);
        }
};

#endif