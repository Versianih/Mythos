#pragma once

#include "../libs.h"

enum class LedMode {
    BOOT,       // vermelho fixo
    WAITING,    // amarelo piscando: sem controle conectado
    CONNECTED,  // verde fixo: conectado e parado
    ACTIVE,     // verde + amarelo: em movimento
    TURBO,      // verde + amarelo + vermelho
    BRAKING     // vermelho fixo: freio ativo
};

class Leds {
    public:
        Leds(uint8_t greenPin, uint8_t yellowPin, uint8_t redPin)
            : _greenPin(greenPin), _yellowPin(yellowPin), _redPin(redPin) {}

        void begin() {
            pinMode(_greenPin, OUTPUT);
            pinMode(_yellowPin, OUTPUT);
            pinMode(_redPin, OUTPUT);
            setMode(LedMode::BOOT);
            update();
        }

        void setMode(LedMode mode) {
            if (mode == _mode) return;
            _mode = mode;
            _lastToggle = millis();
            _state = true;
            apply(true);
        }

        void update() {
            unsigned long interval = blinkInterval();
            if (interval == 0) return;

            unsigned long now = millis();
            if (now - _lastToggle >= interval) {
                _lastToggle = now;
                _state = !_state;
                apply(_state);
            }
        }

        void setGreen(bool on)  { digitalWrite(_greenPin, on); }
        void setYellow(bool on) { digitalWrite(_yellowPin, on); }
        void setRed(bool on)    { digitalWrite(_redPin, on); }

        void off() {
            setGreen(false);
            setYellow(false);
            setRed(false);
        }

    private:
        const uint8_t _greenPin;
        const uint8_t _yellowPin;
        const uint8_t _redPin;
        LedMode _mode = LedMode::BOOT;
        unsigned long _lastToggle = 0;
        bool _state = true;

        static const uint8_t GREEN  = 0x01;
        static const uint8_t YELLOW = 0x02;
        static const uint8_t RED    = 0x04;

        uint8_t pattern() const {
            switch (_mode) {
                case LedMode::BOOT:      return RED;
                case LedMode::WAITING:   return YELLOW;
                case LedMode::CONNECTED: return GREEN;
                case LedMode::ACTIVE:    return GREEN | YELLOW;
                case LedMode::TURBO:     return GREEN | YELLOW | RED;
                case LedMode::BRAKING:   return RED;
            }
            return 0;
        }

        unsigned long blinkInterval() const {
            switch (_mode) {
                case LedMode::WAITING: return 400;
                case LedMode::TURBO:   return 100;
                default:               return 0;
            }
        }

        void apply(bool on) {
            uint8_t mask = on ? pattern() : 0;
            setGreen(mask & GREEN);
            setYellow(mask & YELLOW);
            setRed(mask & RED);
        }
};