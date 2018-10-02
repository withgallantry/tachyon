Adafruit_ADS1015
================

Driver for TI's ADS1015: 12-bit Differential or Single-Ended ADC with PGA and Comparator

Linux Fork
==========
This fork is a modified version of the [Adafruit's one](https://github.com/adafruit/Adafruit_ADS1X15) to be used directly through the i2c bus of a Linux board like a RaspberryPi, pcDuino, Odroid...

The implementation uses the Linux SMBus interface for the i2c communication. So the `linux/i2c-dev.h` header is needed. On Ubuntu, you only have to install the `i2c-tools` and `libi2c-dev` packages:

        $ sudo apt-get install i2c-tools libi2c-dev

You may also need to activate the Linux i2c module on your board.

Build
-----
Thanks to the `Makefile`, you have only to enter `make` in order to build the static library and the examples:

        $ make
