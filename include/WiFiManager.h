#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

// Retorna true se o WiFi estiver conectado
bool wifiEstaConectado();

// Conecta ao WiFi usando as credenciais definidas em secrets.h
void conectarWiFi();

// Reconecta ao WiFi se a conexão tiver caído. Chamar no loop().
void garantirWiFiConectado();

#endif