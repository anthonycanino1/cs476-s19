package main

import "fmt"

func main() {
	c1, c2 := make(chan int), make(chan int)
	go func() {
		<-c2
		c1 <- 0
	}()
	<-c1
	c2 <- 0
	fmt.Printf("Done!\n")
}
