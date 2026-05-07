#ifndef COMPONENTES_H
#define COMPONENTES_H

#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include "DebugManager.h"
#include "WiFiManager.h"
#include "MqttManager.h"
#include "secrets.h"

// Pinos dos componentes — ajuste conforme o hardware utilizado
#define DHTPIN          13
#define DHTTYPE         DHT22
#define BUZZER_PIN      12
#define LED_QUARTO_PIN  26
#define LED_QUARTO2_PIN 27

// Tópicos MQTT de publicação de temperatura e umidade
extern const char* topic_temp;
extern const char* topic_umid;

// Instâncias e pinos dos componentes físicos
extern int            buzzer;
extern int            ledQuarto;
extern int            ledQuarto2;
extern DHT            dht;
extern LiquidCrystal_I2C lcd;

// Inicializa todos os componentes (chamar no setup())
void setupComponentes();

// Emite alarme sonoro de dois tons alternados
void alarme_dois_tons();

// Lê DHT, publica via MQTT e atualiza o LCD (chamar no loop())
void verificarTemperaturaEUmidade();

#endif