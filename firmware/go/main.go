package main

import (
	"machine"
	"time"

	"github.com/michael-duren/picoweather-watch/firmware/go/logger"
	"tinygo.org/x/drivers/dht"
)

// PICO W GPIO11 -> 15 on board
const DHT_PIN = machine.GP11

// func init() {
// 	logger.Log("initializing architecture")
// 	sensor 
// }

func main() {
	logger.Log("hey buddy")
	led := machine.LED
	led.Configure(machine.PinConfig{Mode: machine.PinOutput})
	dht.New(DHT_PIN, dht.DHT11)

	for {
		led.High()
		time.Sleep(time.Second / 2)

		led.Low()
		time.Sleep(time.Second / 2)
	}
}
