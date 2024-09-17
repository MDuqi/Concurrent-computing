#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

long int soma = 0, ultimo = 0; // Variáveis compartilhadas
pthread_mutex_t mutex; // Mutex para exclusão mútua
pthread_cond_t cond_var; // Variável de condição para sincronização
int impressos = 0; // Contador para verificar quantos múltiplos de 10 foram impressos
int espera=0; //Variavel para barramento das outras threads de soma

// Função executada pelas threads
void *ExecutaTarefa(void *arg) {
  long int id = (long int) arg;
  int i;
  printf("\n\nThread : %ld está executando...\n\n", id);

  for (i = 0; i < 100000; i++) {
    pthread_mutex_lock(&mutex);
    if(!espera){
      // Se 'soma' é múltiplo de 10, a thread espera a impressão pela thread 'extra'
      if ((soma % 10 == 0) && (soma != 0) && (impressos < 20)) {
        espera=1;
        pthread_cond_wait(&cond_var, &mutex);  // Espera a sinalização da thread 'extra'
        printf("\n");
        espera=0;
        pthread_cond_broadcast(&cond_var);
      }

      soma++; // Incrementa a variável compartilhada
    }else{
      i--;
      pthread_cond_wait(&cond_var, &mutex);
    }
    pthread_mutex_unlock(&mutex);
  }

  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

// Função executada pela thread de log
void *extra(void *args) {
  printf("\n\nExtra : está executando...\n\n");

  while (impressos < 20) {
    pthread_mutex_lock(&mutex);

    // Se 'soma' é múltiplo de 10 e ainda não foi impresso
    if ((soma % 10 == 0) && (soma != 0) && ultimo != soma) {
      printf("\nSOMA = %ld\n", soma);
      ultimo = soma; // Atualiza o último múltiplo impresso
      impressos++;   // Incrementa o número de múltiplos impressos

      // Sinaliza para as threads 'ExecutaTarefa' que podem continuar
      pthread_cond_broadcast(&cond_var);
    }

    pthread_mutex_unlock(&mutex);
  }

  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

// Função principal
int main(int argc, char *argv[]) {
  pthread_t *tid; // Identificadores das threads
  int nthreads; // Quantidade de threads

  // Avalia os parâmetros de entrada
  if (argc < 2) {
    printf("Digite: %s <numero de threads>\n", argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);

  // Aloca as estruturas
  tid = (pthread_t*) malloc(sizeof(pthread_t) * (nthreads + 1));
  if (tid == NULL) { puts("ERRO--malloc"); return 2; }

  // Inicializa o mutex e a variável de condição
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_var, NULL);

  // Cria as threads de tarefas
  for (long int t = 0; t < nthreads; t++) {
    if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  // Cria a thread de log
  if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
    printf("--ERRO: pthread_create()\n"); exit(-1);
  }

  // Espera todas as threads terminarem
  for (int t = 0; t < nthreads + 1; t++) {
    if (pthread_join(tid[t], NULL)) {
      printf("--ERRO: pthread_join() \n"); exit(-1);
    }
  }

  // Finaliza o mutex e a variável de condição
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond_var);

  printf("Valor final de 'soma' = %ld\n", soma);
  return 0;
}
