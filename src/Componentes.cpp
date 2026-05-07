#include "Componentes.h"

// --- Tópicos MQTT de temperatura e umidade ---
const char* topic_umid = "senai134/dev_01/Coordenador/esp32/StatusSensorUmidade"; // ← ajuste
const char* topic_temp = "senai134/dev_01/Coordenador/esp32/statusSensorTemperatura";     // ← ajuste

// --- Variáveis de hardware ---
int              buzzer     = BUZZER_PIN;

DHT              dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // ← mude para 0x3F se o display não ligar

// -------------------------------------------------------

void setupComponentes() {
  pinMode(buzzer,     OUTPUT);
 

  dht.begin();
  lcd.init();
  lcd.backlight();

 

  debugInfo("Calibrando os sensores");
  for (int i = 0; i < 5; i++) {
    delay(1500);
    debugInfoSemLinha(".");
  }
  delay(1500);
  debugInfoSemLinha("\n\r");
  debugInfo("Sensores calibrados!!!");
  debugInfo("  *TESTE LIBERADO*");
}

// -------------------------------------------------------

void alarme_dois_tons() {
  const int freqAlta   = 2000;
  const int freqBaixa  = 800;
  const int duracaoTom = 300;

  for (int i = 0; i < 2; i++) {
    tone(buzzer, freqAlta,  duracaoTom);
    tone(buzzer, freqBaixa, duracaoTom);
    delay(500);
    tone(buzzer, freqAlta,  duracaoTom);
    tone(buzzer, freqBaixa, duracaoTom);
  }
  noTone(buzzer);
}

// -------------------------------------------------------

void verificarTemperaturaEUmidade() {
  float temperatura = dht.readTemperature();
  float umidade     = dht.readHumidity();

  if (isnan(temperatura) || isnan(umidade)) {
    debugErro("Erro ao ler DHT!");
    return;
  }

  debugInfo("Temp: " + String(temperatura, 1) + " C | Umid: " + String(umidade, 1) + " %");

  char tempStr[10], umidStr[10];
  dtostrf(temperatura, 4, 1, tempStr);
  dtostrf(umidade,     4, 1, umidStr);

  // ⚠️ Se MqttManager expõe uma função de publish (ex: publicarMensagem()),
  // prefira usá-la no lugar de client.publish() direto.
  publicarMensagem(topic_temp, tempStr); // ← verifique o nome em MqttManager.h
  publicarMensagem(topic_umid, umidStr);

  if (temperatura <= 21.00 || temperatura >= 25.00) {
    alarme_dois_tons();
  } else {
    noTone(buzzer);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Umidade: " + String(umidade, 1) + "%");
  lcd.setCursor(0, 1);
  lcd.print("Temp: " + String(temperatura, 1) + "C");
  delay(2000);
}