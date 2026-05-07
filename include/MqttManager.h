#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H
#include <Arduino.h>

// Tipo do callback chamado ao receber uma mensagem MQTT
typedef void (*CallbackMensagemMQTT)(const char* topico, const String& mensagem);

// Configuração e conexão
void configurarMQTT();
void conectarMQTT();
void garantirMQTTConectado();  // Chamar no loop()
void loopMQTT();               // Processar mensagens recebidas — chamar no loop()

// Publicação de mensagens
void publicarMensagem(const char* topico, const char* mensagem);
void publicarMensagemNoTopico(int indiceTopico, const char* mensagem);

// Consulta de tópicos configurados em secrets.h
const char* obterTopicoPublicacao(int indiceTopico);
const char* obterTopicoRecebimento(int indiceTopico);
int         obterTotalTopicosRecebimento();

// Registro do callback da aplicação para mensagens recebidas
void registrarCallbackMensagem(CallbackMensagemMQTT callback);

// Status da conexão
bool mqttEstaConectado();

#endif