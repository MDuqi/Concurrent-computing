#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Estrutura para armazenar os argumentos da thread
typedef struct {
    int idThread;
    int nThreads;
    int N;
    int *vetor;
} t_Args;

// Funcao executada pelas threads
void *incrementaVetor(void *arg) {
    t_Args *args = (t_Args *) arg;
    int start = (args->idThread * args->N) / args->nThreads;
    int end = ((args->idThread + 1) * args->N) / args->nThreads;
    
    for (int i = start; i < end; i++) {
        args->vetor[i] += 1;
    }
    
    pthread_exit(NULL);
}

// Funcao para inicializar o vetor
void inicializaVetor(int *vetor, int N) {
    for (int i = 0; i < N; i++) {
        vetor[i] = i;  // Preenche o vetor com valores sequenciais (0, 1, 2, ...)
    }
}

// Funcao para imprimir o vetor
void imprimeVetor(int *vetor, int N) {
    for (int i = 0; i < N; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");
}

// Funcao para verificar o resultado final
void verificaVetor(int *vetor, int N) {
    int erro = 0;
    for (int i = 0; i < N; i++) {
        if (vetor[i] != i + 1) {
            erro = 1;
            printf("Erro na posicao %d: valor %d\n", i, vetor[i]);
        }
    }
    if (!erro) {
        printf("Todos os valores estao corretos!\n");
    }
}

// Funcao principal
int main(int argc, char *argv[]) {
    int N, M;
    int *vetor;
    pthread_t tid; // Identificador da thread
    t_Args args;   // Estrutura de argumentos para a thread

    // Verifica se o numero de elementos e de threads foi passado na linha de comando
    if(argc<3) { 
      printf("--ERRO: informe a qtde de elementos no vetor <N> e threads <M>\n", argv[0]); 
      return 1;
   }

    N = atoi(argv[1]);
    M = atoi(argv[2]);

    // Aloca memoria para o vetor
    vetor = (int *) malloc(N * sizeof(int));
    if (vetor == NULL) {
        printf("Erro ao alocar memoria para o vetor\n");
        return 2;
    }

    // Inicializa o vetor
    inicializaVetor(vetor, N);

    // Imprime o vetor antes da alteracao
    printf("Vetor antes da alteracao: ");
    imprimeVetor(vetor, N);

    // Configura os parametros compartilhados entre as threads
    args.nThreads = M;
    args.N = N;
    args.vetor = vetor;

    // Cria e sincroniza as threads
    for (int i = 0; i < M; i++) {
        args.idThread = i; // Atribui o identificador da thread

        if (pthread_create(&tid, NULL, incrementaVetor, (void *) &args)) {
            printf("Erro ao criar thread %d\n", i);
            return 4;
        }

        // Espera a thread terminar antes de criar a proxima
        if (pthread_join(tid, NULL)) {
            printf("Erro ao aguardar termino da thread %d\n", i);
            return 5;
        }
    }

    // Imprime o vetor depois da alteracao
    printf("Vetor depois da alteracao: ");
    imprimeVetor(vetor, N);

    // Verifica se o vetor esta correto
    verificaVetor(vetor, N);

    // Libera a memoria alocada
    free(vetor);

    return 0;
}
