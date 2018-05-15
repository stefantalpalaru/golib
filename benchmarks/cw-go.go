package main

import (
	"fmt"
	"runtime"
)

func whisper(left, right chan int) {
	left <- 1 + <-right
}

func main() {
	// Go 1.5 defaults to using all CPU cores. gccgo doesn't yet (as of 7.1.0).
	// Set it explicitly to make the benchmarks more fair.
	runtime.GOMAXPROCS(runtime.NumCPU())

	const n = 500000
	leftmost := make(chan int)
	right := leftmost
	left := leftmost
	for i := 0; i < n; i++ {
		right = make(chan int)
		go whisper(left, right)
		left = right
	}
	go func(c chan int) { c <- 1 }(right)
	fmt.Println(<-leftmost)
}
