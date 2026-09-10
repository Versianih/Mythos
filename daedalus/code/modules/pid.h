#pragma once

#include <Arduino.h>

class LinePID {
public:
  explicit LinePID(int* arrayLeituras)
    : leituras(arrayLeituras),
      Kp(KP_LINE), Ki(KI_LINE), Kd(KD_LINE),
      erro_anterior(0.0f), integral(0.0f) {

    float offset = (NUM_SENSORS_IR - 1) / 2.0f;
    for (int i = 0; i < NUM_SENSORS_IR; i++) {
      pesos[i] = i - offset;
    }
  }

  void setGanhosPID(float kp, float ki, float kd) {
    Kp = kp; Ki = ki; Kd = kd;
  }

  float calcularErro() {
    float soma_ponderada = 0.0f;
    float soma_ativa     = 0.0f;

    for (int i = 0; i < NUM_SENSORS_IR; i++) {
      float v = 4095.0f - leituras[i];
      soma_ponderada += v * pesos[i];
      soma_ativa     += v;
    }

    if (soma_ativa < SOMA_ATIVA_MINIMA) {
      return 0.0f;
    }

    float peso_max = (NUM_SENSORS_IR - 1) / 2.0f;
    float erro     = soma_ponderada / (soma_ativa * peso_max);
    return constrain(erro, -1.0f, 1.0f);
  }

  float calcularCorrecao() {
    float erro = calcularErro();

    integral += erro;
    integral = constrain(integral, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);

    float derivada = erro - erro_anterior;
    erro_anterior  = erro;

    float saida = (Kp * erro) + (Ki * integral) + (Kd * derivada);
    return constrain(saida, -1.0f, 1.0f);
  }

  void resetar() {
    erro_anterior = 0.0f;
    integral      = 0.0f;
  }

  float getErroAtual()   const { return erro_anterior; }
  float getIntegral()    const { return integral; }

private:
  int*  leituras;
  float Kp, Ki, Kd;
  float erro_anterior;
  float integral;
  float pesos[NUM_SENSORS_IR];

  static constexpr float SOMA_ATIVA_MINIMA = 2000.0f;
  static constexpr float INTEGRAL_LIMIT    = MAX_INTEGRAL;
};