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
 * Implementar o loop de mineração para os blocos 2 até 30.000
 * Desenvolver a lógica de prova de trabalho (Proof of Work):
 * Incrementar o nonce
 * Calcular o hash SHA-256
 * Verificar se começa com dois zeros hexadecimais
 * Atualizar a carteira com a recompensa de 50 BTC para o minerador
 * Garantir que o hashAnterior aponte para o bloco anterior
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
#define TOTAL_BLOCOS 30000

// Estrutura do Bloco Não Minerado
typedef struct BlocoNaoMinerado {
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

    // ============================================================================
    // 4. Mineração do Restante dos Blocos
    // ============================================================================

    BlocoMinerado blocoAnterior = blocoGenesis;
    BlocoMinerado blocoAtual;

    for (int i = 2; i <= TOTAL_BLOCOS; ++i) {
        blocoAtual.bloco.numero = i;
        blocoAtual.bloco.nonce = 0;

        // hashAnterior recebe o hash do bloco anterior
        memcpy(blocoAtual.bloco.hashAnterior, blocoAnterior.hash, SHA256_DIGEST_LENGTH);
        memset(blocoAtual.bloco.data, 0, DATA_SIZE_BYTES);

        // quantidade de transações: mín 0 máx 61
        int transacoes = genRandLong(&r) % 62;

        int pos = 0; // índice no vetor data

        for (int t = 0; t < transacoes; t++) {
            int origem;
            int tentativas = 0;
            // 1000 tentativas de achar endereço com saldo
            do{
                origem = genRandLong(&r) % 256;
                tentativas++;
                if(tentativas > 1000) break; // nenhum endereco com saldo 
            }while(carteira[origem] == 0);

            if(carteira[origem] == 0) break;

            int destino = genRandLong(&r) % 256;
            int maxBTC = carteira[origem];
            // qnt recebe 0 se o end de origem possuir 0 bitcoins (não há o que enviar)
            // senão, gera uma quantidade aleatória de bitcoins a ser enviada do saldo
            // maxBTC + 1 pois há a possibilidade de enviar tudo o que tem no saldo
            int qnt = (maxBTC == 0 ? 0 : (genRandLong(&r) % (maxBTC + 1)));

            // transação tirando da origem e levando ao destino
            carteira[origem] -= qnt;
            carteira[destino] += qnt;

            // insere no bloco (ex: 10 20 30 - origem 10, destino 20, qtd de bitcoins 30)
            // ou seja, preenchimento do vetor data com as transações
            blocoAtual.bloco.data[pos++] = (unsigned char) origem;
            blocoAtual.bloco.data[pos++] = (unsigned char) destino;
            blocoAtual.bloco.data[pos++] = (unsigned char) qnt;

            // pare de minerar o bloco quando o vetor data for totalmente preenchido ate a posição 182 (183 é para o minerador)
            if (pos >= DATA_SIZE_BYTES - 1) 
                break;
        }

        // escolhe minerador dentre os 256 e insere no final do vetor data
        unsigned char minerador = (unsigned char)(genRandLong(&r) % 256);
        blocoAtual.bloco.data[183] = minerador;

        // validação da mineração de um bloco 
        int valido = 0;
        while(!valido){
            SHA256((unsigned char*)&blocoAtual.bloco, sizeof(BlocoNaoMinerado), blocoAtual.hash);
            if(blocoAtual.hash[0] == 0) // 2 zeros hexadecimais = 1 byte
                valido = 1;

            else
                blocoAtual.bloco.nonce++;
        }

        // recompensa da mineração
        carteira[minerador] += RECOMPENSA_MINERACAO;

        // bloco anterior atualizado
        blocoAnterior = blocoAtual;

        // exibição dos blocos 1000, 2000, ...
        if (i % 1000 == 0) {
            printf("Bloco %d minerado. Nonce = %u Hash = ", i, blocoAtual.bloco.nonce);
            printHash(blocoAtual.hash);
        }
    }

    return 0;
}