#include "Componentes.h"

// --- Hardware ---
int               buzzer = BUZZER_PIN;
DHT               dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // ← mude para 0x3F se o display não ligar

// --- Temporização ---
static const unsigned long INTERVALO_LEITURA_MS = 3000UL; // intervalo entre leituras
static const unsigned long DURACAO_TOM_MS       = 500UL;  // duração de cada tom do alarme

// --- Estado do alarme (não-bloqueante) ---
static bool          alarmeAtivo    = false;
static int           alarmeEtapa    = 0;   // 0 = tom alto, 1 = tom baixo
static unsigned long alarmeUltimoMs = 0;

static const int FREQ_ALTA  = 2000;
static const int FREQ_BAIXA = 800;

// -------------------------------------------------------
// Inicializa todos os componentes físicos.
// WiFi e MQTT são configurados no main.cpp — não repetir aqui.
// -------------------------------------------------------
void setupComponentes()
{
    pinMode(buzzer, OUTPUT);
    dht.begin();
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("Calibrando...");

    debugInfo("Calibrando sensores");
    for (int i = 0; i < 5; i++) {
        delay(1000);
        debugInfoSemLinha(".");
    }
    debugInfoSemLinha("\n\r");
    debugInfo("Sensores calibrados.");
    debugInfo("*TESTE LIBERADO*");

    lcd.clear();
}

// -------------------------------------------------------
// Atualiza o alarme sonoro de forma NÃO-BLOQUEANTE.
// Alterna entre tom alto e baixo a cada DURACAO_TOM_MS.
// Deve ser chamado a cada iteração do loop().
// -------------------------------------------------------
static void atualizarAlarme()
{
    if (!alarmeAtivo) {
        noTone(buzzer);
        return;
    }

    unsigned long agora = millis();
    if (agora - alarmeUltimoMs < DURACAO_TOM_MS) return;

    alarmeUltimoMs = agora;
    alarmeEtapa    = (alarmeEtapa + 1) % 2; // alterna entre 0 e 1

    tone(buzzer, alarmeEtapa == 0 ? FREQ_ALTA : FREQ_BAIXA);
}

// -------------------------------------------------------
// Lê temperatura e umidade do DHT, publica via MQTT,
// aciona alarme se fora das faixas seguras e exibe no LCD.
//
// Tudo não-bloqueante: usa millis() no lugar de delay().
// O alarme só dispara/desliga na TRANSIÇÃO de estado,
// evitando acionamentos repetidos a cada leitura.
// -------------------------------------------------------
void verificarTemperaturaEUmidade()
{
    static unsigned long ultimaLeitura = 0;
    unsigned long agora = millis();

    // Mantém o alarme atualizado independente do intervalo de leitura
    atualizarAlarme();

    if (agora - ultimaLeitura < INTERVALO_LEITURA_MS) return;
    ultimaLeitura = agora;

    // --- Leitura do sensor ---
    static float ultimaTemp          = NAN;
    static float ultimaUmid          = NAN;
    static int   falhasConsecutivas  = 0;

    float temperatura = dht.readTemperature();
    float umidade     = dht.readHumidity();

    if (isnan(temperatura) || isnan(umidade)) {
        falhasConsecutivas++;

        // Loga apenas nas primeiras ocorrências para não poluir o serial
        if (falhasConsecutivas <= MAX_FALHAS_DHT)
            debugErro("Falha na leitura do DHT. (" + String(falhasConsecutivas) + "x consecutiva)");

        // Exibe último valor conhecido no LCD com indicador de erro
        if (!isnan(ultimaTemp)) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("T:" + String(ultimaTemp, 1) + "C ?ERRO");
            lcd.setCursor(0, 1);
            lcd.print("U:" + String(ultimaUmid, 1) + "% ?ERRO");
        }
        return;
    }

    // Leitura bem-sucedida — zera contador e salva valores
    if (falhasConsecutivas > 0)
        debugInfo("DHT recuperado apos " + String(falhasConsecutivas) + " falha(s).");

    falhasConsecutivas = 0;
    ultimaTemp         = temperatura;
    ultimaUmid         = umidade;

    debugInfo("Temp: " + String(temperatura, 1) + "C | Umid: " + String(umidade, 1) + "%");

    // --- Publicação MQTT usando índices de secrets.cpp ---
    char tempStr[10], umidStr[10];
    dtostrf(temperatura, 4, 1, tempStr);
    dtostrf(umidade,     4, 1, umidStr);
    publicarMensagemNoTopico(INDICE_TOPICO_TEMPERATURA, tempStr);
    publicarMensagemNoTopico(INDICE_TOPICO_UMIDADE,     umidStr);

    // --- Verificação das faixas seguras ---
    bool tempForaFaixa = (temperatura < TEMP_MIN || temperatura > TEMP_MAX);
    bool umidForaFaixa = (umidade     < UMID_MIN || umidade     > UMID_MAX);
    bool foraFaixa     = tempForaFaixa || umidForaFaixa;

    // Só age na transição normal → alarme e alarme → normal
    if (foraFaixa && !alarmeAtivo) {
        alarmeAtivo    = true;
        alarmeEtapa    = 0;
        alarmeUltimoMs = 0; // dispara imediatamente

        debugErro("ALERTA: valores fora da faixa segura!");
        if (tempForaFaixa)
            debugErro("Temperatura: " + String(temperatura, 1) + "C (faixa: 21-27C)");
        if (umidForaFaixa)
            debugErro("Umidade: " + String(umidade, 1) + "% (faixa: 45-60%)");

        publicarMensagemNoTopico(INDICE_TOPICO_ALARME, "1");
    }
    else if (!foraFaixa && alarmeAtivo) {
        alarmeAtivo = false;
        noTone(buzzer);
        debugInfo("Valores normalizados. Alarme desativado.");

        publicarMensagemNoTopico(INDICE_TOPICO_ALARME, "0");
    }

    // --- Atualização do LCD ---
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:" + String(temperatura, 1) + "C");
    if (tempForaFaixa) lcd.print(" ALERTA!");

    lcd.setCursor(0, 1);
    lcd.print("U:" + String(umidade, 1) + "%");
    if (umidForaFaixa) lcd.print(" ALERTA!");
}