#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#define N 13  // Define o tamanho do buffer como 100

char buffer1[N] = { '\0' };         // Buffer para Thread 1 -> Thread 2
char *buffer2;        // Buffer para Thread 2 -> Thread 3
long fileSize;
sem_t sem1, sem2, sem3;     // Semáforos para sincronização entre as threads
FILE *fileIn, *fileOut; // Arquivos de entrada e saída


// Thread 1: Ler do arquivo de entrada e colocar no buffer1
void *thread1(void *arg) {
    char c;
    int charlidos=0;
    printf("\nThread 1 executando\n");

    fseek(fileIn, 0, SEEK_END); // Move o ponteiro para o final do arquivo
    fileSize = ftell(fileIn); // Obtém a posição atual do ponteiro (tamanho do arquivo)
    fseek(fileIn, 0, SEEK_SET); // Volta o ponteiro para o início (opcional, se você precisar continuar lendo o arquivo)

    printf("tamanho do arquivo : %ld", fileSize);

    while ((c = fgetc(fileIn)) != EOF) {  // Lê o arquivo caractere por caractere
        if(charlidos>=N){
            printf("\ncharlidos :%d\n", charlidos);
            charlidos=0;
            puts(buffer1);
            sem_post(&sem1);
            sem_wait(&sem2);
            memset(buffer1, '\0', N);
        }
            buffer1[charlidos]=c;
            charlidos++;
    }

    if(buffer1!='\0'){
        printf("\ncharlidos :%d\n", charlidos);
        charlidos=0;
        puts(buffer1);
        sem_post(&sem1);
        sem_wait(&sem2);
        memset(buffer1, '\0', N);
    }
    sem_post(&sem3);
    pthread_exit(NULL);
}

// Thread 2: Processar o buffer1 e colocar no buffer2
void *thread2(void *arg) {
    long int chartransf=0;
    int j=0, i=0, n=0;

    sem_wait(&sem1);
    printf("\nThread 2 iniciada\n");
    
    while (chartransf<fileSize) {
        while( i < (n<=10? (2*n+1):10)){
            buffer2[j]=buffer1[(chartransf%N)];
            chartransf++; 
            i++; j++;
            printf("chartreanf: %ld  n: %d   i: %d \n\n", chartransf,n, i);
            if(chartransf%N==0 && chartransf<fileSize){
                printf("\ncaracteres enviados para o buffer2 ate agora: %ld\n\n", chartransf);
                sem_post(&sem2);
                sem_wait(&sem1);
                printf("\nThread 2 Passando o conteudo de buffer1 para o buffer2\n");
            }
        }
        puts(buffer2);
        puts("\n");
        buffer2[j]='\n';
        i=0;
        j++; n++;
            
        
    }
    
    sem_post(&sem2);
    pthread_exit(NULL);
}

// Thread 3: Imprimir o buffer2 no arquivo de saída
void *thread3(void *arg) {
    printf("\nThread 3 iniciada\n");
    sem_wait(&sem3);  // Espera por dados no buffer2
    
    fputs(buffer2, fileOut);  // Imprime o buffer2 no arquivo de saída
    fflush(fileOut);  // Garante que os dados sejam gravados imediatamente
    memset(buffer2, '\0', (N + 10));  // Limpa o buffer2

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
        fclose(fileIn); // Fecha o arquivo de entrada em caso de erro no arquivo de saída
        return 3;
    }

    // Alocar memória para os buffers
    buffer2 = (char *)malloc((N + 10) * sizeof(char)); // Buffer2 maior para considerar os novos '\n'
    if (buffer1 == NULL || buffer2 == NULL) {
        printf("Erro ao alocar memória para os buffers.\n");
        fclose(fileIn);
        fclose(fileOut);
        return 5;
    }

    pthread_t tid[3];

    // Inicializar os semáforos
    sem_init(&sem1, 0, 0); // Inicializa com 0 para garantir a sincronização
    sem_init(&sem2, 0, 0);
    sem_init(&sem3, 0, 0);

    // Criar as threads
    if (pthread_create(&tid[0], NULL, thread1, NULL)) { 
        printf("--ERRO: pthread_create() para thread 1\n"); exit(-1); 
    }
    if (pthread_create(&tid[1], NULL, thread2, NULL)) { 
        printf("--ERRO: pthread_create() para thread 2\n"); exit(-1); 
    }
    if (pthread_create(&tid[2], NULL, thread3, NULL)) { 
        printf("--ERRO: pthread_create() para thread 3\n"); exit(-1); 
    }

    //--Esperar todas as threads terminarem
    for (int i = 0; i < 3; i++) {
        if (pthread_join(tid[i], NULL)) {
            printf("--ERRO: pthread_join() para thread %d\n", i + 1);exit(-1);
        }
    }

    //--finaliza o semaforo
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);

    // Liberar a memória do buffer
    free(buffer2);

    // Fechar o arquivo de saída
    fclose(fileOut);
    printf("\n");
    return 0;
}