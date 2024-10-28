#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define N 13  // Define o tamanho do buffer1 como 13

// Declaração dos buffers e variáveis globais
char buffer1[N] = { '\0' };    // Buffer para Thread 1 -> Thread 2
char *buffer2;                 // Buffer para Thread 2 -> Thread 3
long fileSize;                 // Tamanho do arquivo
sem_t sem1, sem2, sem3;        // Semáforos para sincronização entre as threads
FILE *fileIn, *fileOut;        // Arquivos de entrada e saída

// Thread 1: Ler do arquivo de entrada e colocar no buffer1
void *thread1(void *arg) {
    char c;
    int charlidos = 0;  // Contador de caracteres lidos
    printf("\nThread 1 executando\n");

    // Obtém o tamanho do arquivo
    fseek(fileIn, 0, SEEK_END);   // Move o ponteiro para o final do arquivo
    fileSize = ftell(fileIn);     // Obtém o tamanho do arquivo
    fseek(fileIn, 0, SEEK_SET);   // Retorna o ponteiro para o início do arquivo

    printf("tamanho do arquivo : %ld", fileSize);

    // Lê o arquivo caractere por caractere
    while ((c = fgetc(fileIn)) != EOF) {  
        if (charlidos >= N) {  // Se o buffer está cheio, passa o controle para a Thread 2
            printf("\ncharlidos :%d\n", charlidos);
            charlidos = 0;      // Reseta o contador de caracteres
            puts(buffer1);      // Mostra o conteúdo do buffer1
            sem_post(&sem1);    // Libera a Thread 2 para processar
            sem_wait(&sem2);    // Espera a Thread 2 terminar de processar
            memset(buffer1, '\0', N);  // Limpa o buffer1
        }
        buffer1[charlidos] = c;  // Armazena o caractere no buffer
        charlidos++;
    }

    // Verifica se ainda restam caracteres no buffer1 ao fim do arquivo
    if (buffer1[0] != '\0') { 
        printf("\ncharlidos :%d\n", charlidos);
        charlidos = 0;
        puts(buffer1);
        sem_post(&sem1);    // Envia o conteúdo restante para a Thread 2
        sem_wait(&sem2);    // Espera a Thread 2 processar
        memset(buffer1, '\0', N);  // Limpa o buffer
    }

    sem_post(&sem3);  // Sinaliza para a Thread 3 que o processamento terminou
    pthread_exit(NULL);
}

// Thread 2: Processar o buffer1 e colocar no buffer2
void *thread2(void *arg) {
    long int chartransf = 0;  // Contador de caracteres transferidos
    int j = 0, i = 0, n = 0;  // Variáveis de controle

    sem_wait(&sem1);  // Espera a Thread 1 preencher o buffer1
    printf("\nThread 2 iniciada\n");
    
    while (chartransf < fileSize) {
        // Transfere caracteres de buffer1 para buffer2 com base em n (quantidade de iterações)
        while (i < (n <= 10 ? (2 * n + 1) : 10)) {
            buffer2[j] = buffer1[chartransf % N];  // Transferência circular do buffer1
            chartransf++; 
            i++; j++;
            printf("chartreanf: %ld  n: %d   i: %d \n\n", chartransf, n, i);

            // Verifica se terminou de processar o conteúdo de buffer1
            if (chartransf % N == 0 && chartransf < fileSize) {
                printf("\ncaracteres enviados para o buffer2 até agora: %ld\n\n", chartransf);
                sem_post(&sem2);  // Libera a Thread 1 para preencher mais dados
                sem_wait(&sem1);  // Espera por novos dados de buffer1
                printf("\nThread 2 Passando o conteúdo de buffer1 para o buffer2\n");
            }
        }
        puts(buffer2);  // Exibe o conteúdo de buffer2
        puts("\n");
        buffer2[j] = '\n';  // Adiciona um caractere de nova linha no final do buffer2
        i = 0;
        j++; n++;
    }
    
    sem_post(&sem2);  // Finaliza a comunicação com a Thread 1
    pthread_exit(NULL);
}

// Thread 3: Imprimir o buffer2 no arquivo de saída
void *thread3(void *arg) {
    printf("\nThread 3 iniciada\n");
    
    sem_wait(&sem3);  // Espera o sinal da Thread 1 indicando o fim dos dados
    fputs(buffer2, fileOut);  // Escreve o conteúdo de buffer2 no arquivo de saída
    fflush(fileOut);  // Garante que os dados sejam gravados no arquivo imediatamente
    memset(buffer2, '\0', (N + 10));  // Limpa o buffer2 para evitar sobrescritas de dados antigos

    pthread_exit(NULL);
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    // Abrir arquivos de entrada e saída
    fileIn = fopen(argv[1], "r");
    if (!fileIn) {
        printf("Erro ao abrir o arquivo de entrada: %s\n", argv[1]);
        return 2;
    }

    fileOut = fopen(argv[2], "w");
    if (!fileOut) {
        printf("Erro ao abrir o arquivo de saída: %s\n", argv[2]);
        fclose(fileIn);  // Fecha o arquivo de entrada em caso de erro
        return 3;
    }

    // Alocar memória para buffer2
    buffer2 = (char *)malloc((N + 10) * sizeof(char));  // Aloca espaço suficiente para buffer2
    if (buffer1 == NULL || buffer2 == NULL) {
        printf("Erro ao alocar memória para os buffers.\n");
        fclose(fileIn);
        fclose(fileOut);
        return 5;
    }

    pthread_t tid[3];  // Identificadores das threads

    // Inicializar os semáforos
    sem_init(&sem1, 0, 0);  // Inicializa semáforo com valor 0 (Thread 1 -> Thread 2)
    sem_init(&sem2, 0, 0);  // Inicializa semáforo com valor 0 (Thread 2 -> Thread 1)
    sem_init(&sem3, 0, 0);  // Inicializa semáforo com valor 0 (Thread 1 -> Thread 3)

    // Criar as threads
    if (pthread_create(&tid[0], NULL, thread1, NULL)) { 
        printf("--ERRO: pthread_create() para thread 1\n"); 
        exit(-1); 
    }
    if (pthread_create(&tid[1], NULL, thread2, NULL)) { 
        printf("--ERRO: pthread_create() para thread 2\n"); 
        exit(-1); 
    }
    if (pthread_create(&tid[2], NULL, thread3, NULL)) { 
        printf("--ERRO: pthread_create() para thread 3\n"); 
        exit(-1); 
    }

    // Esperar as threads terminarem
    for (int i = 0; i < 3; i++) {
        if (pthread_join(tid[i], NULL)) {
            printf("--ERRO: pthread_join() para thread %d\n", i + 1);
            exit(-1);
        }
    }

    // Destruir os semáforos
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);

    // Liberar a memória alocada para buffer2
    free(buffer2);

    // Fechar os arquivos
    fclose(fileIn);
    fclose(fileOut);

    printf("\n");
    return 0;
}
