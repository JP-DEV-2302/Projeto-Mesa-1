#ifndef COMPONENTES_H
#define COMPONENTES_H

#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include "DebugManager.h"
#include "MqttManager.h"

// ---------------- PINOS ----------------
#define DHTPIN      10      // 🔥 corrigido (mais estável no ESP32-S3)
#define DHTTYPE     DHT22
#define BUZZER_PIN  7

// ---------------- LIMITES ----------------
#define TEMP_MIN 20.0f
#define TEMP_MAX 30.0f
#define UMID_MIN 45.0f
#define UMID_MAX 60.0f

// ---------------- MQTT ----------------
#define INDICE_TOPICO_TEMPERATURA 2
#define INDICE_TOPICO_UMIDADE     1
#define INDICE_TOPICO_ALARME      3

#define MAX_FALHAS_DHT 5

extern int buzzer;
extern DHT dht;
extern LiquidCrystal_I2C lcd;

void setupComponentes();
void verificarTemperaturaEUmidade();

#endif