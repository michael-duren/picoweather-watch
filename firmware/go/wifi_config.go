//go:build wifi

package main

// Edit these values before building with the wifi tag.
const (
	WifiSSID     = "YOUR_SSID"
	WifiPassword = "YOUR_PASSWORD"
	ServerAddr   = "192.168.1.100:9000" // TCP endpoint for weather data
	Hostname     = "picoweather"
)
