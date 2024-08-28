#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *arq;
    size_t ret;
    long int N;
    float *vetorA, *vetorB;
    double produto_interno;

    if (argc < 2) {
        printf("Uso: %s <arquivo de entrada>\n", argv[0]);
        return 1;
    }

    // Abre o arquivo binário para leitura
    arq = fopen(argv[1], "rb");
    if (!arq) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 2;
    }

    // Lê a dimensão
    ret = fread(&N, sizeof(long int), 1, arq);
    if (!ret) {
        fprintf(stderr, "Erro de leitura da dimensão do arquivo\n");
        fclose(arq);
        return 3;
    }

    // Aloca memória para os vetores
    vetorA = (float *)malloc(sizeof(float) * N);
    vetorB = (float *)malloc(sizeof(float) * N);
    if (!vetorA || !vetorB) {
        fprintf(stderr, "Erro de alocação de memória dos vetores\n");
        fclose(arq);
        return 4;
    }

    // Lê os vetores
    ret = fread(vetorA, sizeof(float), N, arq);
    if (ret < N) {
        fprintf(stderr, "Erro de leitura do vetor A\n");
        free(vetorA);
        free(vetorB);
        fclose(arq);
        return 5;
    }

    ret = fread(vetorB, sizeof(float), N, arq);
    if (ret < N) {
        fprintf(stderr, "Erro de leitura do vetor B\n");
        free(vetorA);
        free(vetorB);
        fclose(arq);
        return 6;
    }

    // Lê o produto interno
    ret = fread(&produto_interno, sizeof(double), 1, arq);
    if (!ret) {
        fprintf(stderr, "Erro de leitura do produto interno\n");
        free(vetorA);
        free(vetorB);
        fclose(arq);
        return 7;
    }

    fclose(arq);

    // Imprime os dados lidos
    printf("Dimensão N: %ld\n", N);

    printf("Vetor A: ");
    for (long int i = 0; i < N; i++) {
        printf("%f ", vetorA[i]);
    }
    printf("\n");

    printf("Vetor B: ");
    for (long int i = 0; i < N; i++) {
        printf("%f ", vetorB[i]);
    }
    printf("\n");

    printf("Produto Interno: %.26f\n", produto_interno);

    // Libera memória alocada
    free(vetorA);
    free(vetorB);

    return 0;
}
