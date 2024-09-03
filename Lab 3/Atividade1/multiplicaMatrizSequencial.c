#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

// Função para ler uma matriz a partir de um arquivo binário
float* lerMatrizBinario(const char* ArquivoBinario, int* linhas, int* colunas) {
    FILE* arq = fopen(ArquivoBinario, "rb");
    if (arq == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", ArquivoBinario);
        exit(1);
    }

    // Lê as dimensões da matriz
    fread(linhas, sizeof(int), 1, arq);
    fread(colunas, sizeof(int), 1, arq);

    // Aloca memória para a matriz
    float* matriz = (float*)malloc((*linhas) * (*colunas) * sizeof(float));
    if (matriz == NULL) {
        fprintf(stderr, "Erro de alocação de memória\n");
        exit(2);
    }

    // Lê os elementos da matriz do arquivo
    fread(matriz, sizeof(float), (*linhas) * (*colunas), arq);

    fclose(arq);
    return matriz;
}

// Função para escrever uma matriz em um arquivo binário
void escreverMatrizBinario(const char* ArquivoBinario, float* matriz, int linhas, int colunas) {
    FILE* arq = fopen(ArquivoBinario, "wb");
    if (arq == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s para escrita\n", ArquivoBinario);
        exit(3);
    }

    // Escreve as dimensões da matriz
    fwrite(&linhas, sizeof(int), 1, arq);
    fwrite(&colunas, sizeof(int), 1, arq);

    // Escreve os elementos da matriz no arquivo
    fwrite(matriz, sizeof(float), linhas * colunas, arq);

    fclose(arq);
}

int main(int argc, char *argv[]) {
    int NA, MA, NB, MB;
    float *A, *B, *C;
    double inicio, fim;

    if (argc < 4) {
        fprintf(stderr, "Uso: %s <matrizA.bin> <matrizB.bin> <matrizC.bin>\n", argv[0]);
        return 1;
    }

    // Leitura das matrizes de entrada
    GET_TIME(inicio);
    A = lerMatrizBinario(argv[1], &NA, &MA);
    B = lerMatrizBinario(argv[2], &NB, &MB);
    GET_TIME(fim);
    printf("Tempo de leitura: %lf segundos\n", fim - inicio);

    // Verifica se a multiplicação é possível
    if (MA != NB) {
        fprintf(stderr, "Erro: As matrizes não podem ser multiplicadas.\n");
        free(A);
        free(B);
        return 1;
    }

    // Aloca memória para a matriz resultante
    C = (float *)malloc(NA * MB * sizeof(float));

    // Multiplicação das matrizes
    GET_TIME(inicio);
    for (int i = 0; i < NA; i++) {
        for (int j = 0; j < MB; j++) {
            C[i * MB + j] = 0;
            for (int k = 0; k < MA; k++) {
                C[i * MB + j] += A[i * MA + k] * B[k * MB + j];
            }
        }
    }
    GET_TIME(fim);
    printf("Tempo de processamento: %lf segundos\n", fim - inicio);

    // Escrita da matriz resultante em arquivo binário
    GET_TIME(inicio);
    escreverMatrizBinario(argv[3], C, NA, MB);
    GET_TIME(fim);
    printf("Tempo de escrita: %lf segundos\n", fim - inicio);

    // Liberação de memória
    free(A);
    free(B);
    free(C);

    return 0;
}
