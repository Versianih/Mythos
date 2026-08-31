#pragma once

#include "libs.h"
#include <NimBLEDevice.h>
#include "actions.h"

#define SERVICE_UUID     "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_DRIVE_UUID  "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class BLE {
    public:
        BLE(Actions &actions) : _actions(actions) {}

        void begin(const char *deviceName) {
            NimBLEDevice::init(deviceName);
            NimBLEDevice::setPower(ESP_PWR_LVL_P9);

            _server = NimBLEDevice::createServer();
            _server->setCallbacks(new ServerCallbacks(_actions));

            NimBLEService *service = _server->createService(SERVICE_UUID);

            _driveChar = service->createCharacteristic(
                CHAR_DRIVE_UUID,
                NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
            );
            _driveChar->setCallbacks(new DriveCallbacks(_actions));

            service->start();

            NimBLEAdvertising *advertising = NimBLEDevice::getAdvertising();
            advertising->addServiceUUID(SERVICE_UUID);
            advertising->start();
        }

    private:
        Actions &_actions;
        NimBLEServer *_server = nullptr;
        NimBLECharacteristic *_driveChar = nullptr;

        class ServerCallbacks : public NimBLEServerCallbacks {
            public:
                ServerCallbacks(Actions &actions) : _actions(actions) {}

                void onConnect(NimBLEServer *server, NimBLEConnInfo &connInfo) override {
                    _actions.onConnect();
                }

                void onDisconnect(NimBLEServer *server, NimBLEConnInfo &connInfo, int reason) override {
                    _actions.onDisconnect();
                    NimBLEDevice::getAdvertising()->start();
                }

            private:
                Actions &_actions;
        };

        class DriveCallbacks : public NimBLECharacteristicCallbacks {
            public:
                DriveCallbacks(Actions &actions) : _actions(actions) {}

                void onWrite(NimBLECharacteristic *chr, NimBLEConnInfo &connInfo) override {
                    std::string value = chr->getValue();
                    if (value.size() < 2) return;

                    int8_t speedA = (int8_t)value[0];
                    int8_t speedB = (int8_t)value[1];
                    _actions.onDrive(speedA, speedB);
                }

            private:
                Actions &_actions;
        };
};