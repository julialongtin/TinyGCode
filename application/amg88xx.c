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


#include "serial.h"

void InitAMG()
{
#ifdef USE_TWI_PULLUPS
  /* use the internal pullups to pull up the TWI interface. */
  DDRC = (_BV(DDC5)|_BV(DDC4));
  PORTC = (_BV(PORTC5)|_BV(PORTC4));
#endif
  /* set the clock. */
  TWBR = JustTWBR;
  TWSR = TWPS_MUL;
  /* enable TWI */
  TWCR = (_BV(TWEN));
  putch('Q');
  putch('0'+TWPS_MUL);
}

void setup_twiint(void)
{
  TWCR |= (_BV(TWIE));
}

SIGNAL(__vector_int_twi)
{
  TWCR &= ~(_BV(TWIE));
}

