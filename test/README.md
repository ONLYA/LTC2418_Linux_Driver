# LTC2418 ADC SPI test script
This contains a test script to get the reading of a specific single-ended channel. Please note that the output value is a plain number converted from the 23-bit data and 1-bit polarity sign.

## How to use it?
Please ensure that you do not have `spi-tools` package installed.

1. `git clone https://github.com/cpb-/spi-tools`. Follow the instructions to build and install the tools.
2. `chmod +x test.py`
3. Execute the script.

## Usage
```
./test.py <channel number> [--loop] [--rotate]
```

The `channel number` is within the range of 0-15. Add `--loop` option to get the read of the channel repeatedly.

`--rotate` option is to scan-read all 16 channels. Add `--loop` option to get the rotated results repeatedly.
