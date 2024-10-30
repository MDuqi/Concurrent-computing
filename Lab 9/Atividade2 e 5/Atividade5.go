package main

import (
	"fmt"
	"math"
	"sync"
	"os"
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

// Função para verificar números e armazenar os primos encontrados
func verificarPrimos(numeros <-chan int, primos *[]int, mutex *sync.Mutex, wg *sync.WaitGroup) {
	defer wg.Done()
	for n := range numeros {
		if ehPrimo(n) {
			mutex.Lock()
			*primos = append(*primos, n)
			mutex.Unlock()
		}
	}
}

func main() {
	N := 100 // Intervalo de números a verificar (1 a N)
	M := 10   // Número de goroutines

	numeros := make(chan int, N)
	var primos []int
	var wg sync.WaitGroup
	var mutex sync.Mutex

	// Inicia M goroutines para verificar a primalidade
	for i := 0; i < M; i++ {
		wg.Add(1)
		go verificarPrimos(numeros, &primos, &mutex, &wg)
	}

	// Envia números para o canal 'numeros'
	go func() {
		for i := 1; i <= N; i++ {
			numeros <- i
		}
		close(numeros)
	}()

	// Aguarda as goroutines terminarem e processarem todos os números
	wg.Wait()

	// Exibe a quantidade de primos encontrados
	fmt.Printf("Quantidade de números primos encontrados: %d\n", len(primos))


	// Escreve os números primos encontrados em um arquivo
	file, err := os.Create("primos.txt")
	if err != nil {
		fmt.Println("Erro ao criar o arquivo:", err)
		return
	}
	defer file.Close()

	for _, primo := range primos {
		_, err := file.WriteString(fmt.Sprintf("%d\n", primo))
		if err != nil {
			fmt.Println("Erro ao escrever no arquivo:", err)
			return
		}
	}

	fmt.Println("\n Números primos gerados e salvos em primos.txt")
}