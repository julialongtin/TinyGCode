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
  /* Initialize the AMG. */
  putByteToReg(AMG_PWRCTL, AMG_PWR_NRML, AMG88XX_ADDR);
  putByteToReg(AMG_RST, AMG_RST_INIT, AMG88XX_ADDR);
  putByteToReg(AMG_FRMRATE, AMG_FRM_1FPS, AMG88XX_ADDR);
  putByteToReg(AMG_INTCTL, AMG_INT_DIS, AMG88XX_ADDR);
}

void AMGTEMPtoA(uint16_t temp)
{
  uint8_t digit;
  uint8_t polaritydigits=0;
  uint8_t wholedigits=1;
  uint8_t fracdigits=0;
  uint8_t i=0;
  uint8_t whole = ((temp>>4)&127);
  uint8_t frac = (temp&15);
  uint8_t polarity = (temp>>13);

  if (polarity == 1)
    {
      polaritydigits = 1;
      AMGTEMPA[0] ='-';
      i=1;
    }

  if (whole > 99)
      wholedigits = 3;
  else
    if (whole > 9)
      wholedigits = 2;
    else
      wholedigits = 1;

  if (frac&1)
    fracdigits = 4;
  else
    if (frac&2)
      fracdigits = 3;
    else
      if (frac&4)
	fracdigits = 2;
      else
	if (frac&8)
	  fracdigits = 1;

  for (i=0;i<wholedigits; i++)
    {
      if ((wholedigits-i) == 3)
      {
	digit=((whole&64)&&(whole&32)&&(whole&28))?1:0;
	AMGTEMPA[i+polaritydigits]='0'+digit;
      }
      else
	if ((wholedigits-i) == 2)
	  {
	    digit = mod10(((whole&64)?6:0) + ((whole&32)?3:0) + ((whole&16)?1:0) +
			  (((
			    ((whole&8)&&(whole&16)&&(((whole&64)&&(whole&38))||((whole&4)&&(whole&34))))||
			    (((whole&8)&&(whole&64)&&(whole&4)&&(whole&32)&&(whole&2))))?2:
			    ((((whole&8)&&(whole&118))||((whole&16)&&((whole&68)||((whole&32)&&(whole&2))))||((whole&64)&&(whole&32)&&(whole&4)))?1:0
			     ))));
	    AMGTEMPA[i+polaritydigits]='0'+digit;
	  }
	else
	  if ((wholedigits-i) == 1)
	    {
	      digit = mod10 (((whole&64)?4:0) + ((whole&32)?2:0) + ((whole & 16)?6:0) + (whole&15));
	      AMGTEMPA[i+polaritydigits]='0'+digit;
	    }
    }
  if (fracdigits>0)
    AMGTEMPA[polaritydigits+wholedigits]='.';
  if (fracdigits > 0)
    AMGTEMPA[polaritydigits+wholedigits+1]='0' + ((frac&8)?5:0) + ((frac&4)?2:0) + ((frac&2)?1:0) +
      (((frac&1)&&(frac&4))?1:0);
  if (fracdigits > 1)
    AMGTEMPA[polaritydigits+wholedigits+2]='0' + mod10(((frac&4)?5:0) + ((frac&2)?2:0) + ((frac&1)?6:0));
  if (fracdigits > 2)
    AMGTEMPA[polaritydigits+wholedigits+3]='0' + ((frac&2)?5:0) + ((frac&1)?2:0);
  if (fracdigits > 3)
    AMGTEMPA[polaritydigits+wholedigits+4]='0' + ((frac&1)?5:0);
  AMGTEMPA[polaritydigits+wholedigits+fracdigits+(fracdigits?1:0)]=0;
}
