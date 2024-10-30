package main

import (
	"fmt"
	"math"
	"os"
	"bufio"
	"strconv"
)

// Função que verifica se um número é primo, baseada na implementação fornecida
func ehPrimo(n int) bool {
	if n <= 1 {	return false }
	if n == 2 {	return true }
	if n%2 == 0 { return false }
	for i := 3; i <= int(math.Sqrt(float64(n)))+1; i += 2 {
		if n%i == 0 { return false	}
	}
	return true
}

func main() {
	N := 100 // Intervalo de números a verificar (1 a N)
	var primos []int

	// Verifica cada número de 1 a N de forma sequencial
	for i := 1; i <= N; i++ {
		if ehPrimo(i) {
			primos = append(primos, i)
		}
	}

	// Exibe a quantidade total de números primos encontrados
	fmt.Printf("Quantidade de números primos encontrados: %d\n", len(primos))

	file, err := os.Open("primos.txt")
	if err != nil {
		fmt.Println("Erro ao abrir o arquivo:", err)
		return
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)

	j:=0
	erro:=true
	for scanner.Scan() {
		num, err := strconv.Atoi(scanner.Text())
		if err == nil {
			if num != primos[j] {
				fmt.Println("Erro nos números primos encontrados! O código concorrente apresenta problemas!")
				erro=false
				break
			}
		} else {
			fmt.Println("Erro ao converter número:", scanner.Text())
		}
		j++
	}

	if erro {
		fmt.Println("\nCorretude verificada! O código concorrente funciona corretamente!")
	}

	if err := scanner.Err(); err != nil {
		fmt.Println("Erro ao ler o arquivo:", err)
	}
	
}
