#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// Função para medir o tempo em segundos usando QueryPerformanceCounter
double get_time() {
    LARGE_INTEGER frequency, start;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    return (double)start.QuadPart / frequency.QuadPart;
}

// Função para ler uma matriz a partir de um arquivo binário
float* lerMatrizBinario(const char* nomeArquivo, int* linhas, int* colunas) {
    FILE* arq = fopen(nomeArquivo, "rb");
    if (arq == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nomeArquivo);
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
void escreverMatrizBinario(const char* nomeArquivo, float* matriz, int linhas, int colunas) {
    FILE* arq = fopen(nomeArquivo, "wb");
    if (arq == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s para escrita\n", nomeArquivo);
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
    inicio = get_time();
    A = lerMatrizBinario(argv[1], &NA, &MA);
    B = lerMatrizBinario(argv[2], &NB, &MB);
    fim = get_time();
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
    inicio = get_time();
    for (int i = 0; i < NA; i++) {
        for (int j = 0; j < MB; j++) {
            C[i * MB + j] = 0;
            for (int k = 0; k < MA; k++) {
                C[i * MB + j] += A[i * MA + k] * B[k * MB + j];
            }
        }
    }
    fim = get_time();
    printf("Tempo de processamento: %lf segundos\n", fim - inicio);

    // Escrita da matriz resultante em arquivo binário
    inicio = get_time();
    escreverMatrizBinario(argv[3], C, NA, MB);
    fim = get_time();
    printf("Tempo de escrita: %lf segundos\n", fim - inicio);

    // Liberação de memória
    free(A);
    free(B);
    free(C);

    return 0;
}
