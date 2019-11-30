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

/* serial access functions. */

/* for reti() */
#include <avr/interrupt.h>

/* for UBRR0L, UBRR0H, UCSR0A, UCSR0B, TXEN0, RXEN0, UCSR0C, USBS0, UCSZ00, UCSZ01, RXCIE0. */
#include <avr/io.h>

/* for uint8_t. */
#include <stdint.h>

/* for booleans. */
#include <stdbool.h>

/* our header. also included by start.c. */
#include "serial.h"

/* because we're on a harvard archetecture, not a von-neumann machine. */
#include <avr/pgmspace.h>

void setup_rxint(void);
void setup_txint(void);

/* output buffer */
volatile char outbuf[OUTBUFSIZE];

/* our transmission position in the buffer. */
volatile uint8_t outbuftxpos;

/* our writing position in the buffer. */
volatile uint8_t outbufwritepos;

/* initialize the uart.
 * FIXME: make use of U2X0 only when it makes sense. */
void InitUART(void)
{
  UBRR0L = (uint8_t)(CPUFREQ/(BAUD_RATE*8L)-1);
  UBRR0H = (CPUFREQ/(BAUD_RATE*8L)-1) >> 8;
  UCSR0A = _BV(U2X0);
  UCSR0B = _BV(TXEN0)|_BV(RXEN0);
  UCSR0C = _BV(USBS0)|_BV(UCSZ00)|_BV(UCSZ01);
  setup_rxint();
  setup_txint();
}

/* Enable serial interrupts. 
 * note that this does not enable system interrupts, just sets up the serial hardware to generate interrupts.
 */
void setup_rxint(void)
{
  inbufpos=0;
  use_buffer_0 = true;
  buffer_ready = false;
  process_buffer_0 = false;
  UCSR0B |= (1 << RXCIE0);
  putch('I');
}

/* Initializes the variables used for transmitting. */
void setup_txint(void)
{
  outbuftxpos = 0;
  outbufwritepos = 0;
  putch('J');
}

SIGNAL(__vector_int_rx_uart)
{
  volatile char buf = UDR0;
  volatile uint8_t rx_error = UCSR0A & (_BV(FE0) | _BV(DOR0) | _BV(UPE0));
  
  if ( ! rx_error)
    {
      if (buf == '\r')
	{
	  putch (buf);
	  buf = 0;
	  putch ('\n');
	}
      if (use_buffer_0 == true)
	{
	  inbuf0[inbufpos] = buf;
	}
      else
	{
	  inbuf1[inbufpos] = buf;
	}
      if (buf==0)
	{
	  inbufpos = 0;
	  if (buffer_ready == true)
	    {
	      /* a new buffer has completed while the first buffer is being processed. */
	      putch('O');
	      putch('>');
	    }
	  else
	    {
	      process_buffer_0 = use_buffer_0;
	      use_buffer_0 = ! use_buffer_0;
	      buffer_ready = true;
	    }
	}
      else
	{
	  inbufpos++;
	  putch(buf);
	  if (inbufpos > CMD_LEN_MAX)
	    {
	      inbufpos=0;
	      putch ('\r');
	      putch ('\n');
	      putch ('L');
	      putch ('>');
	    }
	}  
    }
}


SIGNAL (__vector_int_tx_uart)
{
  if (outbuftxpos == outbufwritepos)
      UCSR0B &= ~_BV(UDRIE0);
  else
    {
      UDR0 = outbuf[outbuftxpos];
      outbuftxpos++;
      if (outbuftxpos > OUTBUFSIZE)
	outbuftxpos=0;
    }
}

void putch(const unsigned char ch)
{
  outbuf[outbufwritepos]=ch;
  outbufwritepos++;
  if (outbufwritepos > OUTBUFSIZE)
    outbufwritepos = 0;
  if (! (UCSR0B & _BV(UDRIE0)))
    UCSR0B |= _BV(UDRIE0);
}

uint8_t my_strlen_P (const unsigned char * str)
{
  uint8_t i;
  unsigned char j = 1;
  for (i=0; j != 0; i++)
    {
      j=pgm_read_byte(&(str[i]));
    }
		  
  return i-1;
}

uint8_t my_strlen_M (const volatile unsigned char * str)
{
  uint8_t i;
  unsigned char j = 1;
  for (i=0; j != 0; i++)
    {
      j=str[i];
    }
  return i-1;
}

uint8_t getBufRemainder()
{
  uint8_t txbufremainder;
  if (outbufwritepos > outbuftxpos)
    {
      txbufremainder = outbufwritepos - outbuftxpos;
    }
  else
    if (outbufwritepos == outbuftxpos)
      {
	if (UCSR0B & _BV(UDRIE0))
	  {
	    txbufremainder = OUTBUFSIZE;
	  }
	else
	  txbufremainder = 0;
      }
    else
      txbufremainder = OUTBUFSIZE - (outbufwritepos - outbuftxpos);
  return  txbufremainder;
}

uint8_t puts_P(const unsigned char str [])
{
  uint8_t txbufremainder;
  uint8_t i = 0;
  uint8_t strlen;

  txbufremainder = getBufRemainder();
  strlen=my_strlen_P(str);
  
  if (strlen > txbufremainder)
    {
      if (txbufremainder > 4)
	{
	  putch('<');
	  putch('0' + strlen);
	  putch('0' + txbufremainder);
	  putch('>');
	}
    }
  else
    for (i=0; i<strlen; i++)
      {
	putch(pgm_read_byte(&(str[i])));
      }
  return i;
}

uint8_t puts_M(const volatile unsigned char str [])
{
  uint8_t txbufremainder;
  uint8_t i = 0;
  uint8_t strlen;

  txbufremainder = getBufRemainder();
  strlen=my_strlen_M(str);
  
  if (strlen > txbufremainder)
    {
      if (txbufremainder > 4)
	{
	  putch('<');
	  putch('0' + strlen);
	  putch('0' + txbufremainder);
	  putch('>');
	}
    }
  else
    for (i=0; i<strlen; i++)
      {
	putch(str[i]);
      }
  return i;
}
/* to be called before interrupts are set up. */
void init_putch(unsigned char ch)
{
  while (!(UCSR0A & _BV(UDRE0)))
    ;
  UDR0 = ch;
}
