#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

// Estruturas para parâmetros de thread e resultados
typedef struct {
    int idThread;
    int nThreads;
    long int N;
    float *vetorA;
    float *vetorB;
} t_Args;

typedef struct {
    double resultado;
} t_Ret;

// Função para ler dados do arquivo binário
int ler_binario(const char *filename, long *N, float **vetorA, float **vetorB, double *produto_interno) {
    FILE *arq;
    size_t ret;

    arq = fopen(filename, "rb");
    if (!arq) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 1;
    }

    ret = fread(N, sizeof(long), 1, arq);
    if (!ret) {
        fprintf(stderr, "Erro de leitura da dimensão do arquivo\n");
        fclose(arq);
        return 2;
    }

    *vetorA = (float *)malloc(sizeof(float) * (*N));
    *vetorB = (float *)malloc(sizeof(float) * (*N));
    if (!*vetorA || !*vetorB) {
        fprintf(stderr, "Erro de alocação de memória dos vetores\n");
        fclose(arq);
        return 3;
    }

    ret = fread(*vetorA, sizeof(float), *N, arq);
    if (ret < *N) {
        fprintf(stderr, "Erro de leitura do vetor A\n");
        free(*vetorA);
        free(*vetorB);
        fclose(arq);
        return 4;
    }

    ret = fread(*vetorB, sizeof(float), *N, arq);
    if (ret < *N) {
        fprintf(stderr, "Erro de leitura do vetor B\n");
        free(*vetorA);
        free(*vetorB);
        fclose(arq);
        return 5;
    }

    ret = fread(produto_interno, sizeof(double), 1, arq);
    if (!ret) {
        fprintf(stderr, "Erro de leitura do produto interno\n");
        free(*vetorA);
        free(*vetorB);
        fclose(arq);
        return 6;
    }

    fclose(arq);
    return 0;
}

// Função executada pelas threads
void *calculaProdutoInterno(void *arg) {
    t_Args *args = (t_Args *)arg;
    long int start = (args->idThread * args->N) / args->nThreads;
    long int end = ((args->idThread + 1) * args->N) / args->nThreads;
    if (args->idThread == args->nThreads - 1) end = args->N;

    double soma_local = 0.0;
    for (long int i = start; i < end; i++) {
        soma_local += args->vetorA[i] * args->vetorB[i];
    }

    t_Ret *ret = malloc(sizeof(t_Ret));
    if (ret != NULL) {
        ret->resultado = soma_local;
    } else {
        printf("Erro de alocação de memória na thread\n");
        pthread_exit(NULL);
    }
    pthread_exit((void *)ret);
}

// Função principal
int main(int argc, char *argv[]) {
    long int N;
    float *vetorA, *vetorB;
    double produto_interno_sequencial;
    double produto_interno_concorrente;
    int T; // número de threads
    t_Args args;
    pthread_t *threads;
    t_Ret *resultado_thread;

    if (argc < 3) {
        printf("\nUso: %s <arquivo de entrada> <numero de threads>\n", argv[0]);
        return 1;
    }

    // Lê os dados do arquivo binário
    int status = ler_binario(argv[1], &N, &vetorA, &vetorB, &produto_interno_sequencial);
    if (status != 0) {
        return status;
    }

    // Lê o número de threads da linha de comando
    T = atoi(argv[2]);
    if (T > N) T = N;

    // Aloca memória para as threads
    threads = (pthread_t *)malloc(sizeof(pthread_t) * T);
    if (!threads) {
        fprintf(stderr, "Erro de alocação de memória para threads\n");
        free(vetorA);
        free(vetorB);
        return 7;
    }

    // Configura os parâmetros compartilhados entre as threads
    args.nThreads = T;
    args.N = N;
    args.vetorA = vetorA;
    args.vetorB = vetorB;

    // Cria e sincroniza as threads
    double soma_par_global = 0.0;
    for (int i = 0; i < T; i++) {
        args.idThread = i;
        if (pthread_create(&threads[i], NULL, calculaProdutoInterno, (void *)&args)) {
            fprintf(stderr, "Erro ao criar thread %d\n", i);
            free(vetorA);
            free(vetorB);
            free(threads);
            return 8;
        }
    }

    for (int i = 0; i < T; i++) {
        if (pthread_join(threads[i], (void **)&resultado_thread)) {
            fprintf(stderr, "Erro ao aguardar término da thread %d\n", i);
            free(vetorA);
            free(vetorB);
            free(threads);
            return 9;
        }
        soma_par_global += resultado_thread->resultado;
        free(resultado_thread);
    }

    // Imprime o produto interno calculado e o valor armazenado no arquivo
    printf("Produto Interno Sequencial: %.26f\n", produto_interno_sequencial);
    printf("Produto Interno Concorrente: %.26f\n", soma_par_global);

    // Calcula e imprime a variação relativa em módulo
    double variacao_relativa = fabs((produto_interno_sequencial - soma_par_global) / produto_interno_sequencial);
    printf("Variacao Relativa: %.6f\n", variacao_relativa);

    // Libera memória alocada
    free(vetorA);
    free(vetorB);
    free(threads);

    return 0;
}
