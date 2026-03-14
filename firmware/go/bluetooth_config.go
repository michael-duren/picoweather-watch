//go:build bluetooth

package main

// Edit these values before building with the bluetooth tag.
const (
	BluetoothDeviceName = "PicoWeather"
	// Service UUID for the weather data service
	ServiceUUID = "12345678-1234-5678-1234-56789abcdef0"
	// Characteristic UUID for temperature/humidity data
	DataCharUUID = "12345678-1234-5678-1234-56789abcdef1"
)
