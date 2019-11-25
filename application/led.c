/* tinyGCode -- An experimental GCODE interpreter

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
*/

/* LED support. */

#ifndef HAS_CASE_LIGHT
#error must have HAS_CASE_LIGHT enabled!
#endif

#ifndef LED_PIN
#error must specify the digital pin the LED is on with LED_PIN!
#endif

#include <avr/io.h>

/* FIXME: support 0-255 brightness adjustment. */

/* assumes we are the only one doing digital I/O. */
void InitLED(void)
{
  DDRB = 1<<(LED_PIN-8);
}

void caseLightOn()
{
  PORTB = 1<<(LED_PIN-8);
}

void caseLightOff()
{
  PORTB = 0;
}
