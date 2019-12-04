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

/* AMG88xx support. Tested against the AMG8833. */

/* for reti() */
#include <avr/interrupt.h>

#include <avr/io.h>

#include "amg88xx.h"

/* for booleans. */
#include <stdbool.h>

#define USE_SERIAL_H
#include "serial.h"

#define USE_TWI_H
#include "twi.h"

void InitAMG()
{
  InitTWI();
}
