#include "config.h"

StateMachine maquina;
State *stateParar;
State *stateSeguirLinha;

/* ====== TRANSIÇÕES ====== */
void checkStart() {
  static bool maquinaRodando = false;
  static unsigned long ultimoPressionamentoBtnStart = 0;

  if (digitalRead(BTN1) == LOW) {
    unsigned long tempoAtual = millis();
    if (tempoAtual - ultimoPressionamentoBtnStart > DEBOUNCE_START) {
      ultimoPressionamentoBtnStart = tempoAtual;

      if (maquinaRodando) {
        maquinaRodando = false;
        maquina.transitionTo(stateParar);
      } else {
        maquinaRodando = true;
        maquina.transitionTo(stateSeguirLinha);
        leds.off();
        leds.green();
      }
    }
  }
}

/* ====== ESTADOS ====== */
void estadoParar() {
  leds.off();
  leds.red();
  linePid.resetar();
  motorsControl.stop();
}

void estadoSeguirLinha() {
  sensorsIr.read();
  float correction = linePid.calcularCorrecao();
  motorsControl.setCorrection(BASE_SPEED * correction, BASE_SPEED, true);
}


/* ====== SETUP / LOOP ====== */
void setup() {
  Serial.begin(115200);

  leds.yellow();

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);

  analogReadResolution(10);         // 10 bits (0-1023) é mais rápido que 12 bits e suficiente para o PID
  // analogSetAttenuation(ADC_11db);   // Permite ler a faixa completa de 0 a 3.3V

  motorsControl.begin();
  sensorsIr.begin();
  
  if (IN_CALIBRATION) {
    calibrate(leds, sensorsIr);
    while (true) {
      leds.green();
      delay(300);
      leds.off();
      delay(300);
    }
  }

  stateParar = maquina.addState(&estadoParar);
  stateSeguirLinha = maquina.addState(&estadoSeguirLinha);
  
  maquina.transitionTo(stateParar);

  leds.off();
  leds.green();

  delay(100);
}

void loop() {
  // maquina.run();
  // checkStart();
  sensorsIr.read();

  sensorsIr.debugPrint();

  delay(10);
}