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

// Configuração do LED RGB (NeoPixel)
const int PINO_LED_RGB    = 48;
const int QUANTIDADE_LEDS = 1;

// Configuração da lâmpada
const int PINO_LAMPADA = 40;
Led lampada(PINO_LAMPADA);

// Tópico MQTT que a aplicação escuta
const char TOPICO_COMANDO[] = "senai134/dev_01/Coordenador/esp32/statusLampada";

Adafruit_NeoPixel ledRGB(QUANTIDADE_LEDS, PINO_LED_RGB, NEO_GRB + NEO_KHZ800);

// Declarações antecipadas
void tratarMensagemRecebida(const char* topico, const String& mensagem);
void configurarLEDRGB();
void alterarCorLEDRGB(int vermelho, int verde, int azul);
void tratarJsonLEDRGB(const String& mensagem);

// -------------------------------------------------------
// Inicializa todos os componentes do sistema
// -------------------------------------------------------
void setup()
{
  Serial.begin(9600);
  configurarDebug();
  conectarWiFi();
  configurarMQTT();
  registrarCallbackMensagem(tratarMensagemRecebida);
  conectarMQTT();
  configurarLEDRGB();
  setupComponentes();
}

// -------------------------------------------------------
// Loop principal: mantém conexões ativas e processa dados
// -------------------------------------------------------
void loop()
{
  garantirWiFiConectado();
  garantirMQTTConectado();
  loopMQTT();
  verificarTemperaturaEUmidade();
}

// -------------------------------------------------------
// Callback chamado ao receber qualquer mensagem MQTT.
// Direciona para o handler correto conforme o tópico.
// -------------------------------------------------------
void tratarMensagemRecebida(const char* topico, const String& mensagem)
{
  if (topico == nullptr)
  {
    debugErro("Topico MQTT invalido.");
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

// -------------------------------------------------------
// Inicializa o LED RGB com brilho padrão e apaga
// -------------------------------------------------------
void configurarLEDRGB()
{
  ledRGB.begin();
  ledRGB.setBrightness(80);
  ledRGB.clear();
  ledRGB.show();
  debugInfo("LED RGB configurado no GPIO " + String(PINO_LED_RGB));
}

// -------------------------------------------------------
// Aplica uma cor RGB no LED. Valores são limitados a 0-255.
// -------------------------------------------------------
void alterarCorLEDRGB(int vermelho, int verde, int azul)
{
  ledRGB.setPixelColor(0, ledRGB.Color(
    constrain(vermelho, 0, 255),
    constrain(verde,    0, 255),
    constrain(azul,     0, 255)
  ));
  ledRGB.show();

  debugInfo("Cor aplicada — R:" + String(vermelho) + " G:" + String(verde) + " B:" + String(azul));
}

// -------------------------------------------------------
// Interpreta o JSON recebido e atualiza LED RGB e/ou lâmpada.
//
// Formato esperado:
//   { "led": { "r": 255, "g": 0, "b": 0 }, "lampada": true }
// -------------------------------------------------------
void tratarJsonLEDRGB(const String& mensagem)
{
  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, mensagem);

  if (erro)
  {
    debugErro("Erro ao interpretar JSON: " + String(erro.c_str()));
    return;
  }

  // Atualiza cor do LED RGB, se presente no JSON
  if (doc["led"].is<JsonObject>())
  {
    if (!doc["led"]["r"].is<int>() || !doc["led"]["g"].is<int>() || !doc["led"]["b"].is<int>())
    {
      debugErro("JSON invalido para LED. Esperado: led.r, led.g, led.b");
      return;
    }

    alterarCorLEDRGB(
      doc["led"]["r"].as<int>(),
      doc["led"]["g"].as<int>(),
      doc["led"]["b"].as<int>()
    );
  }

  // Liga ou desliga a lâmpada, se presente no JSON
  if (doc["lampada"].is<bool>())
  {
    bool estado = doc["lampada"].as<bool>();
    estado ? lampada.acender() : lampada.apagar();
    debugInfo("Lampada: " + String(estado ? "ligada" : "desligada"));
  }
}