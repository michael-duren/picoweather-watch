#!/bin/bash
set -e

echo "connecting to serial output"

DEVICE=$(find /dev/tty.usbmodem* 2>/dev/null | head -1)
if [ -z "$DEVICE" ]; then
    echo "no pico found"
    exit 1
fi
echo "found pico at $DEVICE"
screen "$DEVICE" 115200
