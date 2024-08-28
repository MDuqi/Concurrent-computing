O arquivo gera_vet_prod.c contém o programa sequencial que gera 2 vetores randomicos de tamanho N, faz seu produto
interno e guarda todas essas informações no arquivo binario prodinter.bin

O arquivo ler_bin.c é apenas um programa de verificação do arquivo binario, realizando sua leitura e imprimindo 
seus valores no terminal.

E por último, o arquivo prod_inter_conco.c tem o programa concorrente que le do arquivo binario é realiza o calculo do 
produto interno dividido em threads e analisa a Variação Relativa deste calculo para o feito no gera_vet_prod.