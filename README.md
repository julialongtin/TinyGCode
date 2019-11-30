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

At the moment, the only supported board is the arduino nano, with the atmega328p

## Supported sensors:

AMG8833 infrared sensor

## Building

### Dependencies

'''
sudo apt install make gcc-avr avrdude avr-libc
'''

### Compiling:

'''
cd application
make
'''

## Using

# Bootloader

As this is meant to go on a dev board, we recommend you use our preferred bootloader(OptiBoot).

