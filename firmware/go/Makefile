TARGET = pico

# USB mode (default) - sends data over USB serial
.PHONY: flash-usb
flash-usb:
	tinygo flash -target=$(TARGET) .

# USB mode with debug logging
.PHONY: flash-usb-debug
flash-usb-debug:
	tinygo flash -target=$(TARGET) -tags=debug .

# Bluetooth mode - sends data over BLE to connected devices
# Edit bluetooth_config.go with your device name/UUIDs before building
.PHONY: flash-bluetooth
flash-bluetooth:
	tinygo flash -target=$(TARGET) -tags=bluetooth -stack-size=8kb -scheduler=tasks .

# Bluetooth mode with debug logging
.PHONY: flash-bluetooth-debug
flash-bluetooth-debug:
	tinygo flash -target=$(TARGET) -tags="bluetooth debug" -stack-size=8kb -scheduler=tasks .

# Monitor USB serial output
.PHONY: monitor
monitor:
	tinygo monitor

# Flash and immediately monitor
.PHONY: flash-monitor
flash-monitor:
	tinygo flash -target=$(TARGET) -tags=debug -monitor .
