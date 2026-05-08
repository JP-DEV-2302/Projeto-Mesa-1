#ifndef COMPONENTES_H
#define COMPONENTES_H

#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include "DebugManager.h"
#include "MqttManager.h"

// Pinos dos componentes — ajuste conforme o hardware utilizado
#define DHTPIN      10
#define DHTTYPE     DHT22
#define BUZZER_PIN  7

// Faixas seguras para sala de arquivos (ABNT NBR 9077 / ISO 11799)
#define TEMP_MIN    20.0f
#define TEMP_MAX    30.0f
#define UMID_MIN    45.0f
#define UMID_MAX    60.0f

// Índices dos tópicos de publicação definidos em secrets.cpp
// [0] = comandoLampada | [1] = comandoUmidade | [2] = comandoTemperatura | [3] = statusAlarme
#define INDICE_TOPICO_TEMPERATURA 2
#define INDICE_TOPICO_UMIDADE     1
#define INDICE_TOPICO_ALARME      3

// Número máximo de falhas consecutivas do DHT antes de logar repetidamente
#define MAX_FALHAS_DHT 5

// Instâncias dos componentes físicos (definidas em Componentes.cpp)
extern int              buzzer;
extern DHT              dht;
extern LiquidCrystal_I2C lcd;

// Inicializa todos os componentes (chamar no setup())
void setupComponentes();

// Lê DHT, publica via MQTT e atualiza o LCD — não-bloqueante (chamar no loop())
void verificarTemperaturaEUmidade();

#endif