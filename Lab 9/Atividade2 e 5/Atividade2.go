package main

import (
	"fmt"
)

func tarefa(str chan string) {
	msg := <-str
	fmt.Println("Goroutine recebeu:", msg)

	str <- "Oi Main, bom dia, tudo bem?"

	msg = <-str
	fmt.Println("Goroutine recebeu:", msg)

	str <- "Certo, entendido."

	fmt.Println("Goroutine: finalizando")
}

func main() {
	str := make(chan string)

	go tarefa(str)

	str <- "Olá, Goroutine, bom dia!"
	msg := <-str
	fmt.Println("Main recebeu:", msg)

	str <- "Tudo bem! Vou terminar tá?"
	msg = <-str
	fmt.Println("Main recebeu:", msg)
        str <- ""
	fmt.Println("Main: finalizando")
}

