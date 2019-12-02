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

/* GCODE interpreter */

/* conforming to: https://reprap.org/wiki/G-code */

/* with the following exceptions: */

/* GCODE for CASE LIGHT:
   M355 S1 P255
   where S is the switch (on, off), and P is the duty cycle (0-255)
   Complying with http://marlinfw.org/docs/gcode/M355.html
*/

/* for SPL, SPH. */
#include <avr/io.h>

/* for uint8_t. */
#include <stdint.h>

/* for booleans. */
#include <stdbool.h>

/* for putch. */
#define USE_SERIAL_H 1
#include "serial.h"

#ifdef HAS_CASE_LIGHT
#define USE_LED_H 1
#include "led.h"
#endif

#ifdef HAS_AMG88XX
#define USE_AMG88XX_H
#include "amg88xx.h"
#endif

/* because we're on a harvard archetecture, not a von-neumann machine, we need to access program memory
 * differently than ram.
 */
#include <avr/pgmspace.h>

void process_gcode(volatile const unsigned char *);

void app_start(uint8_t entry)
{
  /* Set up for running C. */

  /* zero the zero register. */
  asm volatile(
	       "\tclr  __zero_reg__\n"
	       );

  /* set the stack pointer. */
  SPL = (RAMEND-1)&0xFF;
  SPH = (RAMEND-1)>>8;

  /* shut down clocks to unneeded components. */
#ifdef USE_TWI
  PRR = (uint8_t)~(_BV(PRUSART0)|_BV(PRTWI));
#else
  PRR = ~(_BV(PRUSART0));
#endif
  
  if (entry == 0)
    {
      // setup serial communications
      InitUART();
#ifdef HAS_CASE_LIGHT
      // set up our LED.
      InitLED();
#endif
#ifdef HAS_AMG88XX
      // set up our image sensor.
      InitAMG();
#endif
      /* Enable interrupts. */
      init_putch('J');
      MCUCR = _BV(IVCE);
      MCUCR = 0;
      init_putch('K');
      asm volatile(
		   "\tsei\n"
		   );
  
    }

  /* say we exist. */
  init_putch('A');
  init_putch('0'+entry);
  /* display the reset reason, and reset it. */
  init_putch('R');
  init_putch('0'+MCUSR);
  MCUSR=0;
  /* disable the watchdog timer. */
  WDTCSR=0;
  /* new line. */
  init_putch('\r');
  init_putch('\n');

  /* the inner loop of our program. all work is done in the ISRs, so this should only sleep. */
  for (;;)
    {
      /* Enter sleep mode. */
      SMCR = _BV(SE);
      asm volatile(
		   "\tsleep\n"
		   );
      if (buffer_ready == true)
	{
	  if (process_buffer_0 == true)
	    process_gcode(inbuf0);
	  else
	    process_gcode(inbuf1);
	  putch('>');
	  putch(' ');
	  buffer_ready = false;
	}
      SMCR = 0;
    };
}

uint16_t E(const uint8_t tens)
{
  uint16_t res = 1;
  uint8_t i;
  for (i=0; i< tens; i++)
    {
      res=res * 10;
    }
  return res;
}

uint16_t divU16(uint16_t dividend, uint16_t divisor) {
  uint16_t quotient = 0;
  while (dividend >= divisor) {
    dividend -= divisor;
    quotient++;
  }
  return quotient;
}

/* read a bool. 1 or 0. */
bool matchBoolNum(volatile const unsigned char * src)
{
  if (src[0] == '1')
    return true;
  else
    return false;
}
      
/* read a string of one to three characters. GCODE */ 
uint16_t matchto3num(volatile const unsigned char * src)
{
  uint8_t i = 0;
  uint8_t count = 0;
  uint16_t res = 0;
  while ((src[count] >= '0') && (src[count] <= '9'))
    count++;
  while ((src[i] >= '0') && (src[i] <= '9'))
    {
      count--;
      res = res + ((src[i]-'0') * E(count));
      i++;
    }
  return res;
}

volatile const unsigned char * skipNum(volatile const unsigned char * src)
{
  uint8_t count=0;
  while ((src[count] >= '0') && (src[count] <= '9'))
    count++;
  return &src[count];
}

volatile const unsigned char * skipSpc(volatile const unsigned char * src)
{
  uint8_t count=0;
  while (src[count] == ' ')
    count++;
  return &src[count];
}

volatile unsigned char U16A[6];
void U16toA(uint16_t val)
{
  uint8_t digits;
  uint8_t digit;
  uint8_t i=0;
  uint8_t c1;
  /*  uint8_t c2; */
  uint16_t handled=0;
  if (val > 9999)
    digits = 5;
  else
    if (val > 999)
      digits = 4;
    else
      if (val > 99)
	digits = 3;
      else
	if (val > 9)
	  digits = 2;
	else
	  digits = 1;
  for (i=0; i<digits; i++)
    {
      /* for the first digit, bit 16 contributes 3(2768), bit 15 contributes 1(6384), bit 14 contributes (8192), bit 15 contributes (4096) and bit 14 contributes (2048).. */
      if ((digits-i) == 5)
	{
	  c1 = val >> 11;
	  digit=((c1&16)?3:0) + ((c1&8)>>3) + (((c1&4)&&(c1&8)&&(c1&2)&&(c1&17))?2: ((c1&4)&&(c1&27))?1: (((c1&8)&&((c1&2)||((c1&16)&&(c1&1)))))?1:0);
	  handled=digit*10000;
	  U16A[i]= '0' + digit;
	}
      else
	if ((digits-i) >0)
	  {
	    digit=divU16(val-handled, 1*E(digits-i-1));
	    handled=handled+digit*E(digits-i-1);
	    U16A[i] = '0' + digit;
	  }
	else
	  U16A[i] = 0;
    }
}

void process_gcode(volatile const unsigned char * buffer)
{
  static const unsigned char failmessage1[] PROGMEM = "No GCODE Found\r\n";
  static const unsigned char failmessage2[] PROGMEM = "No G codes supported\r\n";
  static const unsigned char failmessage3[] PROGMEM = "M code not supported: ";
  volatile const unsigned char * rem;
  uint16_t code;
#ifdef HAS_CASE_LIGHT
  static const unsigned char okmessage[] PROGMEM = "OK\r\n";
  static const unsigned char failmessage4[] PROGMEM = "Required S option not found\r\n";
  static const unsigned char failmessage5[] PROGMEM = "Required P option not found\r\n";
  bool switchstate;
  /*  uint8_t duty_cycle; */
#endif
  
  rem=skipSpc(buffer);
  if (rem[0] == 'M' || rem[0] == 'G')
    {
      if (rem[0] == 'M')
	{
	  rem++;
	  code = matchto3num(rem);
	  rem  = skipNum(rem);
	  switch (code) {
#ifdef HAS_AMG88XX
	  case 105:
	    {
	      /* start the transfer. */
	      puts_P(okmessage);
	    }
#endif
#ifdef HAS_CASE_LIGHT
	  case 355:
	    {
	      rem = skipSpc(rem);
	      if (rem[0] == 'S')
		{
		  rem++;
		  switchstate = matchBoolNum(rem);
		  rem = skipNum(rem);
		  rem = skipSpc(rem);
		  if (rem[0] == 'P')
		    {
		      if (switchstate == true)
			caseLightOn();
		      else
			caseLightOff();
		      puts_P(okmessage);
		    }
		  else
		    puts_P(failmessage5);
		}
	      else
		puts_P(failmessage4);
	      break;
	    }
#endif
	  default:
	    {
	      puts_P(failmessage3);
	      U16toA(code);
	      putch('M');
	      puts_M(U16A);
	      putch('\r');
	      putch('\n');
	    }
	  }
	}
      else
	puts_P(failmessage2);
    }
  else
    {
      puts_P(failmessage1);
    }
}

