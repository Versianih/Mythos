#pragma once

#include "../libs.h"
#include "actions.h"


class GamepadController {
    public:
        GamepadController(Actions &actions) : _actions(actions) {
            _instance = this;
        }

        void begin() {
            BP32.setup(&GamepadController::onConnectedStatic, &GamepadController::onDisconnectedStatic);
            // BP32.forgetBluetoothKeys();
            BP32.enableVirtualDevice(false);
            Serial.println("Aguardando controle...");
        }

        void update() {
            BP32.update();

            if (!_controller || !_controller->isConnected() || !_controller->hasData()) return;

            int8_t forward = -normalize(_controller->axisY(), LY_CENTER, LY_MIN, LY_MAX);
            int8_t turn    =  normalize(_controller->axisRX(), RX_CENTER, RX_MIN, RX_MAX);
            _actions.onMove(forward, turn);

            updateButton(Button::L1, _controller->l1(), _prevL1);
            updateButton(Button::L2, _controller->l2(), _prevL2);
            updateButton(Button::R1, _controller->r1(), _prevR1);
            updateButton(Button::R2, _controller->r2(), _prevR2);
            updateButton(Button::A, _controller->a(), _prevA);
        }

    private:
        Actions &_actions;
        ControllerPtr _controller = nullptr;

        bool _prevL1 = false;
        bool _prevL2 = false;
        bool _prevR1 = false;
        bool _prevR2 = false;
        bool _prevA = false;

        inline static GamepadController *_instance = nullptr;

        static void onConnectedStatic(ControllerPtr ctl) {
            if (_instance) _instance->onConnected(ctl);
        }

        static void onDisconnectedStatic(ControllerPtr ctl) {
            if (_instance) _instance->onDisconnected(ctl);
        }

        void onConnected(ControllerPtr ctl) {
            if (_controller) {
                ctl->disconnect();
                return;
            }
            _controller = ctl;
            _prevL1 = _prevL2 = _prevR1 = _prevR2 = _prevA = false;
            Serial.println("Controle conectado!");
            _actions.onConnect();
        }

        void onDisconnected(ControllerPtr ctl) {
            if (_controller != ctl) return;
            _controller = nullptr;
            Serial.println("Controle desconectado.");
            _actions.onDisconnect();
        }

        void updateButton(Button button, bool pressed, bool &previous) {
            if (pressed != previous) {
                previous = pressed;
                _actions.onButton(button, pressed);
            }
        }

        int8_t normalize(int value, int center, int minValue, int maxValue) {
            int delta = value - center;
            if (abs(delta) < AXIS_DEADZONE) return 0;

            long result = delta > 0
                ? map(delta, AXIS_DEADZONE, maxValue - center, 0, 100)
                : map(delta, minValue - center, -AXIS_DEADZONE, -100, 0);

            return (int8_t)constrain(result, -100, 100);
        }
};