#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "WiFiManager.h"
#include "secrets.h"
#include "DebugManager.h"

// Retorna true se o WiFi estiver conectado
bool wifiEstaConectado()
{
    return WiFi.status() == WL_CONNECTED;
}

// -------------------------------------------------------
// Conecta ao WiFi configurado em secrets.h.
// Tenta por até 30 vezes (15 segundos) antes de desistir.
// -------------------------------------------------------
void conectarWiFi()
{
    debugInfo("Iniciando conexao Wi-Fi...");

    // Modo station: conecta a um roteador existente
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_SENHA);

    int tentativas = 0;
    const int maxTentativas = 30;

    while (WiFi.status() != WL_CONNECTED && tentativas < maxTentativas)
    {
        delay(500);
        debugInfoSemLinha(".");
        tentativas++;
    }

    debugInfoSemLinha("\n\r");

    if (WiFi.status() == WL_CONNECTED)
    {
        debugInfo("Conectado ao Wi-Fi!");
        debugInfoSemLinha("[INFO] IP: ");
        debugInfoSemLinha(WiFi.localIP().toString());
        debugInfoSemLinha("\n\r");
    }
    else
    {
        debugErro("Falha ao conectar ao Wi-Fi. Verifique as credenciais.");
    }
}

// -------------------------------------------------------
// Garante que o WiFi está conectado, reconectando se necessário.
// Deve ser chamado periodicamente no loop principal.
// -------------------------------------------------------
void garantirWiFiConectado()
{
    if (!wifiEstaConectado())
    {
        debugErro("Wi-Fi desconectado. Tentando reconectar...");
        conectarWiFi();
    }

    if (!wifiEstaConectado())
    {
        debugErro("Nao foi possivel reconectar ao Wi-Fi.");
    }
}