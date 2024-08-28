#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000 // valor máximo de um elemento do vetor

int main(int argc, char *argv[]) {
    float *vetorA, *vetorB; // vetores que serão gerados
    long int N; // quantidade de elementos dos vetores que serão gerados
    float elem1, elem2; // valor gerado para incluir no vetor
    double produto_interno = 0; // produto interno dos vetores
    FILE *descritorArquivo; // descritor do arquivo de saída
    size_t ret; // retorno da função de escrita no arquivo de saída

    // Verifica se o número de elementos e o nome do arquivo foram passados como argumentos
    if (argc < 3) {
        fprintf(stderr, "Digite: %s <dimensao> <arquivo saida>\n", argv[0]);
        return 1;
    }
    N = atoi(argv[1]);

    // Aloca memória para os vetores
    vetorA = (float *)malloc(sizeof(float) * N);
    vetorB = (float *)malloc(sizeof(float) * N);
    if (!vetorA || !vetorB) {
        fprintf(stderr, "Erro de alocação da memória dos vetores\n");
        return 2;
    }

    // Preenche os vetores com valores float aleatórios
    srand(time(NULL));
    for (long int i = 0; i < N; i++) {
        elem1 = (rand() % MAX)/7.0;
        elem2 = (rand() % MAX)/7.0;
        vetorA[i] = elem1;
        vetorB[i] = elem2; 
    }

    // Calcula o produto interno dos vetores
    for (long int i = 0; i < N; i++) {
        produto_interno += vetorA[i] * vetorB[i];
    }

    // Abre o arquivo para escrita binária
    descritorArquivo = fopen(argv[2], "wb");
    if (!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }

    // Escreve a dimensão
    ret = fwrite(&N, sizeof(long int), 1, descritorArquivo);
    // Escreve os elementos dos vetores
    ret = fwrite(vetorA, sizeof(float), N, descritorArquivo);
    ret = fwrite(vetorB, sizeof(float), N, descritorArquivo);
    // Escreve o produto interno
    ret = fwrite(&produto_interno, sizeof(double), 1, descritorArquivo);

    // Fecha o arquivo de saída
    fclose(descritorArquivo);

    // Abre o arquivo para leitura binária
    descritorArquivo = fopen(argv[2], "rb");
    if (!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo para leitura\n");
        return 4;
    }

    // Lê a dimensão
    ret = fread(&N, sizeof(long int), 1, descritorArquivo);
    // Aloca memória para os vetores
    vetorA = (float *)realloc(vetorA, sizeof(float) * N);
    vetorB = (float *)realloc(vetorB, sizeof(float) * N);
    if (!vetorA || !vetorB) {
        fprintf(stderr, "Erro de alocação da memória dos vetores\n");
        fclose(descritorArquivo);
        return 5;
    }
    // Lê os elementos dos vetores
    ret = fread(vetorA, sizeof(float), N, descritorArquivo);
    ret = fread(vetorB, sizeof(float), N, descritorArquivo);
    // Lê o produto interno
    double produto_interno_lido;
    ret = fread(&produto_interno_lido, sizeof(double), 1, descritorArquivo);

    // Imprime os vetores e o produto interno lido
    printf("Vetor A:\n");
    for (long int i = 0; i < N; i++) {
        printf("%f ", vetorA[i]);
    }
    printf("\n\n");

    printf("Vetor B:\n");
    for (long int i = 0; i < N; i++) {
        printf("%f ", vetorB[i]);
    }
    printf("\n\n");

    printf("Produto Interno Calculado: %f\n", produto_interno);
    printf("Produto Interno Lido do Arquivo: %f\n", produto_interno_lido);

    // Libera a memória alocada
    free(vetorA);
    free(vetorB);
    fclose(descritorArquivo);

    return 0;
}
