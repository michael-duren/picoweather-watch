//go:build !wifi

package main

import "machine"

func initSender() error {
	// USB serial is auto-initialized by TinyGo
	return nil
}

// send writes a framed binary packet over USB serial.
// Protocol matches the C firmware: {0xAA, temp, humidity, 0x55}
// where temp is degrees C and humidity is percent RH.
func send(temperature, humidity int32) error {
	tempByte := byte(temperature / 1000) // millidegrees -> degrees
	humByte := byte(humidity / 1000)     // millipercent -> percent

	packet := [4]byte{0xAA, tempByte, humByte, 0x55}
	_, err := machine.Serial.Write(packet[:])
	return err
}
