#ifndef SECRETS_H
#define SECRETS_H

// Credenciais WiFi
extern const char* WIFI_SSID;
extern const char* WIFI_SENHA;

// Configuração do broker MQTT
extern const char* MQTT_BROKER;
extern const int   MQTT_PORTA;
extern const char* MQTT_CLIENT_ID;
extern const char* MQTT_USUARIO;
extern const char* MQTT_SENHA;
extern const bool  MQTT_USAR_TLS;
extern const char  MQTT_CERTIFICADO_CA[];

// Tópicos MQTT
extern const char* TOPICOS_PUBLICAR[];
extern const int   TOTAL_TOPICOS_PUBLICAR;
extern const char* TOPICOS_RECEBER[];
extern const int   TOTAL_TOPICOS_RECEBER;

// Flags de comportamento
extern const bool USAR_AWS_IOT;

// Configuração de debug
extern const int DEBUG_NIVEL_INICIAL;
extern const int PINO_HABILITA_DEBUG_COMPLETO;

#endif