#ifndef SECRETS_H
#define SECRETS_H

extern const char* WIFI_SSID;
extern const char* WIFI_SENHA;
extern const char* MQTT_BROKER;
extern const int   MQTT_PORTA;
extern const char* MQTT_CLIENT_ID;
extern const char* MQTT_USUARIO;
extern const char* MQTT_SENHA;
extern const bool  MQTT_USAR_TLS;
extern const char  MQTT_CERTIFICADO_CA[];
extern const char* TOPICOS_PUBLICAR[];
extern const int   TOTAL_TOPICOS_PUBLICAR;
extern const char* TOPICOS_RECEBER[];
extern const int   TOTAL_TOPICOS_RECEBER;
extern const bool  USAR_AWS_IOT;
extern const int   DEBUG_NIVEL_INICIAL;
extern const int   PINO_HABILITA_DEBUG_COMPLETO;

// --- Índices dos tópicos de publicação ---
#define TOPICO_IDX_STATUS      0
#define TOPICO_IDX_LOG         1
#define TOPICO_IDX_RESPOSTA    2
#define TOPICO_IDX_TEMPERATURA 3
#define TOPICO_IDX_UMIDADE     4

#endif