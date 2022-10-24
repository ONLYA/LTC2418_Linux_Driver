# LTC241X Linux driver

## Introduction
This contains the generic SPI driver and the LTC2414/LTC2418 SPI ADC chip using the SPI API. `main.c` is the test program to implement the ADC read to-the-go.

## SPI driver

## Device Tree Initialisation
In my case, I am using Armbian Debian Linux. According to the [Official Documentation](https://docs.armbian.com/User-Guide_Allwinner_overlays/), SPI1 with a specific CS select pin 0 and SPI frequency is overlayed to the original device tree. You can then find `/dev/spidev1.0` file after the reboot.

## SPI API
The provided Linux kernel headers contain SPI API: `linux/spi/spidev.h`. You can find it under `/usr/include/linux/spi/spidev`.

To manipulate the SPIdev device `/dev/spidev1.0`, the system IOCTL API is used. The include is `sys/ioctl.h`.

The API contains the init function with a config struct, read, write and sync functions with continuous packet number parameter.

## LTC241X API
This uses the `spi.h` API provided as the SPIdev backend.

In order to change the ADC chip between LTC2414 and LTC2418, change the line in `ltc241x.h` as either of the following:
```
#define LTC241x LTC2414 // For LTC2414
#define LTC241x LTC2418 // For LTC2418
```

There are init, offset calibration and read functions defined.

Please noted that there is maximum 166ms conversion time with the internal oscillator accordint to the datasheet, the time interval between two reads should be at lease 170ms. It is also observed that too fast reading can make the result inaccurate so at least 300ms time interval between two reads is suggested.

## Test program `main.c`

### Build
To build the program, run `make`. Clean the solution with `make clean`. 

(Optional, `make debug` to build the debug version for gdb. `make verbose` to build the debug version with more detailed prints to stdout.)

To run the program, run `./main`.

### Behaviour
1. Init the SPIdev bus for LTC2418.
2. Calibrate the offset of all analogue channels.
3. Read channel n.
4. Sleep for 500ms.
5. Repeat Step 3.

## LICENSE
```
The MIT License (MIT)
Copyright © 2022 Xiaosheng An

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```