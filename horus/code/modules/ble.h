#ifndef BLE_LINK_H
#define BLE_LINK_H

#include "../libs.h"
#include "actions.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>


class BleLink {
    public:
        BleLink(Actions &actions) : _actions(actions) {}

        void begin() {
            BLEDevice::init(BLE_DEVICE_NAME);
            BLEDevice::setPower(BLE_TX_POWER);

            BLEServer *server = BLEDevice::createServer();
            server->setCallbacks(new ServerCallbacks(this));

            BLEService *service = server->createService(BLE_SERVICE_UUID);
            BLECharacteristic *rx = service->createCharacteristic(
                BLE_RX_UUID,
                BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
            );
            rx->setCallbacks(new RxCallbacks(this));
            service->start();

            BLEAdvertising *advertising = BLEDevice::getAdvertising();
            advertising->addServiceUUID(BLE_SERVICE_UUID);
            advertising->setScanResponse(true);
            BLEDevice::startAdvertising();

            Serial.printf("BLE pronto. Anunciando como \"%s\"...\n", BLE_DEVICE_NAME);
        }

        void update() {
            bool evConnect, evDisconnect, hasPacket;
            uint8_t packet[PACKET_LEN];

            portENTER_CRITICAL(&_mux);
            evConnect = _evConnect;       _evConnect = false;
            evDisconnect = _evDisconnect; _evDisconnect = false;
            hasPacket = _hasPacket;
            if (hasPacket) {
                memcpy(packet, _rx, PACKET_LEN);
                _hasPacket = false;
            }
            portEXIT_CRITICAL(&_mux);

            if (evConnect) {
                _connected = true;
                _prevButtons = 0;
                Serial.println("Celular conectado!");
                _actions.onConnect();
            }

            if (evDisconnect) {
                _connected = false;
                Serial.println("Celular desconectado.");
                _actions.onDisconnect();
                _restartAdvAt = millis() + 300;
                _restartAdvPending = true;
            }

            if (hasPacket && _connected) handlePacket(packet);

            if (_restartAdvPending && (int32_t)(millis() - _restartAdvAt) >= 0) {
                _restartAdvPending = false;
                BLEDevice::startAdvertising();
                Serial.println("Anunciando de novo...");
            }
        }

        bool isConnected() const { return _connected; }

        void onClientConnect() {
            portENTER_CRITICAL(&_mux);
            _evConnect = true;
            portEXIT_CRITICAL(&_mux);
        }

        void onClientDisconnect() {
            portENTER_CRITICAL(&_mux);
            _evDisconnect = true;
            _hasPacket = false;
            portEXIT_CRITICAL(&_mux);
        }

        void onRx(const uint8_t *data, size_t len) {
            if (!data || len != PACKET_LEN) {
                _badPackets++;
                return;
            }
            portENTER_CRITICAL(&_mux);
            memcpy(_rx, data, PACKET_LEN);
            _hasPacket = true;
            portEXIT_CRITICAL(&_mux);
        }

    private:
        class ServerCallbacks : public BLEServerCallbacks {
            public:
                ServerCallbacks(BleLink *link) : _link(link) {}
                void onConnect(BLEServer *) override { _link->onClientConnect(); }
                void onDisconnect(BLEServer *) override { _link->onClientDisconnect(); }
            private:
                BleLink *_link;
        };

        class RxCallbacks : public BLECharacteristicCallbacks {
            public:
                RxCallbacks(BleLink *link) : _link(link) {}
                void onWrite(BLECharacteristic *characteristic) override {
                    _link->onRx(characteristic->getData(), characteristic->getLength());
                }
            private:
                BleLink *_link;
        };

        Actions &_actions;

        portMUX_TYPE _mux = portMUX_INITIALIZER_UNLOCKED;
        uint8_t _rx[PACKET_LEN] = {0};
        volatile bool _hasPacket = false;
        volatile bool _evConnect = false;
        volatile bool _evDisconnect = false;
        volatile uint32_t _badPackets = 0;

        bool _connected = false;
        uint8_t _prevButtons = 0;
        bool _restartAdvPending = false;
        unsigned long _restartAdvAt = 0;
        unsigned long _lastLog = 0;

        void handlePacket(const uint8_t *p) {
            uint8_t checksum = (uint8_t)(p[1] + p[2] + p[3] + p[4] + p[5]);
            if (p[0] != PACKET_SYNC || checksum != p[6]) {
                _badPackets++;
                return;
            }

            int lx = axis(p[1]);
            int ly = axis(p[2]);
            int rx = axis(p[3]);
            int ry = axis(p[4]);
            uint8_t buttons = p[5];

            _actions.onMove((int8_t)ly, (int8_t)rx);

            updateButton(buttons, BTN_TURBO, Button::TURBO);
            updateButton(buttons, BTN_STOP, Button::STOP);
            updateButton(buttons, BTN_TURN_LEFT, Button::TURN_LEFT);
            updateButton(buttons, BTN_TURN_RIGHT, Button::TURN_RIGHT);
            updateButton(buttons, BTN_TEST, Button::TEST);
            _prevButtons = buttons;

#ifdef DEBUG_LOG_MS
            unsigned long now = millis();
            if (now - _lastLog >= DEBUG_LOG_MS) {
                _lastLog = now;
                Serial.printf("LX:%4d LY:%4d RX:%4d RY:%4d  turbo:%d parado:%d  ruins:%lu\n",
                    lx, ly, rx, ry,
                    (buttons & BTN_TURBO) != 0, (buttons & BTN_STOP) != 0,
                    (unsigned long)_badPackets);
            }
#else
            (void)lx; (void)ry;
#endif
        }

        void updateButton(uint8_t buttons, uint8_t mask, Button button) {
            bool now = (buttons & mask) != 0;
            bool before = (_prevButtons & mask) != 0;
            if (now != before) _actions.onButton(button, now);
        }

        int axis(uint8_t raw) const {
            int v = constrain((int)raw - AXIS_OFFSET, -100, 100);
            if (abs(v) < STICK_DEADZONE) return 0;
            return v > 0
                ? (v - STICK_DEADZONE) * 100 / (100 - STICK_DEADZONE)
                : (v + STICK_DEADZONE) * 100 / (100 - STICK_DEADZONE);
        }
};

#endif