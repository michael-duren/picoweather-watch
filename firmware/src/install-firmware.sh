#!/bin/bash
set -e

echo "building"
cd build/
cmake ..
make -j4
cd ..

echo "installing"
picotool load build/pico_weather_watch.uf2 -f
picotool reboot

echo "connecting to serial output"
sleep 2
DEVICE=$(find /dev/tty.usbmodem* 2>/dev/null | head -1)
if [ -z "$DEVICE" ]; then
    echo "no pico found"
    exit 1
fi
echo "found pico at $DEVICE"
minicom -b 115200 -D "$DEVICE"
