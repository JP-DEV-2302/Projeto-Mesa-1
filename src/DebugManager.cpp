#include <Arduino.h>
#include "DebugManager.h"
#include "secrets.h"

// Nível de debug ativo (ajustado em configurarDebug())
int nivelDebugAtual = DEBUG_NIVEL_INICIAL;

// Imprime mensagem de erro com prefixo [ERRO]
void debugErro(const String& mensagem)
{
    if(nivelDebugAtual >= DEBUG_ERRO)
    {
        Serial.print("[ERRO] ");
        Serial.println(mensagem);
    }
}

// Imprime mensagem de erro sem quebra de linha
void debugErroSemLinha(const String& mensagem)
{
    if(nivelDebugAtual >= DEBUG_ERRO)
    {
        Serial.print(mensagem);
    }
}

// Imprime mensagem informativa com prefixo [INFO]
void debugInfo(const String& mensagem)
{
    if(nivelDebugAtual >= DEBUG_TUDO)
    {
        Serial.print("[INFO] ");
        Serial.println(mensagem);
    }
}

// Imprime mensagem informativa sem quebra de linha
void debugInfoSemLinha(const String& mensagem)
{
    if(nivelDebugAtual >= DEBUG_TUDO)
    {
        Serial.print(mensagem);
    }
}

// -------------------------------------------------------
// Inicializa o sistema de debug.
// Se o pino PINO_HABILITA_DEBUG_COMPLETO estiver em LOW,
// força o nível máximo independente do secrets.h.
// -------------------------------------------------------
void configurarDebug()
{
    Serial.begin(9600);
    delay(500);

    // Pino físico para forçar debug completo em tempo de execução
    pinMode(PINO_HABILITA_DEBUG_COMPLETO, INPUT_PULLUP);
    nivelDebugAtual = (digitalRead(PINO_HABILITA_DEBUG_COMPLETO) == LOW)
        ? DEBUG_TUDO
        : DEBUG_NIVEL_INICIAL;

    debugInfo("ESP32 iniciado.");
    debugInfo("Modo de debug: " + String(nivelDebugAtual == DEBUG_ERRO ? "apenas erros" : "completo"));
}