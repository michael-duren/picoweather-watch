//go:build bluetooth

package main

import (
	"strconv"
	"time"

	"machine"

	"github.com/michael-duren/picoweather-watch/firmware/go/logger"
	"tinygo.org/x/bluetooth"
)

var (
	adapter     = bluetooth.DefaultAdapter
	bleDevice   bluetooth.Device
	dataChar    bluetooth.Characteristic
	isConnected bool
)

func initSender() error {
	logger.Log("initializing Bluetooth")

	// Enable the Bluetooth adapter
	err := adapter.Enable()
	if err != nil {
		return err
	}

	// Define the Bluetooth service and characteristic
	adv := adapter.DefaultAdvertisement()

	// Configure advertisement with device name
	err = adv.Configure(bluetooth.AdvertisementOptions{
		LocalName: BluetoothDeviceName,
	})
	if err != nil {
		return err
	}

	// Add service
	var svcUUID bluetooth.UUID
	svcUUID, err = bluetooth.ParseUUID(ServiceUUID)
	if err != nil {
		return err
	}

	var charUUID bluetooth.UUID
	charUUID, err = bluetooth.ParseUUID(DataCharUUID)
	if err != nil {
		return err
	}

	err = adapter.AddService(&bluetooth.Service{
		UUID: svcUUID,
		Characteristics: []bluetooth.CharacteristicConfig{
			{
				UUID:   charUUID,
				Value:  make([]byte, 32),
				Flags:  bluetooth.CharacteristicReadPermission | bluetooth.CharacteristicNotifyPermission,
				Handle: &dataChar,
			},
		},
	})
	if err != nil {
		return err
	}

	// Start advertising
	err = adv.Start()
	if err != nil {
		return err
	}

	logger.Log("Bluetooth initialized, advertising as:", BluetoothDeviceName)

	// Blink LED to signal successful Bluetooth initialization
	led := machine.LED
	led.Configure(machine.PinConfig{Mode: machine.PinOutput})
	led.High()
	time.Sleep(time.Second)
	led.Low()

	return nil
}

// send transmits temperature and humidity over Bluetooth.
// Data is sent as "T:<degrees>:H:<percent>\n" text format.
func send(temperature int16, humidity uint16) error {
	tempC := temperature / 1000
	humPct := humidity / 1000
	msg := "T:" + strconv.Itoa(int(tempC)) + ":H:" + strconv.Itoa(int(humPct)) + "\n"

	// Write to the characteristic
	_, err := dataChar.Write([]byte(msg))
	if err != nil {
		return err
	}

	return nil
}
