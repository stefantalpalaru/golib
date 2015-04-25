package main

import (
	"fmt"
	"runtime/debug"
)

func whisper(left, right chan int) {
	left <- 1 + <-right
}

func main() {
	debug.SetGCPercent(-1) // disable the GC, like we do for the golib version

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
