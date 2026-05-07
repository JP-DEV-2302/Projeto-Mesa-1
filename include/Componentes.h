#ifndef COMPONENTES_H
#define COMPONENTES_H

#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include "DebugManager.h"
#include "WiFiManager.h"
#include "MqttManager.h"
#include "secrets.h"

// --- Pinos (ajuste conforme seu hardware) ---
#define DHTPIN           13   // ← ajuste
#define DHTTYPE          DHT22
#define BUZZER_PIN       12   // ← ajuste
#define LED_QUARTO_PIN   26   // ← ajuste
#define LED_QUARTO2_PIN  27   // ← ajuste

// --- Tópicos de temperatura/umidade ---
extern const char* topic_temp;
extern const char* topic_umid;

// --- Variáveis globais de hardware ---
extern int             buzzer;
extern int             ledQuarto;
extern int             ledQuarto2;
extern DHT             dht;
extern LiquidCrystal_I2C lcd;

// --- Protótipos ---
void setupComponentes();
void alarme_dois_tons();
void verificarTemperaturaEUmidade();

#endif