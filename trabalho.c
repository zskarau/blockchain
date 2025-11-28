/*
 * Definir as estruturas de dados principais:
 * BlocoNaoMinerado
 * BlocoMinerado
 * carteira[256]
 * Implementar a criação do Bloco Gênesis (bloco 1), incluindo:
 * Preenchimento do campo data com a string específica
 * Geração aleatória do minerador
 * Cálculo do hash do bloco
 * Inicialização da carteira com saldos zerados
 */

/*
 * Compilação:
 *
 * Compilação Padrão:
 * gcc trabalho.c mtwister.c -o trabalho.out -lssl -lcrypto
 * 
 * 2. Compilação Otimizada (-Ofast):
 * gcc trabalho.c mtwister.c -o trabalho.out -Ofast -lssl -lcrypto
 * 
 * 3. Compilação Máxima Otimização (-Ofast -march=native -flto):
 * gcc trabalho.c mtwister.c -o trabalho.out -Ofast -march=native -flto -lssl -lcrypto
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include "mtwister.h"

// ============================================================================
// 1. Definição das Estruturas de Dados
// ============================================================================

// Tamanho do hash SHA-256 em bytes (32 bytes)
#ifndef SHA256_DIGEST_LENGTH
#define SHA256_DIGEST_LENGTH 32
#endif

#define RECOMPENSA_MINERACAO 50
#define DATA_SIZE_BYTES 184
#define CARTEIRA_SIZE 256

// Estrutura do Bloco Não Minerado
typedef struct {
    unsigned int numero;    // Número sequencial do bloco
    unsigned int nonce;     // Varia de 0 a 2^32 - 1
    unsigned char data[DATA_SIZE_BYTES]; // Dados das transações + endereço do minerador
    unsigned char hashAnterior[SHA256_DIGEST_LENGTH];   // Hash do bloco anterior
} BlocoNaoMinerado;

// Estrutura do Bloco Minerado
typedef struct BlocoMinerado {
    BlocoNaoMinerado bloco;
    unsigned char hash[SHA256_DIGEST_LENGTH]; // O hash válido que começa com zeros
} BlocoMinerado;

// Carteira do Sistema: saldo dos 256 endereços
// Fora da main por conveniência.
unsigned int carteira[CARTEIRA_SIZE];

// ============================================================================
// 2. Funções Auxiliares
// ============================================================================

// Função para imprimir o hash em hexadecimal (para visualização)
void printHash(unsigned char hash[]) {
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

// Inicializa a carteira com zeros
void inicializarCarteira() {
    for (int i = 0; i < 256; i++) {
        carteira[i] = 0;
    }
}

// ============================================================================
// 3. Lógica do Bloco Gênesis
// ============================================================================

int main() {
    // Inicialização da semente do gerador aleatório conforme exigido
    MTRand r = seedRand(1234567);

    // Inicializa a carteira do sistema zerada
    inicializarCarteira();

    // Criação da variável para o Bloco Gênesis
    BlocoMinerado blocoGenesis;

    // --- Preenchimento dos Campos do Bloco Gênesis ---

    // 1. Número sequencial do bloco começa em 1
    blocoGenesis.bloco.numero = 1;

    // 2. Hash anterior deve ser preenchido com zeros para o bloco 1
    memset(blocoGenesis.bloco.hashAnterior, 0, SHA256_DIGEST_LENGTH);

    // 3. Preenchimento do campo data
    // Limpa o vetor data primeiro
    memset(blocoGenesis.bloco.data, 0, sizeof(blocoGenesis.bloco.data));

    // Copia a string obrigatória
    const char *genesisString = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    strcpy((char*)blocoGenesis.bloco.data, genesisString);

    // 4. Definição do Minerador Aleatório
    // O endereço deve ser escolhido aleatoriamente [0, 255]
    unsigned char enderecoMinerador = (unsigned char)(genRandLong(&r) % 256);

    // O minerador é armazenado na última posição do vetor data (índice 183)
    blocoGenesis.bloco.data[183] = enderecoMinerador;

    // --- Mineração (Cálculo do Hash) ---
    
    // O nonce começa em 0
    blocoGenesis.bloco.nonce = 0;
    
    int minerado = 0;

    printf("Iniciando mineracao do Bloco Genesis...\n");

    while (!minerado) {
        // Calcula o SHA-256 do bloco não minerado inteiro
        SHA256((unsigned char*)&blocoGenesis.bloco, sizeof(BlocoNaoMinerado), blocoGenesis.hash);

        // Verifica a condição de validade: Hash deve começar com dois zeros hexadecimais 
        // Dois zeros hexadecimais (00) equivalem ao primeiro byte ser 0 
        if (blocoGenesis.hash[0] == 0) {
            minerado = 1;
        } else {
            // Se não validou, incrementa o nonce e tenta de novo
            blocoGenesis.bloco.nonce++;
        }
    }

    // --- Atualização da Carteira ---

    // Após mineração, o minerador recebe 50 bitcoins de recompensa
    carteira[enderecoMinerador] += RECOMPENSA_MINERACAO;

    // --- Exibição dos Resultados ---
    printf("\n--- Bloco Genesis Minerado com Sucesso ---\n");
    printf("Nonce encontrado: %u\n", blocoGenesis.bloco.nonce);
    printf("Hash do Bloco: ");
    printHash(blocoGenesis.hash);
    printf("Saldo da Carteira [%d]: %d BTC\n", enderecoMinerador, carteira[enderecoMinerador]);

    return 0;
}