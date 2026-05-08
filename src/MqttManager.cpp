#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "WiFiManager.h"
#include "MqttManager.h"
#include "secrets.h"
#include "DebugManager.h"

// Clientes de rede: um sem TLS e outro com TLS (conexão segura e constante)
WiFiClient wifiCliente;
WiFiClientSecure wifiClientSecure;

// Cliente MQTT que usará um dos clientes acima
PubSubClient mqttClient;

// Ponteiro para a função de callback definida pela aplicação
CallbackMensagemMQTT callbackDaAplicacao = nullptr;

// -------------------------------------------------------
// Registra a função que será chamada ao receber mensagens
// -------------------------------------------------------
void registrarCallbackMensagem(CallbackMensagemMQTT callback)
{
    callbackDaAplicacao = callback;
    if(callbackDaAplicacao != nullptr)
        debugInfo("Callback da aplicacao registrada com sucesso.");
    else
        debugErro("Callback da aplicacao nao foi registrada.");
}

// -------------------------------------------------------
// Retorna o tópico de publicação pelo índice
// -------------------------------------------------------
const char* obterTopicoPublicacao(int indiceTopico)
{
    if(indiceTopico < 0 || indiceTopico >= TOTAL_TOPICOS_PUBLICAR)
    {
        debugErro("Indice invalido para topico de publicacao: " + String(indiceTopico));
        return "";
    }
    return TOPICOS_PUBLICAR[indiceTopico];
}

// -------------------------------------------------------
// Retorna o tópico de recebimento pelo índice
// -------------------------------------------------------
const char* obterTopicoRecebimento(int indiceTopico)
{
    if(indiceTopico < 0 || indiceTopico >= TOTAL_TOPICOS_RECEBER)
    {
        debugErro("Indice invalido para topico de recebimento: " + String(indiceTopico));
        return "";
    }
    return TOPICOS_RECEBER[indiceTopico];
}

// -------------------------------------------------------
// Callback interno chamado pela biblioteca PubSubClient
// ao chegar uma mensagem. Converte o payload em String
// e repassa para o callback da aplicação.
// -------------------------------------------------------
void callbackInternoMQTT(char* topico, byte* payload, unsigned int tamanho)
{
    // Converte o payload (bytes) para String
    String mensagem = "";
    for(unsigned int i = 0; i < tamanho; i++)
        mensagem += (char)payload[i];

    debugInfo("Topico: " + String(topico));
    debugInfo("Mensagem: " + mensagem);

    // Repassa para o callback da aplicação, se registrado
    if(callbackDaAplicacao != nullptr)
        callbackDaAplicacao(topico, mensagem);
    else
        debugErro("Mensagem recebida, porem nenhum callback da aplicacao foi registrado.");
}

// -------------------------------------------------------
// Configura o cliente MQTT de acordo com o modo escolhido:
//   - AWS IoT (a implementar)
//   - MQTT com TLS
//   - MQTT sem TLS
// -------------------------------------------------------
void configurarMQTT()
{
    debugInfo("Configurando MQTT...");

    if(USAR_AWS_IOT)
    {
        // TODO: implementar configuração para AWS IoT
    }
    else if(MQTT_USAR_TLS)
    {
        debugInfo("Modo: MQTT com TLS.");

        // Configura o certificado CA, ou usa modo inseguro para testes
        if(strlen(MQTT_CERTIFICADO_CA) > 100)
            wifiClientSecure.setCACert(MQTT_CERTIFICADO_CA);
        else
        {
            debugErro("Certificado CA ausente. Usando setInsecure (somente para testes).");
            wifiClientSecure.setInsecure();
        }

        mqttClient.setClient(wifiClientSecure);
    }
    else
    {
        debugInfo("Modo: MQTT sem TLS.");
        mqttClient.setClient(wifiCliente);
    }

    // Comum aos modos com e sem TLS
    mqttClient.setServer(MQTT_BROKER, MQTT_PORTA);
    mqttClient.setCallback(callbackInternoMQTT);

    debugInfo("Broker: " + String(MQTT_BROKER) + " | Porta: " + String(MQTT_PORTA));
}

// -------------------------------------------------------
// Conecta ao broker MQTT e se inscreve nos tópicos.
// Tenta até maxTentativasMQTT vezes antes de desistir.
// -------------------------------------------------------
void conectarMQTT()
{
    if(!wifiEstaConectado())
    {
        debugErro("WiFi desconectado. Conexao MQTT abortada.");
        return;
    }

    const int maxTentativas = 5;
    int tentativas = 0;

    while(!mqttClient.connected() && tentativas < maxTentativas)
    {
        debugInfo("Tentativa " + String(tentativas + 1) + " de conectar ao broker MQTT...");

        bool conectado = false;

        if(USAR_AWS_IOT)
        {
            // TODO: implementar conexão para AWS IoT
        }
        else
        {
            // Conecta com ou sem autenticação, conforme configurado
            conectado = (strlen(MQTT_USUARIO) > 0)
                ? mqttClient.connect(MQTT_CLIENT_ID, MQTT_USUARIO, MQTT_SENHA)
                : mqttClient.connect(MQTT_CLIENT_ID);
        }

        if(conectado)
        {
            debugInfo("MQTT conectado.");

            // Inscreve em todos os tópicos de recebimento configurados
            int total = obterTotalTopicosRecebimento();
            for(int i = 0; i < total; i++)
            {
                const char* topico = obterTopicoRecebimento(i);
                if(mqttClient.subscribe(topico))
                    debugInfo("Inscrito em: " + String(topico));
                else
                    debugErro("Falha ao se inscrever em: " + String(topico));
            }
        }
        else
        {
            debugErro("Falha na conexao. Codigo: " + String(mqttClient.state()));
            tentativas++;
            delay(2000); // Aguarda 2s antes de tentar novamente
        }
    }

    if(!mqttClient.connected())
        debugErro("Nao foi possivel conectar ao broker apos " + String(maxTentativas) + " tentativas.");
}

// -------------------------------------------------------
// Garante que o MQTT está conectado, reconectando se necessário.
// Deve ser chamado periodicamente no loop principal.
// -------------------------------------------------------
void garantirMQTTConectado()
{
    if(!wifiEstaConectado())
    {
        debugErro("WiFi desconectado. Reconexao MQTT ignorada.");
        return;
    }
    if(!mqttClient.connected())
    {
        debugErro("MQTT desconectado. Reconectando...");
        conectarMQTT();
    }
}

// -------------------------------------------------------
// Processa mensagens recebidas. Deve ser chamado no loop().
// -------------------------------------------------------
void loopMQTT()
{
    mqttClient.loop();
}

// -------------------------------------------------------
// Publica uma mensagem em um tópico específico (por string).
// -------------------------------------------------------
void publicarMensagem(const char* topico, const char* mensagem)
{
    if(!mqttClient.connected())
    {
        debugErro("MQTT desconectado. Publicacao abortada.");
        return;
    }

    if(mqttClient.publish(topico, mensagem))
    {
        debugInfo("Publicado em [" + String(topico) + "]: " + String(mensagem));
    }
    else
    {
        debugErro("Falha ao publicar em: " + String(topico));
    }
}

// -------------------------------------------------------
// Publica uma mensagem usando o índice do tópico configurado.
// -------------------------------------------------------
void publicarMensagemNoTopico(int indiceTopico, const char* mensagem)
{
    const char* topico = obterTopicoPublicacao(indiceTopico);
    if(strlen(topico) == 0)
    {
        debugErro("Indice de topico invalido: " + String(indiceTopico));
        return;
    }
    publicarMensagem(topico, mensagem);
}

// Retorna true se o cliente MQTT estiver conectado
bool mqttEstaConectado()
{
    return mqttClient.connected();
}

// Retorna o total de tópicos de recebimento configurados
int obterTotalTopicosRecebimento()
{
    return TOTAL_TOPICOS_RECEBER;
}