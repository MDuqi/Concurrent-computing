#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <pthread.h>

// Estrutura para passar os dados para as threads
typedef struct {
    int inicioLinha;
    int fimLinha;
    int NA;
    int MA;
    int MB;
    float* A;
    float* B;
    float* C;
} ThreadData;

// Função para medir o tempo em segundos usando QueryPerformanceCounter
double get_time() {
    LARGE_INTEGER frequency, count;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&count);
    return (double)count.QuadPart / frequency.QuadPart;
}

// Função para ler uma matriz a partir de um arquivo binário
float* lerMatrizBinario(const char* nomeArquivo, int* linhas, int* colunas) {
    FILE* arq = fopen(nomeArquivo, "rb");
    if (arq == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nomeArquivo);
        exit(1);
    }

    fread(linhas, sizeof(int), 1, arq);
    fread(colunas, sizeof(int), 1, arq);

    float* matriz = (float*)malloc((*linhas) * (*colunas) * sizeof(float));
    if (matriz == NULL) {
        fprintf(stderr, "Erro de alocação de memória\n");
        exit(2);
    }

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

    fwrite(&linhas, sizeof(int), 1, arq);
    fwrite(&colunas, sizeof(int), 1, arq);
    fwrite(matriz, sizeof(float), linhas * colunas, arq);
    fclose(arq);
}

// Função que será executada por cada thread para multiplicar uma parte da matriz
void* multiplicar_matrizes(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    for (int i = data->inicioLinha; i < data->fimLinha; i++) {
        for (int j = 0; j < data->MB; j++) {
            data->C[i * data->MB + j] = 0;
            for (int k = 0; k < data->MA; k++) {
                data->C[i * data->MB + j] += data->A[i * data->MA + k] * data->B[k * data->MB + j];
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int NA, MA, NB, MB;
    float *A, *B, *C;
    double inicio, fim, inicioTotal, fimTotal;
    int num_threads;

    if (argc < 5) {
        fprintf(stderr, "Uso: %s <matrizA.bin> <matrizB.bin> <matrizC.bin> <num_threads>\n", argv[0]);
        return 1;
    }

    num_threads = atoi(argv[4]);
    if (num_threads <= 0) {
        fprintf(stderr, "Numero de threads inválido\n");
        return 1;
    }

    inicioTotal = get_time();

    // Leitura das matrizes de entrada
    A = lerMatrizBinario(argv[1], &NA, &MA);
    B = lerMatrizBinario(argv[2], &NB, &MB);
   
    // Verifica se a multiplicação é possível
    if (MA != NB) {
        fprintf(stderr, "Erro: As matrizes não podem ser multiplicadas.\n");
        free(A);
        free(B);
        return 1;
    }

    // Aloca memória para a matriz resultante
    C = (float *)malloc(NA * MB * sizeof(float));
    if (C == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria\n");
        free(A);
        free(B);
        return 1;
    }

    // Criação e execução das threads
    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ThreadData* thread_data = (ThreadData*)malloc(num_threads * sizeof(ThreadData));
    
    int linhas_por_thread = NA / num_threads;
    int restante_linhas = NA % num_threads;
    
    for (int t = 0; t < num_threads; t++) {
        thread_data[t].inicioLinha = t * linhas_por_thread;
        thread_data[t].fimLinha = (t == num_threads - 1) ? NA : (t + 1) * linhas_por_thread;
        thread_data[t].NA = NA;
        thread_data[t].MA = MA;
        thread_data[t].MB = MB;
        thread_data[t].A = A;
        thread_data[t].B = B;
        thread_data[t].C = C;

        pthread_create(&threads[t], NULL, multiplicar_matrizes, (void*)&thread_data[t]);
    }

    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    // Escrita da matriz resultante em arquivo binário
    escreverMatrizBinario(argv[3], C, NA, MB);
    
    // Liberação de memória
    free(A);
    free(B);
    free(C);
    free(threads);
    free(thread_data);

    fimTotal = get_time();
    printf("\nTempo total de execucao do programa: %lf segundos\n\n", fimTotal - inicioTotal);

    return 0;
}
