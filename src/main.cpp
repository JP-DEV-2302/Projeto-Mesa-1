#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <LED.h>
#include <DHT.h>
#include <DHT_U.h>

#include "WiFiManager.h"
#include "MqttManager.h"
#include "DebugManager.h"
#include "Componentes.h"

const int PINO_LED_RGB    = 48;
const int QUANTIDADE_LEDS = 1;
const int PINO_LAMPADA = 40;
Led lampada(PINO_LAMPADA);
const char TOPICO_COMANDO[] = "senai134/dev_01/Coordenador/esp32/statusLampada";


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
  pinMode(PINO_LAMPADA, OUTPUT);
  setupComponentes();
}

void loop()
{
  garantirWiFiConectado();
  garantirMQTTConectado();
  loopMQTT();
  verificarTemperaturaEUmidade();
}

void tratarMensagemRecebida(const char* topico, const String& mensagem)
{
  debugInfo("==============================");
  debugInfo("Mensagem recebida na aplicacao");
  debugInfo("==============================");

  if (topico == nullptr)
  {
    debugErro("Topico MQTT invalido");
    return;
  }

  debugInfo("Topico: " + String(topico));
  debugInfo("Mensagem: " + mensagem);

  if (strcmp(topico, TOPICO_COMANDO) == 0)
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
  ledRGB.setPixelColor(0, ledRGB.Color(
    constrain(vermelho, 0, 255),
    constrain(verde,    0, 255),
    constrain(azul,     0, 255)
  ));
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

  if (erro)
  {
    debugErro("Erro ao interpretar o Json.");
    debugErro(erro.c_str());
    return;
  }

  // ---------------- LED RGB ----------------
  if (doc["led"].is<JsonObject>())
  {
    if (!doc["led"]["r"].is<int>() || !doc["led"]["g"].is<int>() || !doc["led"]["b"].is<int>())
    {
      debugErro("JSON INVALIDO. use led.r, led.g e led.b");
      return;
    }

    alterarCorLEDRGB(
      doc["led"]["r"].as<int>(),
      doc["led"]["g"].as<int>(),
      doc["led"]["b"].as<int>()
    );
  }

  // ---------------- LAMPADA ----------------
  if (doc["lampada"].is<bool>())
  {
    bool estadoLampada = doc["lampada"].as<bool>();
    estadoLampada ? lampada.acender() : lampada.apagar();
    debugInfo("Estado lampada: " + String(estadoLampada));
  }
}