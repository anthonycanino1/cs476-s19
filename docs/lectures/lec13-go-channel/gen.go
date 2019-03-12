package main

import (
	"fmt"
	"math/rand"
)

func primeSearcher(top int, c chan int) {
	for {
		n := rand.Intn(top)
		div := 2
		isPrime := true
		for div < n {
			if n%div == 0 {
				isPrime = false
				break
			}
			div++
		}
		if isPrime {
			c <- n
		}
	}
}

const letters = "abcdefghijklmnopqrstuvwzyz"

func stringGen(l int, c chan string, exit chan bool) {
	count := 0
	for {
		s := make([]byte, l)
		for i := 0; i < l; i++ {
			s[i] = letters[rand.Intn(len(letters))]
		}
		c <- string(s)
		count++
		if count > 10000 {
			exit <- true
		}
	}
}

func main() {
	primeC := make(chan int)
	stringC := make(chan string)
	exitC := make(chan bool)

	go primeSearcher(10000, primeC)
	go stringGen(50, stringC, exitC)

	for {
		select {
		case prime := <-primeC:
			fmt.Printf("Found prime: %d\n", prime)
		case str := <-stringC:
			fmt.Printf("Got string: %s\n", str)
		case <-exitC:
			return
		}
		//time.Sleep(time.Second / 2)
	}

}
