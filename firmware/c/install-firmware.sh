#!/bin/bash
set -e

DEBUG=false

while getopts "D" arg; do
    case $arg in
    D)
        DEBUG=true
        ;;
    *)
        echo "Usage is :-D for debug or no flags"
        ;;
    esac
done

echo "building"
cd build/
if [[ $DEBUG == true ]]; then
    cmake -DENABLE_DEBUG_LOGGING=ON ..
else
    cmake ..
fi
make -j4
cd ..

echo "installing"
# Hold BOOTSEL button when plugging in Pico first
cp build/pico_weather_watch.uf2 /Volumes/RPI-RP2/

# echo "connecting to serial output"
# sleep 2
# DEVICE=$(find /dev/tty.usbmodem* 2>/dev/null | head -1)
# if [ -z "$DEVICE" ]; then
#     echo "no pico found"
#     exit 1
# fi
# echo "found pico at $DEVICE"
# minicom -b 115200 -D "$DEVICE"
