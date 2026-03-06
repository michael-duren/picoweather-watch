# picoweather-watch

## Firmware

### Building

#### C

Make sure to have the C raspberry pi sdk installed. Docs [here](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html#your-first-binaries).

```bash
cd `firmware/src/build`
cmake ..
```

#### Go

TODO: Add go docs here

### Debugging

Find the device on your computer:

```bash
ls /dev/tty.*
```

Start `screen`

```bash
#               your dev    bit rate
screen /dev/tty.usbmodem101 115200
```

Or `minicom`

```bash
# install if necessary brew/apt install mincom

# run
minicom -b 115200 -D /dev/tty.usbmodem1101
```

You should see stdout being printed to your terminal.

