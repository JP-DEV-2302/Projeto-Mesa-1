#ifndef DEBUG_MANAGER_H
#define DEBUG_MANAGER_H
#include <Arduino.h>

// Níveis de debug (do mais silencioso ao mais verboso)
#define DEBUG_NENHUM 0  // Nenhuma saída serial
#define DEBUG_ERRO   1  // Apenas erros
#define DEBUG_TUDO   2  // Erros e informações

// Inicializa o serial e define o nível de debug ativo
void configurarDebug();

// Imprime mensagem com prefixo [ERRO] e quebra de linha
void debugErro(const String& mensagem);

// Imprime mensagem com prefixo [INFO] e quebra de linha
void debugInfo(const String& mensagem);

// Versões sem quebra de linha (úteis para progresso em linha, ex: ".....")
void debugErroSemLinha(const String& mensagem);
void debugInfoSemLinha(const String& mensagem);

// Retorna o nível de debug atualmente ativo
int obterNivelDebugAtual();

#endif