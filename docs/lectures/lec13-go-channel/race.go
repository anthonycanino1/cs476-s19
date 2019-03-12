package main

import "fmt"

var n = 0

func step() {
	for i := 0; i < 10000; i++ {
		n++
	}
	fmt.Printf("Current:%d\n", n)
}

func main() {
	for i := 0; i < 10; i++ {
		go step()
	}
	for {
	}
}
