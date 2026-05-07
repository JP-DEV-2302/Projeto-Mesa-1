#include "Componentes.h"

// Tópicos MQTT seguindo o padrão do projeto (definido em secrets.h)
const char* topic_temp = "senai134/dev_01/Coordenador/esp32/comandoTemperatura";
const char* topic_umid = "senai134/dev_01/Coordenador/esp32/comandoUmidade";

// Pinos dos componentes (definidos em Componentes.h)
int buzzer     = BUZZER_PIN;
int ledQuarto  = LED_QUARTO_PIN;
int ledQuarto2 = LED_QUARTO2_PIN;

// Instâncias do sensor DHT e display LCD
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Use 0x3F se o display não ligar

// -------------------------------------------------------
// Inicializa todos os componentes físicos.
// WiFi e MQTT são configurados no main.cpp — não repetir aqui.
// -------------------------------------------------------
void setupComponentes()
{
    pinMode(buzzer,     OUTPUT);
    pinMode(ledQuarto,  OUTPUT);
    pinMode(ledQuarto2, OUTPUT);

    dht.begin();
    lcd.init();
    lcd.backlight();

    // Aguarda estabilização do sensor DHT
    debugInfo("Calibrando sensores...");
    for (int i = 0; i < 5; i++)
    {
        delay(1500);
        debugInfoSemLinha(".");
    }
    debugInfoSemLinha("\n\r");
    debugInfo("Sensores calibrados. Sistema pronto.");
}

// -------------------------------------------------------
// Emite alarme sonoro de dois tons alternados (2 ciclos).
// Cada tom toca pelo tempo de duracaoTom antes do próximo.
// -------------------------------------------------------
void alarme_dois_tons()
{
    const int freqAlta   = 2000;
    const int freqBaixa  = 800;
    const int duracaoTom = 300;

    for (int i = 0; i < 2; i++)
    {
        tone(buzzer, freqAlta);
        delay(duracaoTom);
        tone(buzzer, freqBaixa);
        delay(duracaoTom);
        delay(500);
        tone(buzzer, freqAlta);
        delay(duracaoTom);
        tone(buzzer, freqBaixa);
        delay(duracaoTom);
    }
    noTone(buzzer);
}

// -------------------------------------------------------
// Lê temperatura e umidade do DHT, publica via MQTT,
// aciona alarme se fora do intervalo (21°C–25°C)
// e exibe os valores no display LCD.
// -------------------------------------------------------
void verificarTemperaturaEUmidade()
{
    float temperatura = dht.readTemperature();
    float umidade     = dht.readHumidity();

    if (isnan(temperatura) || isnan(umidade))
    {
        debugErro("Falha na leitura do sensor DHT.");
        return;
    }

    debugInfo("Temp: " + String(temperatura, 1) + "C | Umid: " + String(umidade, 1) + "%");

    // Converte para string e publica nos tópicos MQTT
    char tempStr[10], umidStr[10];
    dtostrf(temperatura, 4, 1, tempStr);
    dtostrf(umidade,     4, 1, umidStr);
    publicarMensagem(topic_temp, tempStr);
    publicarMensagem(topic_umid, umidStr);

    // Aciona alarme se temperatura fora do intervalo seguro
    if (temperatura <= 21.00 || temperatura >= 25.00)
        alarme_dois_tons();
    else
        noTone(buzzer);

    // Exibe leituras no display LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Umidade: " + String(umidade, 1) + "%");
    lcd.setCursor(0, 1);
    lcd.print("Temp: " + String(temperatura, 1) + "C");
    delay(2000);
}