PT6312 is an Arduino/AVR library for the PT6312 family of Vacuum Fluorescent Display (VFD)
controllers including AD16312, HT16512, ET16312 etc.

It allows the full management of control boards of VFD screens extracted in particular
from DVD players by micro-controllers supporting the Arduino development environment.


# Features

It supports all the grids and segments combinations of this familly of controllers:

    4 digits, 16 segments
    5 digits, 16 segments
    6 digits, 16 segments
    7 digits, 15 segments
    8 digits, 14 segments
    9 digits, 13 segments
    10 digits, 12 segments
    11 digits, 11 segments

Additional features:

- Control of 4 LEDs
- Control of 4 keys
- Control of 4 generic switches


# Wiring

Only 3 pins are required from the MCU to communicate with the VFD driver :
CS/STB (Cable Select/Strobe), SCLK and DATA (IN/OUT).
Ex for the ATtiny85: PB0 (CS/STB), PB1 (SCLK), PB2 (DATA).

The communication is based on SPI protocol, but since data transfer is never simultaneously
in both directions, MISO and MOSI pins are often connected to each other to save a wire connection.

Thus a small AVR like the ATtiny85 is enough to control the display.

=> TODO wiring schema

On the controller side (from the PT6312 datasheet):

- VCC: 5V
- VEE: 0 to VCC-30V DC*

The filament must be connected to a negative power rail: ~-1.2V DC* is acceptable.

\* Negative voltages can be obtained very easily by using a very common LM2596 based
DC-DC converter (See their datasheet).


# Configuration

## Library configuration

The main options are placed in the global.h file.

You will find there :
- the definition of the pins to use (For the ATtiny85: PB0 (CS/STB), PB1 (SCLK), PB2 (DATA)),
- the characteristics of the screen used (number of grids, number of displayable characters),
- and options related to the library (scrolling speed, use of a buffer dedicated to the usage of icons that can be activated on demand to save space).

## Screen configuration

The existing layouts & implementations are in the [src/display_variants/](src/display_variants/) folder.
The selection of a configuration is done in global.h.

However it is unlikely that the character layout supported by your screen would be directly supported.

It will be necessary to create a specific font file (correspondence table between
displayable character and segments to be activated).

A second file containing specific functions of the screen can be made.
The functions concerned are `VFD_writeString()` and `VFD_busySpinningCircle()`.


The other functions of the library are generic. `VFD_segmentsGenericTest()` will be able to
for example, help to identify the number of segments on the screen, leaving enough time to take
notes.


Here is a short function in Python to generate the 2 bytes of a character from a list of active bits,
ready to be inserted in the `FONT` array :

```python
def convert_to_bits(*args):
    # MSB
    msb = "0b" + "".join(map(str, [1 if i in args else 0 for i in range(16, 8, -1)]))
    # LSB
    lsb = "0b" + "".join(map(str, [1 if i in args else 0 for i in range(8, 0, -1)]))

    print(f"{{{msb}, {lsb}}}")

# char 0: bits: 8, 7, 6, 4, 3, 2
convert_to_bits(8, 7, 6, 4, 3, 2)
# Gives: {0b00000000, 0b11101110}
```


# Functions







# Examples

A full clock example is available at [examples/test_attiny_vfd/test_attiny_vfd.ino](examples/test_attiny_vfd/test_attiny_vfd.ino).

```c++
// Test all segments
VFD_displayAllSegments();
_delay_ms(1000);

// Test/debug display segments from number 1 to number 16
VFD_segmentsGenericTest()
VFDClear();

// Enable LED output
VFD_setLEDs(PT6312_LED3);

// Set specific icons (Do not forget to enable ENABLE_ICON_BUFFER in the library config)
VFD_setIcon(ICON_DVD);
VFD_setIcon(ICON_PLAY);
VFD_setIcon(ICON_DOLBY);

// Write a big signed int
// This one uses 7 characters it will be cropped to 6: -12345
// The colon symbol will be displayed if available
VFD_home();
VFD_writeInt(-123456, 6, true);
```


# FAQ

## Does this work with other Princeton VFD controllers like the PT6311?

This controller has more memory to address more segments per grid (20 instead of 16
for the AD16312 controller family).
Therefore instead of sending 2 bytes (16 bits) per grid, you have to send 2.5 bytes (20 bits).

The PT6311 can also manage 5 LEDs instead of 4.

Except these minor differences, the protocol is exactly the same.
Thus as it is, the library does not support this controller.
However, it is enough to rewrite some functions to make it possible since the
work is already well advanced.

Any contribution is welcome!


# Contributing

Any contribution is welcome!
You can open an issue or propose a pull request.


# PT6312 is free AND open source

PT6312 is released under the GPL (GNU General Public License).
You can read, test and modify its code.


# Credits

Thanks to Istrate Liviu for the first base of the project on his blog
<https://www.programming-electronics-diy.xyz/2017/06/salvage-vfd-from-broken-dvd.html>.

Also thanks to the website <https://os.mbed.com/users/wim/notebook/vacuum-fluorescent-display-vfd-drivers/>
for their precious information.