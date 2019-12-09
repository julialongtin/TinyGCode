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

AMG8833 infrared sensor

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

 * Type 'M105' to get the temperature of the AMG88xx's thermistor.
 * Type 'M355 S1 P255' to turn on the nano's LED.
 * Type 'M111 S0' to turn echo off.
 * Type 'M240' to recieve the current AMG88xx camera feed as a json object. For instance:

```
"image_as_json":{ "p0":"5.4375", "p1":"5.4375", "p2":"5.375", "p3":"5.6875", "p4":"5.625", "p5":"5.375", "p6":"5.25", "p7":"5.25", "p8":"5.125", "p9":"5.1875", "p10":"5.125", "p11":"5.375", "p12":"5.4375", "p13":"5.25", "p14":"5.125", "p15":"5.1875", "p16":"4.9375", "p17":"5", "p18":"5.0625", "p19":"5.0625", "p20":"5.1875", "p21":"5", "p22":"5.125", "p23":"5.1875", "p24":"5.25", "p25":"5", "p26":"5.0625", "p27":"5.125", "p28":"5", "p29":"4.9375", "p30":"5.0625", "p31":"5.1875", "p32":"4.875", "p33":"5.0625", "p34":"5", "p35":"4.9375", "p36":"5", "p37":"5.0625", "p38":"5.1875", "p39":"5.125", "p40":"4.9375", "p41":"5", "p42":"5", "p43":"5.0625", "p44":"5.0625", "p45":"5", "p46":"5.125", "p47":"5.0625", "p48":"5", "p49":"4.875", "p50":"5.0625", "p51":"5", "p52":"4.9375", "p53":"4.9375", "p54":"5.0625", "p55":"5.0625", "p56":"4.9375", "p57":"4.875", "p58":"5.0625", "p59":"4.9375", "p60":"5.0625", "p61":"5.0625", "p62":"4.9375", "p63":"5" }
```

# Bootloader

As this is meant to go on a dev board, we recommend you use our preferred bootloader(OptiBoot). You can select which bootloader you're using by editing application/Makefile .



