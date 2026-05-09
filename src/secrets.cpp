/*
 * ============================================================
 * secrets.cpp
 * ------------------------------------------------------------
 * Define todas as credenciais, configurações de rede,
 * tópicos MQTT e parâmetros de comportamento do sistema.
 *
 * ATENÇÃO: Este arquivo contém dados sensíveis.
 * Adicione-o ao .gitignore para não versionar credenciais.
 *
 * As declarações (extern) estão em secrets.h.
 * ============================================================
 */

#include "secrets.h"
#include <Arduino.h>

// -------------------------------------------------------
// Credenciais da rede WiFi
// -------------------------------------------------------
const char* WIFI_SSID  = "SALA 09";    // Nome da rede (SSID)
const char* WIFI_SENHA = "info@134";   // Senha da rede

// -------------------------------------------------------
// Configuração do broker MQTT
// Usando HiveMQ Cloud com conexão TLS (porta 8883)
// -------------------------------------------------------
const char* MQTT_BROKER    = "adb9a2c2e8db40e785446b4911f449ed.s1.eu.hivemq.cloud";
const int   MQTT_PORTA     = 8883;          // 8883 = TLS | 1883 = sem TLS
const char* MQTT_CLIENT_ID = "Esp1Mesa1";  // ID único do ESP no broker
const char* MQTT_USUARIO   = "Coordenador";// Usuário de autenticação
const char* MQTT_SENHA     = "Senai@134";  // Senha de autenticação
const bool  MQTT_USAR_TLS  = true;         // Habilita conexão segura TLS

// -------------------------------------------------------
// Certificado CA (ISRG Root X1 — Let's Encrypt)
// Usado para validar a identidade do broker HiveMQ Cloud.
// Armazenado em PROGMEM para economizar RAM no ESP32.
// -------------------------------------------------------
const char MQTT_CERTIFICADO_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// -------------------------------------------------------
// Tópicos MQTT de PUBLICAÇÃO (ESP → broker)
// O ESP envia dados nesses tópicos.
// Os índices correspondem aos defines em Componentes.h:
//   [0] INDICE_TOPICO_LAMPADA      → comandoLampada
//   [1] INDICE_TOPICO_UMIDADE      → comandoUmidade
//   [2] INDICE_TOPICO_TEMPERATURA  → comandoTemperatura
//   [3] INDICE_TOPICO_ALARME       → statusAlarme ("1"=alerta, "0"=normal)
// -------------------------------------------------------
const char* TOPICOS_PUBLICAR[] = {
    "senai134/dev_01/Coordenador/esp32/comandoLampada",
    "senai134/dev_01/Coordenador/esp32/comandoUmidade",
    "senai134/dev_01/Coordenador/esp32/comandoTemperatura",
    "senai134/dev_01/Coordenador/esp32/statusAlarme"
};
const int TOTAL_TOPICOS_PUBLICAR = 4; // Deve sempre refletir o tamanho do array acima

// -------------------------------------------------------
// Tópicos MQTT de RECEBIMENTO (broker → ESP)
// O ESP assina esses tópicos para receber comandos.
// Usa wildcard "#" para escutar TODOS os subtópicos
// da hierarquia "senai134/dev_01/Coordenador/esp32/"
// em uma única assinatura.
// -------------------------------------------------------
const char* TOPICOS_RECEBER[] = {
    "senai134/dev_01/Coordenador/esp32/#"  // "#" = todos os subtópicos abaixo desse nível
};
const int TOTAL_TOPICOS_RECEBER = 1; // Deve sempre refletir o tamanho do array acima

// -------------------------------------------------------
// Flags de comportamento
// -------------------------------------------------------
const bool USAR_AWS_IOT = false; // false = usa HiveMQ | true = usa AWS IoT Core (não implementado)

// -------------------------------------------------------
// Configurações do sistema de debug serial
// -------------------------------------------------------
const int DEBUG_NIVEL_INICIAL           = 2; // 0=nenhum | 1=erros | 2=completo
const int PINO_HABILITA_DEBUG_COMPLETO  = 4; // GPIO4: se LOW no boot, força DEBUG_TUDO