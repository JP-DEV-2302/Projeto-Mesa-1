#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "WiFiManager.h"
#include "MqttManager.h"
#include "secrets.h"
#include "DebugManager.h"

WiFiClient wifiCliente;
WiFiClientSecure wifiClientSecure;
PubSubClient mqttClient;
CallbackMensagemMQTT callbackDaAplicacao = nullptr;

void registrarCallbackMensagem(CallbackMensagemMQTT callback)
{
    callbackDaAplicacao = callback;
    if(callbackDaAplicacao != nullptr)
    {
        debugInfo("Callback da aplicacao registrada com sucesso.");
    }
    else
    {
        debugErro("Callback da aplicacao nao foi registrada.");
    }
}

const char* obterTopicoPublicacao(int indiceTopico)
{
    if(indiceTopico < 0 || indiceTopico >= TOTAL_TOPICOS_PUBLICAR)
    {
        debugErro("Indice invalido para topico de publicacao: " + String(indiceTopico));
        return "";
    }
    return TOPICOS_PUBLICAR[indiceTopico];
}

const char* obterTopicoRecebimento(int indiceTopico)
{
    if(indiceTopico < 0 || indiceTopico >= TOTAL_TOPICOS_RECEBER)
    {
        debugErro("Indice invalido para topico de recebimento: " + String(indiceTopico));
        return "";
    }
    return TOPICOS_RECEBER[indiceTopico];
}

void callbackInternoMQTT(char* topico, byte* payload, unsigned int tamanho)
{
    String mensagem = "";
    for(unsigned int i = 0; i < tamanho; i++)
    {
        mensagem += (char)payload[i];
    }
    debugInfo("======================");
    debugInfo("Mensagem MQTT recebida");
    debugInfo("======================");
    debugInfo("Topico: " + String(topico));
    debugInfo("mensagem: " + mensagem);
    if(callbackDaAplicacao != nullptr)
    {
        callbackDaAplicacao(topico, mensagem);
    }
    else
    {
        debugErro("Mensagem recebida, porem nenhum callback da aplicacao foi registrado.");
    }
}

void configurarMQTT()
{
    debugInfo("======================");
    debugInfo(" Configurando MQTT... ");
    debugInfo("======================");
    if(USAR_AWS_IOT)
    {

    }
    else if(MQTT_USAR_TLS)
    {
        debugInfo("Modo selecionado: MQTT com TLS.");
        if(strlen(MQTT_CERTIFICADO_CA) > 100)
        {
            debugInfo("Certificado CA do broker MQTT configurado.");
            wifiClientSecure.setCACert(MQTT_CERTIFICADO_CA);
        }
        else
        {
            debugErro("Certificado CA do MQTT nao configurado. Usando setInsecure apenas para teste.");
            wifiClientSecure.setInsecure();
        }
        mqttClient.setClient(wifiClientSecure);
        mqttClient.setServer(MQTT_BROKER, MQTT_PORTA);
        debugInfo("Broker MQTT: " + String(MQTT_BROKER));
        debugInfo("Porta MQTT: " + String(MQTT_PORTA));
    }
    else 
    {
        debugInfo("Modo selecionado: MQTT sem TLS.");
        mqttClient.setClient(wifiCliente);
        mqttClient.setServer(MQTT_BROKER, MQTT_PORTA);
        debugInfo("Broker MQTT: " + String(MQTT_BROKER));
        debugInfo("Porta MQTT: " + String(MQTT_PORTA));
    }

    mqttClient.setCallback(callbackInternoMQTT);
    debugInfo("Callback interno do MQTT configurado.");
}

void conectarMQTT()
{
    if(!wifiEstaConectado())
    {
        debugErro("MQTT nao pode conectar porque o WiFi esta desconectado.");
    }
    debugInfo("======================");
    debugInfo("Iniciando conexao MQTT...");
    debugInfo("======================");
    int tentativasMQTT = 0;
    const int maxTentativasMQTT = 5;
    while(!mqttClient.connected() && tentativasMQTT < maxTentativasMQTT)
    {
        debugInfo("Tentando conectar ao broker MQTT. Tentativas: " + String(tentativasMQTT));
        bool conectado = false;

        if(USAR_AWS_IOT)
        {

        }
        else
        {
            if(strlen(MQTT_USUARIO) > 0)
            {
                debugInfo("Conectando MQTT com usuario e senha.");
                conectado = mqttClient.connect(MQTT_CLIENT_ID, MQTT_USUARIO, MQTT_SENHA);
            }
            else 
            {
                debugInfo("Conectando MQTT sem usuario e senha.");
                conectado = mqttClient.connect(MQTT_CLIENT_ID);
            }
        }
        if(conectado)
        {
            debugInfo("MQTT conectado com sucesso.");
            int totalTopicos = obterTotalTopicosRecebimento();
            debugInfo("Total de topicos para isncricao: " + String(totalTopicos));
            for(int i = 0; i < totalTopicos; i++)
            {
                const char* topico = obterTopicoRecebimento(i);
                bool inscrito = mqttClient.subscribe(topico);
                if(inscrito)
                {
                    debugInfo("Inscrito no topico: " + String(topico));
                }
                else
                {
                    debugErro("Falha ao se inscrever no topico: " + String(topico));
                }
            }
        }
        else
        {
            debugErro("Falha ao conectar no MQTT. COdigo de erro: " + String(mqttClient.state()));
            tentativasMQTT++;
            delay(2000);
        }
    }
    if(!mqttClient.connected())
    {
        debugErro("Nao foi possivel conectar ao broker MQTT apos " + String(maxTentativasMQTT) + " tentativas.");
    }
}

void garantirMQTTConectado()
{
    if(!wifiEstaConectado())
    {
        debugErro("MQTT nao sera reconectatdo porque o WiFi esta desconectado.");
        return;
    }
    if(!mqttClient.connected())
    {
        debugErro("MQTT desconectado. Tentando reconectar...");
        conectarMQTT();
    }
}

void loopMQTT()
{
    mqttClient.loop();
}

void publicarMensagem(const char* topico, const char* mensagem)
{
    if(!mqttClient.connected())
    {
        debugErro("Nao foi possivel publicar. MQTT desconectado.");
        return;
    }
    bool publicado = mqttClient.publish(topico, mensagem);
    if(publicado)
    {
        debugInfo("Mensagem publicada via MQTT.");
        debugInfo("Topico: " + String(topico));
        debugInfo("Mensagem: " + String(mensagem));
    }
    else
    {
        debugErro("Falha ao publicar mensagem no topico: " + String(topico));
    }
}

void publicarMensagemNoTopico(int indiceTopico, const char* mensagem)
{
    const char* topico = obterTopicoPublicacao(indiceTopico);
    if(strlen(topico) == 0)
    {
        debugErro("Nao foi possivel publicar. Indice de topico invalido: " + String(indiceTopico));
        return;
    }
    publicarMensagem(topico, mensagem);
}

bool mqttEstaConectado()
{
    return mqttClient.connected();
}

int obterTotalTopicosRecebimento()
{
    return TOTAL_TOPICOS_RECEBER;
}