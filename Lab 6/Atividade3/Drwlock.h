// drwlock.c

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t can_read;
    pthread_cond_t can_write;
    int rd;       // Número de leitores ativos
    int wr_fila;  // Número de escritores aguardando na fila
    int wr;       // Indica se há um escritor ativo
} drwlock_t;

// Inicialização do drwlock
void drwlock_init(drwlock_t *rw) {
    pthread_mutex_init(&rw->lock, NULL);
    pthread_cond_init(&rw->can_read, NULL);
    pthread_cond_init(&rw->can_write, NULL);
    rw->rd = 0;
    rw->wr = 0;
    rw->wr_fila = 0;
}

// Lock de leitura
void drwlock_rdlock(drwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    
    // Espera até não haver escritores ativos ou esperando
    while (rw->wr == 1 || rw->wr_fila > 0) {
        printf("Thread %ld aguardando para ler (escritor na fila)\n", pthread_self());
        pthread_cond_wait(&rw->can_read, &rw->lock);
    }

    // Incrementa o número de leitores ativos
    rw->rd++;
    
    pthread_mutex_unlock(&rw->lock);
}

// Lock de escrita
void drwlock_wrlock(drwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);

    // Incrementa o número de escritores na fila
    rw->wr_fila++;
    
    // Espera até que não haja escritores ativos ou leitores
    while (rw->wr == 1 || rw->rd > 0) {
        printf("Thread %ld aguardando para escrever (escritores na fila)\n", pthread_self());
        pthread_cond_wait(&rw->can_write, &rw->lock);
    }
    
    // Um escritor está ativo, então decrementa a fila e seta `wr` para 1
    rw->wr = 1;
    rw->wr_fila--;
    pthread_mutex_unlock(&rw->lock);
}

// Unlock (para leitura e escrita)
void drwlock_unlock(drwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);

    // Verifica se há um escritor ativo
    if (rw->wr == 1) {
        // Uma escrita terminou
        rw->wr = 0; // Indica que não há mais escritores escrevendo
    } else if (rw->rd > 0) {
        // Uma leitura terminou
        rw->rd--; // Decrementa o contador de leitores
    }

    // Verifica se há escritores esperando e não há leitores ativos
    if (rw->wr_fila > 0 && rw->rd == 0) {
        // Acorda um escritor, já que não há mais leitores
        pthread_cond_signal(&rw->can_write);
    } else if (rw->wr_fila == 0 && rw->wr == 0) {
        // Se não houver escritores esperando, acorda todos os leitores
        pthread_cond_broadcast(&rw->can_read);
    }

    pthread_mutex_unlock(&rw->lock);
}

// Destrói o drwlock
void drwlock_destroy(drwlock_t *rw) {
    pthread_mutex_destroy(&rw->lock);
    pthread_cond_destroy(&rw->can_read);
    pthread_cond_destroy(&rw->can_write);
}
