TinyGCode: a GCode engine for experiments.

Copyright (C) 2019 Julia Longtin

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

To contact the authors, email julia.longtin@gmail.com.

# Application

tinyGCode is a G-Code interpreter, for sensors, and other components.

## Supported boards:

At the moment, the only supported board is the Arduino Nano, populated with an ATmega328p microcontroller.

## Supported sensors:

~AMG8833 infrared sensor~

## Building

### Dependencies:

On debian-alikes:
```
sudo apt install make gcc-avr avrdude avr-libc
```

### Compiling:

```
cd application
make
```

### Programming:
Because of the ability for errors in the fuse setting / flashing process to make a brick of your controller, I have not automated that.

Please select a programmer by editing the Makefile, assuming you're not using the default 'usbtinyisp v2.0'.

#### Fuses
Run ```make showfuses``` to ensure your programmer selection is working.

Run ```make shownewfuses``` to see the fuse values that were calculated from the settings in the makefile. Read them, and understand them before setting them, or you may brick your device!

#### Flashing

To flash optiboot, and then my firmware, i use the following command:
```
make && sudo avrdude -c usbtiny -p atmega328p -u -D -e -U flash:w:../bootloader/optiboot/optiboot/bootloaders/optiboot/optiboot_atmega328.hex && sudo avrdude -c usbtiny -p atmega328p -u -D -U flash:w:tinygcode.hex
```

## Using
Open up your favorite serial gcode manager (mine's pronsole), or:
```
screen /dev/ttyUSB0
```

# Bootloader

As this is meant to go on a dev board, we recommend you use our preferred bootloader(OptiBoot). You can select which bootloader you're using by editing application/Makefile .



