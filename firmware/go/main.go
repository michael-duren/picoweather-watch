package main

import (
	"time"

	"machine"

	"github.com/michael-duren/picoweather-watch/firmware/go/logger"
	"tinygo.org/x/drivers/dht"
)

// PICO W GPIO11 -> 15 on board
const DHTPin = machine.GP11

const ReadInterval = 3 * time.Second

func main() {
	// allow USB serial to initialize
	time.Sleep(2 * time.Second)

	logger.Log("initializing picoweather-watch")

	sensor := dht.New(DHTPin, dht.DHT11)

	if err := initSender(); err != nil {
		logger.Log("sender init failed:", err.Error())
		panic(err)
	}

	logger.Log("initialization complete, starting read loop")

	for {
		logger.Log("reading from sensor...")

		if err := sensor.ReadMeasurements(); err != nil {
			logger.Log("dht read failed:", err.Error())
			time.Sleep(ReadInterval)
			continue
		}

		temp, err := sensor.Temperature()
		if err != nil {
			logger.Log("temperature read failed:", err.Error())
			time.Sleep(ReadInterval)
			continue
		}

		humidity, err := sensor.Humidity()
		if err != nil {
			logger.Log("humidity read failed:", err.Error())
			time.Sleep(ReadInterval)
			continue
		}

		logger.Log("raw temp (m°C):", temp, "raw humidity (m%):", humidity)

		// Convert to degrees C and percent
		tempC := temp / 1000
		humidityPercent := humidity / 1000

		logger.Log("temp (°C):", tempC, "humidity (%):", humidityPercent)

		if err := send(temp, humidity); err != nil {
			logger.Log("send failed:", err.Error())
		}

		time.Sleep(ReadInterval)
	}
}
