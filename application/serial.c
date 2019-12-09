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
  uint8_t ucsr0c=0;
  /* set the clock. */
  UBRR0L = (uint8_t)(CPUFREQ/(BAUD_RATE*8L)-1);
  UBRR0H = (CPUFREQ/(BAUD_RATE*8L)-1) >> 8;
  /* and the multiplier. */
  UCSR0A = _BV(U2X0);
  /* enable the port. */
  UCSR0B = _BV(TXEN0)|_BV(RXEN0);
  /* set the frame format. */
  if (STOP_BITS == 2)
      ucsr0c = _BV(USBS0);
  if (CHARACTER_WIDTH == 8)
    ucsr0c = ucsr0c + (_BV(UCSZ00)|_BV(UCSZ01));
  else
    ucsr0c = ucsr0c + _BV(UCSZ01);
  UCSR0C=ucsr0c;
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
  do_echo = true;
  UCSR0B |= (1 << RXCIE0);
  putch('I');
}

/* Initializes the variables used for transmitting. */
void setup_txint(void)
{
  outbuftxpos = 0;
  outbufwritepos = 1;
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
	  buf = 0;
	  if (do_echo)
	    {
	      putch_i ('\r');
	      putch_i ('\n');
	    }
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
	      if (! do_echo)
		putch_i('<');
	      putch_i('O');
	      putch_i('>');
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
	  if (do_echo)
	    putch_i(buf);
	  inbufpos++;
	  if (inbufpos > CMD_LEN_MAX)
	    {
	      inbufpos=0;
	      if (do_echo)
		{
		  putch_i ('\r');
		  putch_i ('\n');
		}
	      else
		putch_i ('<');
	      putch_i ('L');
	      putch_i ('>');
	    }
	}  
    }
}


/* outbufwritepos is always one characher ahead of the most recently written character, or 0 when
 * the most recently written character is at outbufsize-1.
 */

SIGNAL (__vector_int_tx_uart)
{
  UDR0 = outbuf[outbuftxpos]; /* outbuf[outbuftxpos]; */
  if (outbuftxpos+1 == OUTBUFSIZE)
    {
      outbuftxpos=0;
      if (outbufwritepos == 0)
	UCSR0B ^= _BV(UDRIE0);
    }
  else
    {
      outbuftxpos++;
      if (outbuftxpos == outbufwritepos)
	UCSR0B ^= _BV(UDRIE0);
    }
}

/* must always be called with interrupts disabled. */
void putch_c(void)
{
  if (outbufwritepos+1 == OUTBUFSIZE)
    outbufwritepos = 0;
  else
    outbufwritepos++;
}

void putch(const unsigned char ch)
{
  cli();
  outbuf[outbufwritepos]=ch;
  putch_c();
  sei();
  if (! (UCSR0B & _BV(UDRIE0)))
    UCSR0B |= _BV(UDRIE0);
}

void putch_i(const unsigned char ch)
{
  outbuf[outbufwritepos]=ch;
  putch_c();
  if (! (UCSR0B & _BV(UDRIE0)))
    UCSR0B |= _BV(UDRIE0);
}

/* to be called before interrupts are set up. */
void init_putch(unsigned char ch)
{
  while (!(UCSR0A & _BV(UDRE0)))
    ;
  UDR0 = ch;
}

uint8_t getBufRemainder()
{
  if (outbufwritepos == outbuftxpos)
    return OUTBUFSIZE;
  if (outbufwritepos > outbuftxpos)
    return OUTBUFSIZE - (outbufwritepos - outbuftxpos);
  else
    /* we wrapped around. */
    return OUTBUFSIZE - (outbuftxpos - outbufwritepos);
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

uint8_t puts_P(const unsigned char str [])
{
  uint8_t txbufremainder;
  uint8_t i = 0;
  uint8_t strlen;

  strlen=my_strlen_P(str);
  txbufremainder = getBufRemainder();
  
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
    {
      cli();
      for (i=0; i<strlen; i++)
	{
	  putch_i(pgm_read_byte(&(str[i])));
	}
      sei();
    }
  return i;
}

uint8_t puts_M(const volatile unsigned char str [])
{
  uint8_t txbufremainder;
  uint8_t i = 0;
  uint8_t strlen;

  strlen=my_strlen_M(str);
  txbufremainder = getBufRemainder();
  
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
    {
      cli();
      for (i=0; i<strlen; i++)
	{
	  putch(str[i]);
	}
      sei();
    }
  return i;
}
