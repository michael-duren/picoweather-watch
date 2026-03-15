package main

import (
	"fmt"
	"path/filepath"
)

func main() {
	matches, err := filepath.Glob("/dev/tty.usbmodem*")
	if err != nil {
		fmt.Println("error:", err)
		return
	}
	for _, m := range matches {
		fmt.Println(m)
	}
}
