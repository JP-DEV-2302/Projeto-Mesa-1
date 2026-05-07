#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <LED.h>

#include "WiFiManager.h"
#include "MqttManager.h"
#include "DebugManager.h"

const int PINO_LED_RGB = 48;
const int QUANTIDADE_LEDS = 1;
const int PINO_LAMPADA = 40;
Led lampada(PINO_LAMPADA);
const char TOPICO_COMANDO[] = "senai134/dev_01/esp32/comando";

Adafruit_NeoPixel ledRGB(QUANTIDADE_LEDS, PINO_LED_RGB, NEO_GRB + NEO_KHZ800);

void tratarMensagemRecebida(const char* topico, const String& mensagem);
void configurarLEDRGB();
void alterarCorLEDRGB(int vermelho, int verde, int azul);
void tratarJsonLEDRGB(const String& mensagem);

void setup()
{
  Serial.begin(9600);
  configurarDebug();
  conectarWiFi();
  configurarMQTT();
  registrarCallbackMensagem(tratarMensagemRecebida);
  conectarMQTT();
  configurarLEDRGB();
  pinMode(PINO_LAMPADA, OUTPUT); // 🔧 ADICIONADO (CORRETO)
}

void loop()
{
  garantirWiFiConectado();
  garantirMQTTConectado();
  loopMQTT();
}

void tratarMensagemRecebida(const char* topico, const String& mensagem)
{
  debugInfo("==============================");
  debugInfo("Mensagem recebida na aplicacao");
  debugInfo("==============================");

  if(topico == nullptr)
  {
    debugErro("Topico MQTT invalido");
    return;
  }

  debugInfo("Topico: " + String(topico));
  debugInfo("Mensagem: " + mensagem);

  if(strcmp(topico, TOPICO_COMANDO) == 0)
  {
    tratarJsonLEDRGB(mensagem);
    return;
  }

  debugErro("Topico nao tratado: " + String(topico));
}

void configurarLEDRGB()
{
  ledRGB.begin();
  ledRGB.setBrightness(80);
  ledRGB.clear();
  ledRGB.show();
  debugInfo("LED RGB configurado no GPIO " + String(PINO_LED_RGB));
}

void alterarCorLEDRGB(int vermelho, int verde, int azul)
{
  vermelho = constrain(vermelho, 0, 255);
  verde = constrain(verde, 0, 255);
  azul = constrain(azul, 0, 255);

  ledRGB.setPixelColor(0, ledRGB.Color(vermelho, verde, azul));
  ledRGB.show();

  debugInfo("Cor aplicada no led RGB");
  debugInfo("R: " + String(vermelho));
  debugInfo("G: " + String(verde));
  debugInfo("B: " + String(azul));
}

void tratarJsonLEDRGB(const String& mensagem)
{
  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, mensagem);

  if(erro)
  {
    debugErro("Erro ao interpretar o Json.");
    debugErro(erro.c_str());
    return; // 🔧 IMPORTANTE
  }

  // ---------------- LED RGB ----------------
  if(doc["led"].is<JsonObject>())
  {
    if(!doc["led"]["r"].is<int>() || !doc["led"]["g"].is<int>() || !doc["led"]["b"].is<int>())
    {
      debugErro("JSON INVALIDO. use led.r, led.g e led.b");
      return;
    }

    int vermelho = doc["led"]["r"].as<int>();
    int verde = doc["led"]["g"].as<int>();
    int azul = doc["led"]["b"].as<int>();

    alterarCorLEDRGB(vermelho, verde, azul);
  }

  // ---------------- LAMPADA (CORRIGIDO) ----------------
  if(doc["lampada"].is<bool>())
  {
    bool estadoLampada = doc["lampada"].as<bool>();

    lampada.acender();
    

    debugInfo("Estado lampada: " + String(estadoLampada));
  }
  else if(doc.containsKey("lampada"))
  {
    debugErro("JSON INVALIDO. Use lampada: true ou false");
  }
}