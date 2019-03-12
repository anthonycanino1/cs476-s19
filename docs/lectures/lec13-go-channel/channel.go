package main

import "fmt"

func main() {
	c1 := make(chan int)
	c2 := make(chan int)

	go func(f int) {
		v := 1
		for f > 0 {
			v *= f
			f--
		}
		c1 <- v
	}(4)

	go func(f int) {
		v := 1
		for f > 0 {
			v *= f
			f--
		}
		c2 <- v
	}(5)

	f4 := <-c1
	f5 := <-c2
	fmt.Printf("f(4) = %d, f(5) = %d\n", f4, f5)
}
