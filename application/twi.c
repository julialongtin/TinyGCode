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

/* Two Wire Interface. */

/* for reti() */
#include <avr/interrupt.h>

#include <avr/io.h>


#include <util/twi.h>
#include "twi.h"

/* for booleans. */
#include <stdbool.h>

#define USE_SERIAL_H
#include "serial.h"

void InitTWI()
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
  setup_twiint();
}

volatile uint8_t failure_code;

void setup_twiint(void)
{
  twi_state=0;
  SLADDROP=0;
  failure_code=0;
}

SIGNAL(__vector_int_twi)
{
  uint8_t foundSR=TWSR&0xF8;
  switch (twi_state)
    {
    case TWS_MT_START:
      {
	if (foundSR == TW_START)
	  {
	    TWDR = SLADDROP;
	    TWCR = (_BV(TWINT)|_BV(TWIE)|_BV(TWEN));
	    twi_state=TWS_MT_SLAACK;
	    return;
	  }
	else
	  goto fail;
      }
    case TWS_MT_SLAACK:
      {
	if (foundSR == TW_MT_SLA_ACK)
	  {
	    TWDR = twi_register;
	    TWCR = (_BV(TWINT)|_BV(TWIE)|_BV(TWEN));
	    twi_state = TWS_MT_REGISTERACK;
	    return;
	  }
	else
	  goto fail;
      }
    case TWS_MT_REGISTERACK:
      {
	if (foundSR == TW_MT_DATA_ACK)
	  {
	    TWDR = twi_data;
	    TWCR = (_BV(TWINT)|_BV(TWIE)|_BV(TWEN));
	    twi_state=TWS_MT_END;
	    return;
	  }
	else
	  goto fail;
      }
    case TWS_MT_END:
      {
	if (foundSR == TW_MT_DATA_ACK)
	  {
	    TWCR = (_BV(TWINT)|_BV(TWSTO)|_BV(TWEN)|_BV(TWIE));
	    twi_state=TWS_IDLE;
	    return;
	  }
	else
	  goto fail;
      }
    }
  switch (twi_state)
    {
    case TWS_MR_START:
      {
	if (foundSR == TW_START)
	  {
	    TWDR = SLADDROP;
	    TWCR = (_BV(TWINT)|_BV(TWIE)|_BV(TWEN));
	    twi_state=TWS_MR_SLAACK;
	    return;
	  }
	else
	  goto fail;
      }
    case TWS_MR_SLAACK:
      {
	if (foundSR == TW_MT_SLA_ACK)
	  {
	    TWDR = twi_register;
	    TWCR = (_BV(TWINT)|_BV(TWIE)|_BV(TWEN));
	    twi_state=TWS_MR_REGISTERACK;
	    return;
	  }
	else
	  goto fail;
      }
    case TWS_MR_REGISTERACK:
      {
	if (foundSR == TW_MT_DATA_ACK)
	  {
	    TWCR = (_BV(TWINT)|_BV(TWSTA)|_BV(TWIE)|_BV(TWEN));
	    twi_state=TWS_MR_START2;
	    return;
	  }
	else
	  goto fail;
      }
    }
  switch (twi_state)
    {
    case TWS_MR_START2:
      {
	if (foundSR == TW_REP_START)
	  {
	    TWDR = SLADDROP2;
	    TWCR = (_BV(TWINT)|_BV(TWIE)|_BV(TWEN));
	    twi_state=TWS_MR_SLAACK2;
	    return;
	  }
	else
	  goto fail;
      }
    case TWS_MR_SLAACK2:
      {
	if (foundSR == TW_MR_SLA_ACK)
	  {
	    TWDR = twi_register;
	    TWCR = (_BV(TWINT)|_BV(TWIE)|_BV(TWEN)|_BV(TWEA));
	    twi_state=TWS_MR_DATAINCOMING;
	    return;
	  }
	else
	  goto fail;
      }
    case TWS_MR_DATAINCOMING:
      {
	if (foundSR == TW_MR_DATA_ACK)
	  {
	    twi_ret[twi_readpos]=TWDR;
	    twi_readpos++;
	    if (twi_readpos >= twi_readbytes)
	      {
		/* last byte */
		TWCR = (_BV(TWINT)|_BV(TWIE)|_BV(TWEN));
		twi_state=TWS_STOP;
	      }
	    else
	      {
		TWCR = (_BV(TWINT)|_BV(TWIE)|_BV(TWEN)|_BV(TWEA));
	      }
	    return;
	  }
	else
	  goto fail;
      }
    case TWS_STOP:
      {
	TWCR = (_BV(TWINT)|_BV(TWSTO)|_BV(TWEN));
	twi_state=TWS_IDLE;
	return;
      }
    }
 fail:
  failure_code=foundSR;
  TWCR = (_BV(TWINT)|_BV(TWSTO)|_BV(TWEN));
  twi_state=TWS_ERROR;
}

void twi_wait_idle()
{
  while ((twi_state != TWS_IDLE) && (twi_state != TWS_ERROR))
    {
      SMCR = _BV(SE);
      asm volatile(
		   "\tsleep\n"
		   );
      SMCR=0;
    }
  if (twi_state == TWS_ERROR)
    {
      putch('0'+failure_code);
      putch('!');
    }
}

void putByteToReg(const uint8_t byte, const uint8_t reg, const uint8_t target)
{
  twi_wait_idle();
  twi_state=TWS_MT_START;
  SLADDROP=(uint8_t)(target<<1|TW_WRITE);
  twi_register=reg;
  twi_data=byte;
  TWCR = (_BV(TWINT)|_BV(TWSTA)|_BV(TWEN)|_BV(TWIE));
  twi_wait_idle();
}

uint8_t getBytesFromReg(const uint8_t reg, const uint8_t bytes, const uint8_t target)
{
  twi_wait_idle();
  twi_state=TWS_MR_START;
  SLADDROP=(uint8_t)(target<<1|TW_WRITE);
  SLADDROP2=(uint8_t)(target<<1|TW_READ);
  twi_readbytes=bytes;
  twi_readpos=0;
  twi_register=reg;
  TWCR = (_BV(TWINT)|_BV(TWSTA)|_BV(TWEN)|_BV(TWIE));
  twi_wait_idle();
  return twi_readpos;
}
