#include "Componentes.h"

// ================= HARDWARE =================
int buzzer = BUZZER_PIN;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= TIMERS =================
static const unsigned long INTERVALO_MS = 3000;
static const unsigned long BUZZ_INTERVAL = 400;

// ================= ESTADO =================
static bool alarmeAtivo = false;
static bool buzzerState = false;

static unsigned long lastDHT = 0;
static unsigned long lastBuzz = 0;

// =====================================================
void setupComponentes()
{
    pinMode(buzzer, OUTPUT);
    digitalWrite(buzzer, LOW);

    dht.begin();

    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("Calibrando...");

    debugInfo("Calibrando sensores");

    for (int i = 0; i < 5; i++)
    {
        delay(200);
        debugInfo(".");
        yield();
    }

    lcd.clear();
    debugInfo("Sistema OK");
}

// =====================================================
// ALARME ESTAVEL (SEM tone / SEM LEDC)
// =====================================================
static void atualizarAlarme()
{
    if (!alarmeAtivo)
    {
        digitalWrite(buzzer, LOW);
        return;
    }

    if (millis() - lastBuzz < BUZZ_INTERVAL)
        return;

    lastBuzz = millis();

    buzzerState = !buzzerState;
    digitalWrite(buzzer, buzzerState);
}

// =====================================================
// LEITURA DHT ULTRA SEGURA (ANTI-CRASH)
// =====================================================
void verificarTemperaturaEUmidade()
{
    atualizarAlarme();

    if (millis() - lastDHT < INTERVALO_MS)
        return;

    lastDHT = millis();

    float t = NAN;
    float h = NAN;

    // 🔥 tentativa múltipla para evitar travamento do DHT
    for (int i = 0; i < 3; i++)
    {
        t = dht.readTemperature();
        h = dht.readHumidity();

        if (!isnan(t) && !isnan(h))
            break;

        delay(50);
        yield();
    }

    if (isnan(t) || isnan(h))
    {
        debugErro("Falha DHT (ignorado)");
        return;
    }

    debugInfo("T:" + String(t) + " H:" + String(h));

    char tStr[10], hStr[10];
    dtostrf(t, 4, 1, tStr);
    dtostrf(h, 4, 1, hStr);

    publicarMensagemNoTopico(INDICE_TOPICO_TEMPERATURA, tStr);
    publicarMensagemNoTopico(INDICE_TOPICO_UMIDADE, hStr);

    bool alerta = (t < TEMP_MIN || t > TEMP_MAX ||
                   h < UMID_MIN || h > UMID_MAX);

    if (alerta && !alarmeAtivo)
    {
        alarmeAtivo = true;
        buzzerState = false;

        debugErro("ALERTA ATIVADO");
        publicarMensagemNoTopico(INDICE_TOPICO_ALARME, "1");
    }
    else if (!alerta && alarmeAtivo)
    {
        alarmeAtivo = false;
        digitalWrite(buzzer, LOW);

        debugInfo("ALERTA DESATIVADO");
        publicarMensagemNoTopico(INDICE_TOPICO_ALARME, "0");
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:" + String(t, 1));

    lcd.setCursor(0, 1);
    lcd.print("H:" + String(h, 1));
}