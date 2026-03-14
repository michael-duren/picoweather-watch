TARGET = pico-w

# USB mode (default) - sends data over USB serial
.PHONY: flash-usb
flash-usb:
	tinygo flash -target=$(TARGET) .

# USB mode with debug logging
.PHONY: flash-usb-debug
flash-usb-debug:
	tinygo flash -target=$(TARGET) -tags=debug .

# WiFi mode - sends data over TCP to configured server
# Edit wifi_config.go with your SSID/password/server before building
.PHONY: flash-wifi
flash-wifi:
	tinygo flash -target=$(TARGET) -tags=wifi -stack-size=8kb -scheduler=tasks .

# WiFi mode with debug logging
.PHONY: flash-wifi-debug
flash-wifi-debug:
	tinygo flash -target=$(TARGET) -tags="wifi debug" -stack-size=8kb -scheduler=tasks .

# Monitor USB serial output
.PHONY: monitor
monitor:
	tinygo monitor

# Flash and immediately monitor
.PHONY: flash-monitor
flash-monitor:
	tinygo flash -target=$(TARGET) -tags=debug -monitor .
