#include <stdio.h>
#include "mtwister.h"
#define DATA_SIZE 1008
#define QTD_BLOCOS 100 //qtd de blocos de 4096 bytes da simulacao
#define QTD_REGISTROS_BLOCO 4

//criando struct de 8+8+1008 = 1024 bytes
//ou seja cada escrita/leitura no arquivo será de 4 registros
//pois o arquivo guarda 4096 bytes por vez (4096/1024 = 4)
typedef struct TRegistro
{
	unsigned long chave;	//8 bytes
	unsigned long naochave; //8 bytes
	unsigned char data[DATA_SIZE]; //1008 bytes de dados para a struct ter 1024 bytes
}TRegistro;

int main()
{
	FILE *fArq = fopen("exemplo.txt", "w+");
	if(!fArq){
		perror("Arquivo não abriu\n");
		return -1;
	}

	MTRand r = seedRand(1234567);
	TRegistro bufferRAM[QTD_REGISTROS_BLOCO];
	size_t qtdRegistrosEscritos = 0, qtdRegistrosLidos = 0;
	unsigned long contadorRegistros = 0;

	for(int i=0; i<QTD_BLOCOS; ++i) //laço do numero de blocos (cada bloco = 4 registros)
	{
		//para cada escrita, produzimos 4 registros
		for(int j=0; j<QTD_REGISTROS_BLOCO; ++j)
		{
			bufferRAM[j].chave = contadorRegistros;
			contadorRegistros++;
			bufferRAM[j].naochave = (unsigned long) genRandLong(&r);
			for(int p = 0; p<DATA_SIZE-1; ++p)
			{
				bufferRAM[j].data[p] = (unsigned char) genRandLong(&r) % (126 - 33) + 33;
			}
			bufferRAM[j].data[DATA_SIZE-1] = '\0'; //tratando como string %s

			// insercao numa ABP do campo naochave do arquivo como campo chave da arvore
			// insereABP(&raiz, bufferRAM[j].naochave, i);
		}

		qtdRegistrosEscritos = fwrite(bufferRAM, sizeof(TRegistro), QTD_REGISTROS_BLOCO, fArq);
		//printf("Gravei %lu registros\n", qtdRegistrosEscritos);
	}

	//voltar o cabecote de leitura para o comeco do arquivo
	rewind(fArq);

	//imprimir o arquivo de 4 em 4 registros
	for(int i=0; i<QTD_BLOCOS; ++i)
	{
		qtdRegistrosLidos = fread(bufferRAM, sizeof(TRegistro), QTD_REGISTROS_BLOCO, fArq);

		printf("\nBLOCO NUMERO %d\n", i+1);
		for(int j=0; j<qtdRegistrosLidos; ++j)
		{
			printf("CHAVE: %lu\n", bufferRAM[j].chave);
			printf("NAOCHAVE: %lu\n", bufferRAM[j].naochave);
			printf("DATA: %s\n", bufferRAM[j].data);
		}
	}

	//SEEK_SET volta ao inicio do arquivo
	//avanca 4096*numerobloco bytes
	//se sabemos o numero do bloco, posicionamos o cabecote no inicio dele (do bloco)

	unsigned long numerobloco = 92;
	fseek(fArq, 4096*numerobloco, SEEK_SET);
	qtdRegistrosLidos = fread(bufferRAM, sizeof(TRegistro), QTD_REGISTROS_BLOCO, fArq);

	printf("\n\nNUMERO BLOCO DE INTERESSE: %lu\n", numerobloco);
	for(int j=0; j<qtdRegistrosLidos; ++j)
	{
		//se tivermos tambem o campo chave, é só fazer uma pesquisa
		printf("CHAVE: %lu\n", bufferRAM[j].chave);
		printf("NAOCHAVE: %lu\n", bufferRAM[j].naochave);
		printf("DATA: %s\n", bufferRAM[j].data);
	}

	fclose(fArq);
	return 0;
}